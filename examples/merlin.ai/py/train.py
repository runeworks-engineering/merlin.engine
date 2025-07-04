import gymnasium as gym
from gymnasium import spaces
import numpy as np
import zmq
import cv2
import signal
import os

# Import your RL agent library
# Example for Stable-Baselines3:
#from stable_baselines3 import PPO  # or DQN, etc.
from stable_baselines3 import SAC  # or DQN, etc.
from stable_baselines3.common.env_checker import check_env

from collections import deque

# For statistics
episode_rewards = []
episode_goal_diffs = []
episode_paths = []
episode_extrusions = []
episode_avg_v = []
episode_avg_e = []


class AdditiveManufacturingEnv(gym.Env):
    """Custom Gym environment for C++ additive manufacturing sim."""
    metadata = {'render.modes': ['human']}
    
    def __init__(self, zmq_addr="tcp://localhost:5555", img_shape=(64,64,3), verbose=True):
        super().__init__()
        self.img_shape = img_shape
        self.verbose = verbose
        self.total_volume_extruded = 0
        self.total_path = 0
        self.max_velocity = 1000.0       # units/sec, adjust as needed
        self.print_area = 100.0         # in sim units; nozzle can go from -100 to +100
        self.position = np.array([0.0, 0.0, 0.0], dtype=np.float32)  # or sim default
        self.sim_steps_per_agent_step = 20  # or 10, etc.
        self.last_similarity = 0
        self.speed_history = []
        self.prev_delta = np.zeros(2, dtype=np.float32)

        # Action: X, Y are velocity controls (customize range!), E is flow rate 0-100
        self.action_space = spaces.Box(
            low=np.array([-self.print_area, -self.print_area, 0.0]),
            high=np.array([ self.print_area,  self.print_area, 150.0]),  # (X, Y, E)
            dtype=np.float32
        )

        self.observation_space = spaces.Dict({
            "current": spaces.Box(0, 255, shape=img_shape, dtype=np.uint8),
            "goal": spaces.Box(0, 255, shape=img_shape, dtype=np.uint8)
        })
        
        # ZMQ client
        self.ctx = zmq.Context()
        self.sock = self.ctx.socket(zmq.REQ)
        self.sock.connect(zmq_addr)
        
        self.step_count = 0
        self.max_steps = 0
        self.cum_reward = 0.0

    def reset(self, *, seed=None, options=None):
        self.sock.send_json({"type": "reset"})
        _ = self.sock.recv()
        self.step_count = 0
        self.cum_reward = 0.0
        self.total_path = 0.0
        self.total_volume_extruded = 0
        self.last_similarity = 0
        self.position = np.array([0.0, 0.0], dtype=np.float32)  # or sim default
        obs = self._get_obs()

        self.last_similarity = np.mean(np.abs(obs["current"].astype(float) - obs["goal"].astype(float)))
        self.prev_current = obs["current"][:,:,0].copy()

        self.episode_velocities = []
        self.episode_extrusions = []
        self.episode_goal_diffs = []
        self.episode_reward = 0.0

        if self.verbose:
            print("[RESET] Environment reset.")
        return obs, {}

    def setMaxSteps(self, maxS):
        self.max_steps = maxS

    def _get_obs(self):
        self.sock.send_json({"type": "get_images"})
        current = self.sock.recv()
        goal = self.sock.recv()
        current_img = np.frombuffer(current, dtype=np.uint8).reshape(self.img_shape)
        goal_img = np.frombuffer(goal, dtype=np.uint8).reshape(self.img_shape)
        return {"current": current_img, "goal": goal_img}

    def step(self, action):
        # 1. Apply action to environment
        velocity_xy = np.array(action[:2], dtype=np.float32)
        e_flow = action[2]

        velocity_mag = np.linalg.norm(velocity_xy)
        extrusion_mag = abs(e_flow)
        if velocity_mag > self.max_velocity:
            velocity_xy = velocity_xy / velocity_mag * self.max_velocity

        sim_action = np.array([velocity_xy[0], velocity_xy[1], e_flow])
        self.position += velocity_xy
        self.total_path += np.linalg.norm(velocity_xy)
        self.total_volume_extruded += e_flow

        self.sock.send_json({"type": "step", "xye": sim_action.tolist()})
        _ = self.sock.recv()

        # 2. Get new observation
        obs = self._get_obs()
        self.render()

        # 3. Reward Calculation
        # -- Progress-based reward --
        goal_diff_prev = self.last_similarity
        goal_diff = np.mean(np.abs(obs["current"].astype(float) - obs["goal"].astype(float)))
        progress = goal_diff_prev - goal_diff
        progress_reward = progress * 200.0  # Strong reward for making progress!
        self.last_similarity = goal_diff

        # -- Pixel-level reward for newly matched pixels --
        current = obs["current"][:, :, 0]
        goal = obs["goal"][:, :, 0]
        newly_filled = np.logical_and(
            (current == goal) & (goal == 255),
            (self.prev_current != goal)
        ).sum()
        pixel_fill_reward = newly_filled * 1.0  # Add 1.0 for each new correct pixel
        self.prev_current = current.copy()

        # -- Activity and penalty shaping --
        idle_penalty = -0.05 if velocity_mag < 1e-2 and extrusion_mag < 1e-2 else 0
        productive_movement_reward = 0.02 if velocity_mag >= 1e-2 and extrusion_mag >= 1e-2 else 0
        smoothness_penalty = -0.01 * np.linalg.norm(velocity_xy - getattr(self, 'prev_velocity', velocity_xy))
        self.prev_velocity = velocity_xy
        blob_penalty = -0.05 if velocity_mag < 1e-2 and extrusion_mag >= 1e-2 else 0

        # -- Penalize wasteful movement/extrusion if not making progress --
        waste_penalty = 0
        if progress <= 0:
            waste_penalty -= 0.05 * extrusion_mag
            waste_penalty -= 0.05 * velocity_mag

        # -- Combine all rewards --
        reward = (
            progress_reward
            + pixel_fill_reward
            + productive_movement_reward
            + idle_penalty
            + smoothness_penalty
            + blob_penalty
            + waste_penalty
        )

        # 4. Statistics for logging
        self.episode_velocities.append(velocity_mag)
        self.episode_extrusions.append(extrusion_mag)
        self.episode_goal_diffs.append(goal_diff)
        self.episode_reward += reward

        # 5. Step and done logic
        self.step_count += 1
        self.sock.send_json({"type": "is_done"})
        done = self.sock.recv_json()["done"]

        if self.step_count >= self.max_steps:
            if goal_diff < 20:  # reward for good final similarity
                reward += 10.0
            done = True
            print(f"[EPISODE END] Path traveled: {self.total_path:.2f}, Total flow: {self.total_volume_extruded:.2f}")

        # 6. Logging info
        if done:
            info = {
                "episode_reward": self.episode_reward,
                "final_goal_diff": goal_diff,
                "avg_velocity": np.mean(self.episode_velocities) if self.episode_velocities else 0.0,
                "avg_extrusion": np.mean(self.episode_extrusions) if self.episode_extrusions else 0.0,
                "total_path": self.total_path,
                "total_extruded": self.total_volume_extruded,
                "steps": self.step_count
            }
        else:
            info = {}

        if self.verbose:
            print(
                f"[STEP {self.step_count}] R: {reward:.2f} | Progress: {progress:.4f} | "
                f"Fill: {newly_filled} | Diff: {goal_diff:.2f} | V: {velocity_mag:.2f} | E: {extrusion_mag:.2f}"
            )

        return obs, reward, done, False, info


    def render(self, mode='human'):
        obs = self._get_obs()
        cv2.imshow("Current", obs["current"])
        cv2.imshow("Goal", obs["goal"])
        cv2.waitKey(1)
    
    def close(self):
        self.sock.close()
        self.ctx.term()
        if self.verbose:
            print(f"[STATS] Total steps: {self.step_count}, Total cumulative reward: {self.cum_reward:.3f}")
            print("[CLOSE] Environment closed.")

# ---- Main loop with debug ----

env = AdditiveManufacturingEnv(verbose=True)

num_epochs = 1000
steps_per_epoch = 180

env.setMaxSteps(steps_per_epoch)

obs, info = env.reset()
done = False

check_env(env)  # Optional: checks Gym API compliance

# Train with PPO agent
#model = PPO("MultiInputPolicy", env, verbose=1, tensorboard_log="./ppo_additive_manufacturing_tb")


model = SAC("MultiInputPolicy", env, verbose=1, tensorboard_log="./sac_additive_manufacturing_tb", buffer_size=2000)

# Add signal handler to save model on Ctrl+C or sim stop
def save_model_on_exit(*args):
    print("[INFO] Saving model...")
    model.save("ppo_additive_manufacturing")
    env.close()
    print("[INFO] Model saved. Exiting.")
    exit(0)
    

signal.signal(signal.SIGINT, save_model_on_exit)
signal.signal(signal.SIGTERM, save_model_on_exit)




for epoch in range(num_epochs):
    print(f"\n[TRAINING] Starting epoch {epoch+1}/{num_epochs}")
    obs, info = env.reset()
    done = False
    while not done:
        action, _ = model.predict(obs, deterministic=False)
        obs, reward, done, _, info = env.step(action)
    # Episode finished, log statistics
    if info:
        episode_rewards.append(info["episode_reward"])
        episode_goal_diffs.append(info["final_goal_diff"])
        episode_avg_v.append(info["avg_velocity"])
        episode_avg_e.append(info["avg_extrusion"])
        episode_paths.append(info["total_path"])
        episode_extrusions.append(info["total_extruded"])
        print(
            f"[EPOCH {epoch+1}] Reward: {info['episode_reward']:.2f}, "
            f"Final goal diff: {info['final_goal_diff']:.2f}, "
            f"Avg v: {info['avg_velocity']:.2f}, "
            f"Avg E: {info['avg_extrusion']:.2f}, "
            f"Total path: {info['total_path']:.2f}, "
            f"Total extruded: {info['total_extruded']:.2f}"
        )
    # Save model, etc.
    model.save(f"sac_additive_manufacturing_epoch{epoch+1}")

print("[INFO] Training finished.")
model.save("sac_additive_manufacturing_final")
env.close()

# while not done:
#     # Sample random action (replace with agent logic)
#     action = env.action_space.sample()
#     obs, reward, done, _, _ = env.step(action)
#     env.render()
# env.close()
