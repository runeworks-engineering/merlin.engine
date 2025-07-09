import os
import json
import signal

import gymnasium as gym
from gymnasium import spaces
import numpy as np
import zmq
import cv2
import math

from sb3_contrib import RecurrentPPO
from stable_baselines3.common.env_checker import check_env

def create_difference_overlay(current, goal, threshold=128):
    """
    current, goal: np.array, shape (H,W,3) or (H,W), dtype uint8
    threshold: int, threshold for 'on' pixel (default 128)
    Returns: np.array, shape (H,W,3), overlay image
    """
    if current.ndim == 3:
        current_gray = current[:, :, 0]
    else:
        current_gray = current
    if goal.ndim == 3:
        goal_gray = goal[:, :, 0]
    else:
        goal_gray = goal

    current_mask = current_gray >= threshold
    goal_mask = goal_gray >= threshold

    # Create blank RGB image
    overlay = np.zeros((*current_gray.shape, 3), dtype=np.uint8)

    # Green: correct (both on)
    overlay[(current_mask & goal_mask)] = (0, 255, 0)
    # Red: over-extruded (current on, goal off)
    overlay[(current_mask & ~goal_mask)] = (0, 0, 255)
    # Optional: Blue for under-extruded (goal on, current off)
    overlay[(~current_mask & goal_mask)] = (255, 0, 0)
    # Black otherwise
    return overlay

class AdditiveManufacturingEnv(gym.Env):
    metadata = {'render.modes': ['human']}

    def __init__(self,
                 zmq_addr: str = "tcp://localhost:5555",
                 img_shape=(128, 128, 3),
                 verbose: bool = True):
        super().__init__()
        self.img_shape = img_shape
        self.verbose = verbose

        self.nozzle_mm = 25.0  # nozzle diameter in mm

        # --- Manual offset/scaling for image alignment ---
        #128
        self.offset_mm = (-50, 0)      # <<< Tune these: (x_offset_px, y_offset_px)
        self.scale_px_per_mm = 0.65     # <<< Tune this: pixels per mm

        #256
        #self.offset_mm = (-50, 0)      # <<< Tune these: (x_offset_px, y_offset_px)
        #self.scale_px_per_mm = 0.65*2.0     # <<< Tune this: pixels per mm

        self.offset_px = (self.offset_mm[0] * self.scale_px_per_mm, self.offset_mm[1] * self.scale_px_per_mm)

        self.max_steps = None
        self.episode_count = 0

        # --- ZMQ sim interface ---
        self.ctx = zmq.Context()
        self.sock = self.ctx.socket(zmq.REQ)
        self.sock.connect(zmq_addr)
        self.last_current_mask = None
        # --- Gym spaces ---
        self.print_area = 80.0
        self.action_space = spaces.Box(low=np.array([-1.0]), high=np.array([1.0]), dtype=np.float32)
        self.observation_space = spaces.Dict({
            "current": spaces.Box(0, 255, shape=img_shape, dtype=np.uint8),
            "goal":    spaces.Box(0, 255, shape=img_shape, dtype=np.uint8),
            "nozzle":  spaces.Box(
                low=np.array([0.0, 0.0]), high=np.array([300, 200]), dtype=np.float32)
        })

    def reset(self, *, seed=None, options=None):
        self.episode_count += 1
        self.sock.send_json({"type": "reset"})
        _ = self.sock.recv()
        self.step_count = 0
        obs = self._get_obs()
        self.last_current_mask = None
        return obs, {}

    def setMaxSteps(self, max_steps: int):
        self.max_steps = max_steps

    def _get_obs(self):
        self.sock.send_json({"type": "get_images"})
        curr = self.sock.recv()
        goal = self.sock.recv()
        curr_img = np.frombuffer(curr, dtype=np.uint8).reshape(self.img_shape)
        goal_img = np.frombuffer(goal, dtype=np.uint8).reshape(self.img_shape)

        # Get nozzle position (as in your step function)
        self.sock.send_json({"type": "get_nozzle_position"})
        pos_msg = self.sock.recv()
        pos = json.loads(pos_msg)
        nozzle_x_mm = np.clip(pos['x'], 0, 300)
        nozzle_y_mm = np.clip(pos['y'], 0, 300)

        nozzle_x_px, nozzle_y_px = self._world_to_img(nozzle_x_mm, nozzle_y_mm)
        nozzle_x_px = np.clip(nozzle_x_px, 0, self.img_shape[0])
        nozzle_y_px = np.clip(nozzle_y_px, 0, self.img_shape[1])

        return {
            "current": curr_img,
            "goal": goal_img,
            "nozzle": np.array([nozzle_x_px, nozzle_y_px], dtype=np.float32)
        }

    def _world_to_img(self, x_mm, y_mm):
        x_px = int(x_mm * self.scale_px_per_mm + self.offset_px[0])
        y_px = int(y_mm * self.scale_px_per_mm + self.offset_px[1])
        return x_px, y_px

    def render(self, mode='human'):
        obs = self._get_obs()
        img = obs["current"].copy()
        if hasattr(self, "_last_nozzle_pos_px"):
            x, y = self._last_nozzle_pos_px
            r = self._last_nozzle_radius_px
            img = cv2.circle(img, (int(x), int(y)), int(r), (0, 0, 255), 2)

        cv2.imshow("Current (with nozzle)", img)
        cv2.imshow("Goal", obs["goal"])

        overlay = create_difference_overlay(obs['current'], obs['goal'])
        cv2.imshow("Difference Overlay", overlay)
        cv2.waitKey(1)

    def step(self, action):
        e_flow = float(np.clip((action[0]*0.5 + 0.5)*3.0, 0.0, 3.0))
        # Send action, receive nozzle position
        self.sock.send_json({"type": "step", "xye": [0, 0, e_flow]})
        _ = self.sock.recv()

        obs = self._get_obs()

        pos = obs["nozzle"].copy()
        nozzle_x_px = pos[0]
        nozzle_y_px = pos[1]
        nozzle_radius_px = int((self.nozzle_mm / 2) * self.scale_px_per_mm)
        
        # Compute reward
        current_mask = obs['current'][:, :, 0] >= 128
        goal_mask = obs['goal'][:, :, 0] >= 128
        #goal_now = obs['goal'][:, :, 0] >= 128

        # Build a circular mask for the nozzle area
        Y, X = np.ogrid[:self.img_shape[0], :self.img_shape[1]]
        dist = np.sqrt((X - nozzle_x_px) ** 2 + (Y - nozzle_y_px) ** 2)
        area_mask = dist <= nozzle_radius_px

        current_in_area = None
        if self.last_current_mask is not None:
            current_in_area = current_mask & (~self.last_current_mask) & area_mask
        else:
            current_in_area = current_mask & area_mask
        # Restrict to nozzle area for local reward
       
        goal_in_area = goal_mask & area_mask

        green = current_in_area & goal_in_area
        red = current_in_area & ~goal_in_area

        num_green = np.sum(green)
        num_red = np.sum(red)
        num_area = np.sum(area_mask)

        self.last_current = current_mask
        # Convert masks to uint8 images
        #curr_area_img = (current_in_area.astype(np.uint8)) * 255
        #goal_area_img = (goal_in_area.astype(np.uint8)) * 255
#
        #overlay = create_difference_overlay(curr_area_img, goal_area_img)
        #cv2.imshow("Goal", overlay)
        #cv2.waitKey(1)

        # Main reward: penalize bad (red) more than reward good (green)
        reward = (1.0 * num_green - 2.0 * num_red) / (num_area + 1e-6)
        
        reward -= 0.01 * e_flow  # tune the 0.05 as needed

        self.step_count += 1
        self.sock.send_json({"type": "is_done"})
        done = self.sock.recv_json().get("done", False)
        if not done and self.max_steps and self.step_count >= self.max_steps:
            done = True

        info = {}
        if done:
            #curr = obs["current"].astype(float)
            #goal = obs["goal"].astype(float)
            #mean_diff = float(np.mean(np.abs(curr - goal)))
            #reward = -5.0 * (mean_diff / 255.0)
            #reward = np.clip(reward, -10, 10)

            
            ful_green = current_mask & goal_mask
            ful_red = current_mask & ~goal_mask

            ful_num_green = np.sum(green)
            ful_num_red = np.sum(red)
            ful_num_area = np.sum(goal_mask)

            # Main reward: penalize bad (red) more than reward good (green)
            reward = (10.0 * ful_num_green - 50.0 * ful_num_red) / (ful_num_area + 1e-6)

            info = {
                "episode_reward": reward,
                "steps": self.step_count
            }
            cv2.imwrite(f"./episodes/episode_{self.episode_count}_goal.png", obs["goal"])
            cv2.imwrite(f"./episodes/episode_{self.episode_count}_current.png", obs["current"])
            overlay = create_difference_overlay(obs['current'], obs['goal'])
            cv2.imwrite(f"./episodes/episode_{self.episode_count}_metrics.png", overlay)

        if self.verbose:
            print(f"[STEP {self.step_count}] reward={reward:+.3f}, E={e_flow:+.3f}")

        # Save nozzle overlay for use in render()
        self._last_nozzle_pos_px = (nozzle_x_px, nozzle_y_px)
        self._last_nozzle_radius_px = nozzle_radius_px
        #self.render()

        return obs, reward, done, False, info

    def close(self):
        self.sock.close()
        self.ctx.term()


if __name__ == "__main__":
    env = AdditiveManufacturingEnv(verbose=True)
    env.setMaxSteps(400)
    check_env(env)

    os.makedirs("./episodes", exist_ok=True)

    checkpoint = "rppo_flow_latest"
    if os.path.exists(checkpoint + ".zip"):
        model = RecurrentPPO.load(checkpoint, env=env)
        print(f"Resumed from {checkpoint}.zip")
    else:
        model = RecurrentPPO("MultiInputLstmPolicy", env,
                    verbose=1,
                    tensorboard_log="./tb_rppo_flow")

    def _save_and_exit(*_):
        print("Saving model...")
        model.save(checkpoint)
        env.close()
        exit()
    signal.signal(signal.SIGINT, _save_and_exit)
    signal.signal(signal.SIGTERM, _save_and_exit)

    model.learn(total_timesteps=200_000)
    model.save(checkpoint)
    env.close()
