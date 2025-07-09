import os
import numpy as np
import torch
import torch.nn as nn
import torch.optim as optim
from torch.utils.data import Dataset, DataLoader
from tqdm import tqdm
import cv2

# ================== CONFIG ==================
GRID_SIZE = 64
MAX_STEPS = 200
DEVICE = torch.device("cuda" if torch.cuda.is_available() else "cpu")
NUM_CURRICULA = 6
EPISODES_PER_STAGE = 100
BATCH_SIZE = 64
LR = 1e-4
EPOCHS = 500
MODEL_PATH = "imitation_model.pth"

# ========== ENVIRONMENT ==========
class BabyCoverageEnv:
    def __init__(self, grid_size=GRID_SIZE):
        self.grid_size = grid_size
        self.canvas = np.zeros((grid_size, grid_size), dtype=np.uint8)
        self.goal = np.zeros_like(self.canvas)
        self.agent_pos = [grid_size // 2, grid_size // 2]
        self.extrude = True
        self.step_count = 0

    def reset(self, curriculum_stage=0):
        self.canvas.fill(0)
        self.goal.fill(0)
        self.agent_pos = [self.grid_size // 2, self.grid_size // 2]
        self.step_count = 0
        self._set_goal(curriculum_stage)
        return self._get_obs()

    def _set_goal(self, stage):
        g = self.grid_size
        cx, cy = g // 2, g // 2
        size = g // 4
        if stage == 0:
            size = g // 4
            cx, cy = g // 2, g // 2
        elif stage == 1:
            size = g // 5
            cx = np.random.randint(g // 3, 2 * g // 3)
            cy = np.random.randint(g // 3, 2 * g // 3)
        elif stage == 2:
            size = g // 6
            cx = np.random.randint(size, g - size)
            cy = np.random.randint(size, g - size)
        elif stage == 3:
            size = g // 8
            cx = np.random.randint(size, g - size)
            cy = np.random.randint(size, g - size)
        elif stage == 4:
            size = np.random.randint(g // 12, g // 8)
            cx = np.random.randint(size, g - size)
            cy = np.random.randint(size, g - size)
        else:
            size = np.random.randint(g // 20, g // 10)
            cx = np.random.randint(size, g - size)
            cy = np.random.randint(size, g - size)

        min_y = max(cy - size, 0)
        max_y = min(cy + size, g)
        min_x = max(cx - size, 0)
        max_x = min(cx + size, g)
        self.goal.fill(0)
        self.goal[min_y:max_y, min_x:max_x] = 1

    def _get_obs(self):
        diff = np.clip(self.goal - self.canvas, 0, 1)
        return np.stack([self.canvas, self.goal, diff], axis=0).astype(np.float32)

    def render(self, window_name="Env", scale=8):
        vis = np.stack([self.goal * 255, self.canvas * 255, np.zeros_like(self.canvas)], axis=-1)
        x, y = self.agent_pos
        if 0 <= x < self.grid_size and 0 <= y < self.grid_size:
            vis[y, x] = [0, 255, 0]
        vis = cv2.resize(vis, (self.grid_size * scale, self.grid_size * scale), interpolation=cv2.INTER_NEAREST)
        cv2.imshow(window_name, vis)
        cv2.waitKey(1)

    def step(self, action):
        x, y = self.agent_pos
        if action == 0 and y > 0:      # up
            y -= 1
        elif action == 1 and y < self.grid_size - 1: # down
            y += 1
        elif action == 2 and x > 0:   # left
            x -= 1
        elif action == 3 and x < self.grid_size - 1: # right
            x += 1
        elif action == 4: # pen down
            self.extrude = True
        elif action == 5: # pen up
            self.extrude = False
        self.agent_pos = [x, y]
        if self.extrude and action in [0, 1, 2, 3]:
            self.canvas[y, x] = 1
        self.step_count += 1
        return self._get_obs()

    def force_draw(self):
        """Draw at current agent position if pen is down."""
        if self.extrude:
            x, y = self.agent_pos
            self.canvas[y, x] = 1

    def close(self):
        cv2.destroyAllWindows()

# ========== EXPERT POLICY ==========

def move_agent_to(env, target_x, target_y, data=None, record=False):
    """Moves agent (without drawing) to target [x, y]."""
    while True:
        x, y = env.agent_pos
        if (x, y) == (target_x, target_y):
            break
        if x < target_x:
            a = 3 # right
        elif x > target_x:
            a = 2 # left
        elif y < target_y:
            a = 1 # down
        else:
            a = 0 # up
        obs = env._get_obs().copy()
        env.step(a)
        if record and data is not None:
            data.append((obs, a))

def expert_raster_policy(env: BabyCoverageEnv, stage=0):
    """
    Draws the rectangle as per goal, matching the goal position.
    """
    ys, xs = np.where(env.goal == 1)
    if xs.size == 0:
        return []

    min_x, max_x = xs.min(), xs.max()
    min_y, max_y = ys.min(), ys.max()

    data = []
    orig_extrude = env.extrude

    # --- Move to (min_x, min_y) with pen up ---
    env.extrude = False
    move_agent_to(env, min_x, min_y, data, record=True)

    # Pen down and force draw at the starting cell
    obs = env._get_obs().copy()
    env.extrude = True
    data.append((obs, 4)) # Pen down
    env.force_draw()
    obs = env._get_obs().copy()
    # You can optionally record a 'noop' (dummy action) here if needed, but for learning this is fine

    # --- Draw perimeter (clockwise) ---
    # Top edge
    for x in range(min_x + 1, max_x + 1):
        obs = env._get_obs().copy()
        env.step(3) # right
        data.append((obs, 3))
    # Right edge
    for y in range(min_y + 1, max_y + 1):
        obs = env._get_obs().copy()
        env.step(1) # down
        data.append((obs, 1))
    # Bottom edge
    for x in range(max_x - 1, min_x - 1, -1):
        obs = env._get_obs().copy()
        env.step(2) # left
        data.append((obs, 2))
    # Left edge
    for y in range(max_y - 1, min_y, -1):
        obs = env._get_obs().copy()
        env.step(0) # up
        data.append((obs, 0))

    # --- Fill interior (snake pattern) ---
    for y in range(min_y + 1, max_y):
        if (y - min_y) % 2 == 0:
            x_range = range(min_x + 1, max_x)
            direction = 3 # right
        else:
            x_range = range(max_x - 1, min_x, -1)
            direction = 2 # left
        for x in x_range:
            obs = env._get_obs().copy()
            env.step(direction)
            data.append((obs, direction))

    # --- Pen up at end ---
    obs = env._get_obs().copy()
    env.extrude = False
    data.append((obs, 5)) # Pen up

    env.extrude = orig_extrude
    return data

# ========== VISUALIZATION ==========
def visualize_expert(env: BabyCoverageEnv, stage: int, save_dir="viz"):
    os.makedirs(save_dir, exist_ok=True)
    env.reset(curriculum_stage=stage)
    data = expert_raster_policy(env, stage)
    env.reset(curriculum_stage=stage)
    env.extrude = True
    for _, a in data:
        env.step(a)
        env.render(window_name=f"Stage {stage}", scale=8)
    env.close()

# ========== DATASET ==========
class ImitationDataset(Dataset):
    def __init__(self, samples):
        obs, acts = zip(*samples)
        self.obs = torch.tensor(obs, dtype=torch.float32)
        self.acts = torch.tensor(acts, dtype=torch.long)

    def __len__(self):
        return len(self.acts)

    def __getitem__(self, idx):
        return self.obs[idx], self.acts[idx]

# ========== MODEL ==========
class CNNPolicy(nn.Module):
    def __init__(self, n_actions=6):
        super().__init__()
        self.net = nn.Sequential(
            nn.Conv2d(3, 32, 3, 2), nn.ReLU(),
            nn.Conv2d(32, 64, 3, 2), nn.ReLU(),
            nn.Flatten(), nn.Linear(64 * 15 * 15, 128), nn.ReLU(),
            nn.Linear(128, n_actions)
        )
    def forward(self, x):
        return self.net(x)

# ========== TRAIN & EVAL ==========
def train():
    env = BabyCoverageEnv()
    dataset = []

    model = CNNPolicy().to(DEVICE)

    if os.path.exists(MODEL_PATH):
        try:
            model.load_state_dict(torch.load(MODEL_PATH, map_location=DEVICE))
            print(f"Loaded existing model from {MODEL_PATH}")
        except Exception as e:
            print(f"Warning: failed to load model from {MODEL_PATH}, starting fresh. Error:\n{e}")

    try:
        for stage in range(NUM_CURRICULA):
            print(f"Collecting data for stage {stage}")
            for _ in tqdm(range(EPISODES_PER_STAGE)):
                env.reset(curriculum_stage=stage)
                episode_data = expert_raster_policy(env, stage)
                dataset += episode_data
            env.reset(curriculum_stage=stage)
            visualize_expert(env, stage)
        print(f"Total samples collected: {len(dataset)}")

        if not dataset:
            raise RuntimeError("No data collected from expert! Cannot train.")

        ds = ImitationDataset(dataset)
        loader = DataLoader(ds, BATCH_SIZE, shuffle=True)

        optimizer = optim.Adam(model.parameters(), lr=LR)
        loss_fn = nn.CrossEntropyLoss()

        for epoch in range(1, EPOCHS + 1):
            total_loss = 0.0
            for x, y in loader:
                x, y = x.to(DEVICE), y.to(DEVICE)
                preds = model(x)
                loss = loss_fn(preds, y)
                optimizer.zero_grad()
                loss.backward()
                optimizer.step()
                total_loss += loss.item()
            print(f"Epoch {epoch}/{EPOCHS} - Loss: {total_loss/len(loader):.4f}")

    except KeyboardInterrupt:
        print("\nTraining interrupted. Saving model...")

    finally:
        torch.save(model.state_dict(), MODEL_PATH)
        print(f"Model saved to {MODEL_PATH}")

def evaluate():
    env = BabyCoverageEnv()
    model = CNNPolicy().to(DEVICE)
    
    if os.path.exists(MODEL_PATH):
        print(f"Loading model from {MODEL_PATH}")
        model.load_state_dict(torch.load(MODEL_PATH, map_location=DEVICE))
    else:
        print(f"No model file found at {MODEL_PATH}. Creating a new untrained model.")
        torch.save(model.state_dict(), MODEL_PATH)
        print(f"Saved untrained model to {MODEL_PATH}")

    model.eval()
    obs = env.reset()
    for _ in range(MAX_STEPS):
        inp = torch.tensor(obs).unsqueeze(0).to(DEVICE)
        with torch.no_grad():
            action = model(inp).argmax().item()
        obs = env.step(action)
        env.render()
    env.close()

if __name__ == "__main__":
    import argparse
    parser = argparse.ArgumentParser()
    parser.add_argument('--eval', action='store_true')
    args = parser.parse_args()

    if args.eval:
        evaluate()
    else:
        train()
