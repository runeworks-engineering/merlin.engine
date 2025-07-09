#!/usr/bin/env python3
import os
import numpy as np
import cv2
from stable_baselines3 import PPO
import gymnasium as gym
from gymnasium import spaces

class BabyCoverageEnv(gym.Env):
    """Simplified 2D Coverage Env for early learning (no training)."""
    metadata = {'render.modes': ['human']}
    def __init__(self, grid_size=64, max_steps=800):
        super().__init__()
        self.grid_size = grid_size
        self.max_steps = max_steps

        # curriculum
        self.curriculum_step = 0
        self.curriculum_thresholds = [0, 100_000, 200_000]
        self.completed = True

        # spaces
        self.action_space = spaces.Discrete(6)
        self.observation_space = spaces.Box(
            low=0, high=1,
            shape=(3, grid_size, grid_size),
            dtype=np.uint8
        )

        # state
        self.canvas = np.zeros((grid_size, grid_size), dtype=np.uint8)
        self.goal   = np.zeros_like(self.canvas)
        self.agent_pos = [grid_size//2, grid_size//2]
        self.extrude   = False
        self.step_count = 0

    def set_curriculum_step(self, step):
        self.curriculum_step = step

    def reset(self, *, seed=None, options=None):
        self.canvas.fill(0)
        self.step_count = 0
        self.agent_pos = [self.grid_size//2, self.grid_size//2]
        self.extrude   = False

        # sample new goal if needed
        if self.completed:
            if self.curriculum_step < self.curriculum_thresholds[1]:
                shapes = ['line']
            elif self.curriculum_step < self.curriculum_thresholds[2]:
                shapes = ['line','square']
            else:
                shapes = ['square','square','square']

            self.goal.fill(0)
            shape = np.random.choice(shapes)
            g = self.grid_size
            if shape == 'line':
                y = np.random.randint(g//4,3*g//4)
                x1,x2 = sorted(np.random.randint(g//6,5*g//6,2))
                self.goal[y,x1:x2]=1
            elif shape=='square':
                s = np.random.randint(g//5,g//3)
                cx,cy = np.random.randint(s,g-s,2)
                self.goal[cy-s:cy+s,cx-s:cx+s]=1
            else:  # circle
                cx,cy = np.random.randint(g//4,3*g//4,2)
                r = np.random.randint(g//6,g//4)
                yy,xx = np.ogrid[:g,:g]
                self.goal[(xx-cx)**2+(yy-cy)**2<=r*r]=1

        self.completed = False
        return self._get_obs(), {}

    def _get_obs(self):
        diff = np.clip(self.goal - self.canvas, 0, 1)
        return np.stack([self.canvas, self.goal, diff], axis=0)

    def step(self, action):
        x,y = self.agent_pos
        # move / toggle
        if   action==0 and y>0:           y-=1
        elif action==1 and y<self.grid_size-1: y+=1
        elif action==2 and x>0:           x-=1
        elif action==3 and x<self.grid_size-1: x+=1
        elif action==4:                   self.extrude=True
        elif action==5:                   self.extrude=False
        self.agent_pos=[x,y]

        # extrusion
        reward=0
        if self.extrude and self.goal[y,x] and not self.canvas[y,x]:
            reward=1
            self.canvas[y,x]=1

        self.step_count+=1
        done = (self.step_count>=self.max_steps)
        if done:
            # coverage check
            tot = self.goal.sum()
            cov = np.logical_and(self.goal,self.canvas).sum() / tot if tot>0 else 1.0
            self.completed = (cov>=0.95)

        return self._get_obs(), reward, done, False, {}

    def render(self, mode='human'):
        vis = np.stack([self.goal*255, self.canvas*255, np.zeros_like(self.canvas)],-1)
        x,y = self.agent_pos
        vis[y,x]=[0,255,0]
        vis = cv2.resize(vis,(512,512),interpolation=cv2.INTER_NEAREST)
        cv2.imshow("Eval",vis)
        cv2.waitKey(1)

    def close(self):
        cv2.destroyAllWindows()


def run_phase(model, phase, render=True):
    env = BabyCoverageEnv()
    env.set_curriculum_step(phase)
    obs,_ = env.reset()
    total_reward = 0
    done = False
    while not done:
        action, _ = model.predict(obs, deterministic=True)
        obs, r, done, _, _ = env.step(int(action))
        total_reward += r
        if render:
            env.render()
    env.close()
    print(f"Phase {phase}: total reward = {total_reward}")

def main():
    # load model (CPU or change to "cuda")
    path = "ppo_baby_coverage.zip"
    if not os.path.exists(path):
        raise FileNotFoundError(path)
    model = PPO.load(path, device="cpu")

    for phase in [0, 100_000, 200_000]:
        run_phase(model, phase, render=True)

if __name__=="__main__":
    main()
