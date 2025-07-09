import os
import signal
import numpy as np
import gymnasium as gym
from gymnasium import spaces
import cv2
import torch
import multiprocessing
from stable_baselines3 import PPO
from stable_baselines3.common.env_checker import check_env
from stable_baselines3.common.vec_env import SubprocVecEnv
import time
device = "cuda" if torch.cuda.is_available() else "cpu"

class BabyCoverageEnv(gym.Env):
    """Simplified 2D Coverage Env for early learning."""
    metadata = {'render.modes': ['human']}

    def __init__(self, grid_size=64, max_steps=200, verbose=False):
        super().__init__()
        self.grid_size = grid_size
        self.max_steps = max_steps
        self.verbose = verbose

        # Curriculum phase
        self.curriculum_step = 0
        self.curriculum_thresholds = [0, 100_000, 200_000]  # After these timesteps, increase difficulty

        # Action space: 0=Up, 1=Down, 2=Left, 3=Right, 4=Extrude On, 5=Extrude Off
        self.action_space = spaces.Discrete(6)

        # Observation: current canvas + goal mask + diff (3x64x64)
        self.observation_space = spaces.Box(
            0, 1, shape=(3, grid_size, grid_size), dtype=np.uint8
        )

        self.completed = True  # start by sampling a new shape
        # State
        self.canvas = np.zeros((grid_size, grid_size), dtype=np.uint8)
        self.goal = np.zeros_like(self.canvas)
        self.agent_pos = [grid_size // 2, grid_size // 2]
        self.extrude = False
        self.step_count = 0
        self.prev_dist = None

    def set_curriculum_step(self, step):
        self.curriculum_step = step

    def _closest_unpainted_dist(self):
        # positions of all goal pixels not yet on canvas
        ys, xs = np.where((self.goal == 1) & (self.canvas == 0))
        if len(xs) == 0:
            return 0.0
        ax, ay = self.agent_pos
        # Manhattan distances
        dists = np.abs(xs - ax) + np.abs(ys - ay)
        return float(dists.min())

    def reset(self, *, seed=None, options=None):
        self.canvas.fill(0)
        self.step_count = 0
        self.agent_pos = [self.grid_size // 2, self.grid_size // 2]
        self.extrude = False
        # Only sample a new goal if the last one was completed
        if self.completed:
            # Determine difficulty and sample shape exactly as before
            if self.curriculum_step < self.curriculum_thresholds[1]:
                shape_choices = ['square']
            elif self.curriculum_step < self.curriculum_thresholds[2]:
                shape_choices = ['square', 'circle']
            else:
                shape_choices = ['square', 'circle', 'square']

            self.goal.fill(0)
            shape_type = np.random.choice(shape_choices)
            g = self.grid_size
            if shape_type == 'square':
                s = np.random.randint(g//6, g//4)
                cx, cy = np.random.randint(s, g-s), np.random.randint(s, g-s)
                self.goal[cy-s:cy+s, cx-s:cx+s] = 1
            elif shape_type == 'line':
                y = np.random.randint(g//6, 2*g//4)
                x1, x2 = sorted(np.random.randint(g//6, 5*g//6, size=2))
                self.goal[y, x1:x2] = 1
            elif shape_type == 'circle':
                cx, cy = np.random.randint(g//4, 2*g//4, size=2)
                r = np.random.randint(g//6, g//4)
                yy, xx = np.ogrid[:g, :g]
                mask = (xx - cx)**2 + (yy - cy)**2 <= r**2
                self.goal[mask] = 1

        # Reset the completion flag (we'll set it at episode end)
        self.completed = False
        return self._get_obs(), {}

    def _get_obs(self):
        diff = np.clip(self.goal - self.canvas, 0, 1)
        stacked = np.stack([self.canvas, self.goal, diff], axis=0)
        return stacked

    def step(self, action):
        x, y = self.agent_pos
        lx, ly = self.agent_pos
        # 1) Move / toggle extrude as before
        if action == 0 and y > 0:          y -= 1
        elif action == 1 and y < self.grid_size-1: y += 1
        elif action == 2 and x > 0:        x -= 1
        elif action == 3 and x < self.grid_size-1: x += 1
        elif action == 4:                  self.extrude = True
        elif action == 5:                  self.extrude = False
        self.agent_pos = [x, y]

        # Extrude
        reward = 0
        if self.extrude:
            if self.goal[y, x] and not self.canvas[y, x]:
                reward += 1
            else :
                reward -= 0.001
            self.canvas[y, x] = 1

        if self.goal[y, x] and not self.canvas[y, x]:
            reward += 0.1

        reward -= 0.1 # constant reward at each steps. (encourage faster completion)

        if(lx == x and ly == y) : reward -= 0.5 #punish not moving
        if(lx != x or ly != y) : reward += 0.1 #reward moving


        self.step_count += 1
        done = (self.step_count >= self.max_steps)
        #self.render()
        #reward += 0.1 * shaping

        # compute coverage fraction
        total = self.goal.sum()
        if total > 0:
            covered = np.logical_and(self.goal, self.canvas).sum()
            coverage = covered / total
        else:
            coverage = 1.0
        
        if(coverage > 0.95) :
            self.completed = True #(coverage >= 0.95)
            reward += 6
            done = True

        if(coverage > 0.5) :
            self.completed = True #(coverage >= 0.95)
            reward += 4

        if(coverage > 0.25) :
            reward += 2

        coverage = 0
        if done:
            self.prev_dist = None

        if self.verbose:
            print(f"Step {self.step_count}: Action={action} Pos=({x},{y}) Reward={reward} Coverage={coverage}")
            self.render()

        return self._get_obs(), reward, done, False, {}

    def render(self, mode='human'):
        vis = np.stack([
            self.goal * 255,
            self.canvas * 255,
            np.zeros_like(self.canvas)
        ], axis=-1)
        vis[self.agent_pos[1], self.agent_pos[0]] = [0, 255, 0]  # agent in green
        vis = cv2.resize(vis, (512, 512), interpolation=cv2.INTER_NEAREST)
        cv2.imshow("Baby Coverage Env", vis)
        cv2.waitKey(1)

    def close(self):
        cv2.destroyAllWindows()


if __name__ == "__main__":
    multiprocessing.freeze_support()
    def make_env(rank):
        def _init():
            return BabyCoverageEnv(grid_size=64, max_steps=600, verbose=False)
        return _init

    n_envs = 64
    vec_env = SubprocVecEnv([make_env(i) for i in range(n_envs)])

    checkpoint = "ppo_baby_coverage"
    if os.path.exists(checkpoint + ".zip"):
        model = PPO.load(checkpoint, device=device, env=vec_env)
        print("Resumed from saved model.")
    else:
        model = PPO(
            "CnnPolicy",
            vec_env,
            verbose=1,
            device=device,
            tensorboard_log="./tb_baby",
            policy_kwargs=dict(normalize_images=False)
        )

    try:
        total_steps = n_envs * 600 * 1_000
        phase = n_envs * 600
        viz_check = 0
        for start in range(0, total_steps, phase):
            vec_env.env_method("set_curriculum_step", start)
            model.learn(total_timesteps=phase, reset_num_timesteps=False)
            model.save(checkpoint)

            viz_env = BabyCoverageEnv(grid_size=64, max_steps=600, verbose=True)
            obs, _ = viz_env.reset()
            done = False

            while not done:
                # predict deterministically for a smoother demo
                action, _ = model.predict(obs)
                obs, reward, done, _, _ = viz_env.step(action)
                #time.sleep(0.01)               # slow it down a bit
            
            viz_env.close()


    except KeyboardInterrupt:
        print("\nInterrupted! Saving and closing…")
        model.save(checkpoint)
    finally:
        vec_env.close()
