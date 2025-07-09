import os
import json
import signal

import gymnasium as gym
from gymnasium import spaces
import numpy as np
import zmq
import cv2

from stable_baselines3 import SAC
from stable_baselines3.common.env_checker import check_env


class AdditiveManufacturingEnv(gym.Env):
    """Gym environment for 2D printing via ZMQ‐driven sim."""
    metadata = {'render.modes': ['human']}

    def __init__(self,
                 zmq_addr: str = "tcp://localhost:5555",
                 img_shape=(128, 128, 3),
                 verbose: bool = True):
        super().__init__()
        self.img_shape = img_shape
        self.verbose = verbose

        # --- State ---
        self.total_path = 0.0
        self.total_extruded = 0.0
        self.step_count = 0
        self.max_steps = None

        # --- ZMQ sim interface ---
        self.ctx = zmq.Context()
        self.sock = self.ctx.socket(zmq.REQ)
        self.sock.connect(zmq_addr)

        # --- Action & observation spaces ---
        self.print_area = 80.0
        self.action_space = spaces.Box(
            low=np.array([-self.print_area, -self.print_area, 0.0], dtype=np.float32),
            high=np.array([ self.print_area,  self.print_area, 150.0], dtype=np.float32),
        )
        self.observation_space = spaces.Dict({
            "current": spaces.Box(0, 255, shape=img_shape, dtype=np.uint8),
            "goal":    spaces.Box(0, 255, shape=img_shape, dtype=np.uint8),
        })

        # --- Reward weights & constants ---
        self.coverage_radius = 15.0
        self.pixel_area      = (200.0 / img_shape[0])**2
        self.dt              = 0.2

        self.lambda_area = 5.0   # max area reward
        self.lambda_tv   = 0.2   # total variation penalty
        self.lambda_obs  = 1.0   # pixel-diff penalty
        self.r_const     = 0.01  # time penalty

        # --- Episode buffers ---
        self.prev_mask      = None
        self.prev_TV        = 0.0
        self.last_similarity = 0.0

    def reset(self, *, seed=None, options=None):
        # 1) reset sim
        self.sock.send_json({"type": "reset"})
        _ = self.sock.recv()

        # 2) counters
        self.step_count = 0
        self.total_path = 0.0
        self.total_extruded = 0.0

        # 3) get initial obs
        obs = self._get_obs()

        # 4) init histories
        curr_gray = obs["current"][:, :, 0]
        goal_gray = obs["goal"][:, :, 0]
        self.prev_mask = curr_gray >= 128
        self.last_similarity = float(
            np.mean(np.abs(obs["current"].astype(float) - obs["goal"].astype(float)))
        )
        self.prev_TV = self._total_variation(self.prev_mask)

        return obs, {}

    def setMaxSteps(self, max_steps: int):
        self.max_steps = max_steps

    def _get_obs(self):
        self.sock.send_json({"type": "get_images"})
        curr = self.sock.recv()
        goal = self.sock.recv()

        curr_img = np.frombuffer(curr, dtype=np.uint8).reshape(self.img_shape)
        goal_img = np.frombuffer(goal, dtype=np.uint8).reshape(self.img_shape)
        return {"current": curr_img, "goal": goal_img}

    @staticmethod
    def _total_variation(mask: np.ndarray) -> float:
        m  = mask.astype(np.float32)
        dx = m[1:, :]  - m[:-1, :]
        dy = m[:, 1:]  - m[:, :-1]
        dx2 = dx[:, :-1]
        dy2 = dy[:-1, :]
        return float(np.sum(np.sqrt(dx2**2 + dy2**2)))

    def step(self, action):
        # --- 1) apply action ---
        vx, vy, e_flow = [float(a) for a in action]
        vel_mag = np.linalg.norm([vx, vy])

        # clip velocity
        if vel_mag > self.print_area:
            scale = self.print_area / vel_mag
            vx, vy = vx * scale, vy * scale
            vel_mag = self.print_area

        # send to sim
        self.sock.send_json({"type": "step", "xye": [vx, vy, e_flow]})
        _ = self.sock.recv()

        self.total_path    += vel_mag
        self.total_extruded+= abs(e_flow)

        # --- 2) get obs & prep for reward ---
        obs = self._get_obs()
        curr = obs["current"].astype(float)
        goal = obs["goal"].astype(float)

        # pixel‐diff term
        mean_diff = float(np.mean(np.abs(curr - goal)))
        R_obs = - self.lambda_obs * (mean_diff / 255.0)

        # binarize masks
        curr_gray = obs["current"][:, :, 0]
        curr_mask = curr_gray >= 128
        goal_mask = obs["goal"][:, :, 0] >= 128

        # area reward
        newly = goal_mask & curr_mask & (~self.prev_mask)
        A_new = float(newly.sum()) * self.pixel_area
        R_area = self.lambda_area * (A_new / (2 * self.coverage_radius * self.dt * self.print_area))

        # TV penalty
        TV_curr = self._total_variation(curr_mask)
        R_tv = - self.lambda_tv * ((TV_curr - self.prev_TV) / (2 * self.print_area * self.dt))

        # time penalty
        R_const = - self.r_const

        # total
        reward = R_area + R_tv + R_obs + R_const
        reward = float(np.clip(reward, -10.0, 10.0))

        # --- 3) update histories ---
        self.prev_mask       = curr_mask.copy()
        self.prev_TV         = TV_curr
        self.last_similarity = mean_diff
        self.step_count     += 1

        # --- 4) done flag ---
        self.sock.send_json({"type": "is_done"})
        done = self.sock.recv_json().get("done", False)
        if self.max_steps and self.step_count >= self.max_steps:
            done = True

        # --- 5) info dict ---
        info = {}
        if done:
            total_covered = int((curr_mask & goal_mask).sum())
            info = {
                "episode_reward": self.total_path,  # or sum of per-step rewards if you track
                "total_covered": total_covered,
                "steps": self.step_count
            }

        if self.verbose:
            print(f"[STEP {self.step_count}] "
                  f"A={R_area:+.3f}  TV={R_tv:+.3f}  OBS={R_obs:+.3f}  T={R_const:+.3f} → {reward:+.3f}")

        return obs, reward, done, False, info

    def render(self, mode='human'):
        obs = self._get_obs()
        cv2.imshow("Current", obs["current"])
        cv2.imshow("Goal", obs["goal"])
        cv2.waitKey(1)

    def close(self):
        self.sock.close()
        self.ctx.term()


if __name__ == "__main__":
    # ---- Setup ----
    env = AdditiveManufacturingEnv(verbose=True)
    env.setMaxSteps(400)
    check_env(env)

    checkpoint = "sac_2dprint_latest"
    if os.path.exists(checkpoint + ".zip"):
        model = SAC.load(checkpoint, env=env, buffer_size=2000)
        print(f"Resumed from {checkpoint}.zip")
    else:
        model = SAC("MultiInputPolicy", env,
                    verbose=1,
                    tensorboard_log="./tb_2dprint",
                    buffer_size=2000)

    # ---- Safe save on exit ----
    def _save_and_exit(*_):
        print("Saving model...")
        model.save(checkpoint)
        env.close()
        exit()
    signal.signal(signal.SIGINT, _save_and_exit)
    signal.signal(signal.SIGTERM, _save_and_exit)

    # ---- Train ----
    model.learn(total_timesteps=200_000)
    model.save(checkpoint)
    env.close()
