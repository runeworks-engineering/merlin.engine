import gymnasium as gym
from gymnasium import spaces
import numpy as np
import zmq
import cv2

class AdditiveManufacturingEnv(gym.Env):
    """Custom Gym environment for C++ additive manufacturing sim."""
    metadata = {'render.modes': ['human']}
    
    def __init__(self, zmq_addr="tcp://localhost:5555", img_shape=(128,128,3)):
        super().__init__()
        self.img_shape = img_shape
        
        # Action: XYZE - normalized or in sim units
        self.action_space = spaces.Box(
            low=np.array([-1, -1, -1, 0]), 
            high=np.array([1, 1, 1, 1]),
            dtype=np.float32
        )
        # Observation: Two images (current, goal)
        self.observation_space = spaces.Dict({
            "current": spaces.Box(0, 255, shape=img_shape, dtype=np.uint8),
            "goal": spaces.Box(0, 255, shape=img_shape, dtype=np.uint8)
        })
        
        # ZMQ client
        self.ctx = zmq.Context()
        self.sock = self.ctx.socket(zmq.REQ)
        self.sock.connect(zmq_addr)
        
    def reset(self, *, seed=None, options=None):
        self.sock.send_json({"type": "reset"})
        _ = self.sock.recv()
        obs = self._get_obs()
        return obs, {}

    def _get_obs(self):
        self.sock.send_json({"type": "get_images"})
        imgs = self.sock.recv_pyobj()
        current, goal = imgs['current'], imgs['goal']
        return {"current": current, "goal": goal}

    def step(self, action):
        # Send action to C++ sim
        self.sock.send_json({"type": "step", "xyze": action.tolist()})
        _ = self.sock.recv()  # Wait for ack
        
        # Get new observation
        obs = self._get_obs()
        
        # Reward: Negative pixel-wise difference (simple baseline)
        reward = -np.mean(np.abs(obs["current"].astype(float) - obs["goal"].astype(float)))
        
        # Check if done
        self.sock.send_json({"type": "is_done"})
        done = self.sock.recv_json()["done"]
        
        return obs, reward, done, False, {}
    
    def render(self, mode='human'):
        # Just show the current and goal images
        obs = self._get_obs()
        cv2.imshow("Current", obs["current"])
        cv2.imshow("Goal", obs["goal"])
        cv2.waitKey(1)
    
    def close(self):
        self.sock.close()
        self.ctx.term()


env = AdditiveManufacturingEnv()
obs, info = env.reset()
done = False

while not done:
    action = env.action_space.sample()  # Replace with RL agent
    obs, reward, done, _, _ = env.step(action)
    env.render()
env.close()