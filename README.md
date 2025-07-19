## Build & Run

All commands are run from the **root path** of the project.

### To build the project:
```bash
cd build
cmake ..
make
```

### To run the game:
```bash
make run
```

Alternatively, you can run the compiled binary directly from the `bin/` folder if available.

---
![out](https://github.com/user-attachments/assets/70407322-1d7e-4cc5-875b-8d7fd5773368)

# Game Description

## Controls

- `W`, `A`, `S`, `D` — Move the player  
- **Left Mouse Button** — Shoot  
- **Right Mouse Button** — Super shoot  
- `P` — Pause / Unpause the game  
  > While paused, all inputs (except `P`) are disabled and enemy movement stops.

---

## Gameplay

- Enemies spawn randomly and bounce off the edges of the screen.
- When an enemy is killed, it splits into smaller enemies.
- **Scoring:**
  - Regular enemy: **+20 points**
  - Small enemy: **+40 points**
- If the player is hit by any enemy, they respawn at the center of the screen.

---
## Configuration Format

### Window
Window W H FL FS
- This line declares that the SFML Window must be constructed with width W and height H, each of which will be integers. FL is the frame limit that the window should be set to, and FS will be an integer which specifies whether to display the application in full-screen mode (1) or not (0).
- `W` — Window width (`int`)  
- `H` — Window height (`int`)  
- `FL` — Frame limit (`int`)  
- `FS` — Fullscreen (1 = yes, 0 = no)

---
### Font
Font F S R G B
- This lines defines the font which is to be used to draw text
for this program. The format of the line is as follows:
- `F` — Font file (`std::string`)  
- `S` — Font size (`int`)  
- `R G B` — Font color (`int, int, int`)

---
### Player Specification
Player SR CR S FR FG FB OR OG OB OT V
- `SR` — Shape radius (`int`)  
- `CR` — Collision radius (`int`)  
- `S` — Speed (`float`)  
- `FR FG FB` — Fill color (`int, int, int`)  
- `OR OG OB` — Outline color (`int, int, int`)  
- `OT` — Outline thickness (`int`)  
- `V` — Number of shape vertices (`int`)

---
### Enemy Specification:
- `SR` — Shape radius (`int`)  
- `CR` — Collision radius (`int`)  
- `SMIN SMAX` — Min and max speed (`float, float`)  
- `OR OG OB` — Outline color (`int, int, int`)  
- `OT` — Outline thickness (`int`)  
- `VMIN VMAX` — Min and max vertices (`int, int`)  
- `L` — Small enemy lifespan (`int`)  
- `SI` — Spawn interval (`int`)

---
### Bullet Specification:
- `SR` — Shape radius (`int`)  
- `CR` — Collision radius (`int`)  
- `S` — Speed (`float`)  
- `FR FG FB` — Fill color (`int, int, int`)  
- `OR OG OB` — Outline color (`int, int, int`)  
- `OT` — Outline thickness (`int`)  
- `V` — Shape vertices (`int`)  
- `L` — Lifespan (`int`)

---


##### THE BASIC COMMIT FOR ASSIGNMENT IS -commit 1aa40bb44- #####

## Assignment Hints
--------------------------
I recommend approaching this assignment in the following order:

0. Save the configuration file reading until later, after Entities implemented
1. Implement the Vec2 class, which you will use for all Components
2. Implement th basic functionality in the EntityManager class
   Implement the EntityManager's addEntity() and update() functions so
   you can start testing the Game class quickly. Don't worry about the update()
   function which deletes dead entities until later when you get more game mechanics working.
3. Implement basics of the Game class:
   a. Construct a player Entity using the spawnPlayer() function
   b. Implement basic drawing of entities using the Game::sRender() function
   c. Construct some enemies using the spawnEnemy() function
   d. Construct a bullet using the spawnBullet() function
4. Implement Player Movement in Game::sUserInput and Game::sMovement
5. Implement the EntityManager::update() function so it deletes dead entities
6. Implement the EntityManager::getEntities(tag) functionality
7. Implement collisions in sCollision and entity.destroy() if it's dead
8. Implement the rest of the game's functionality, including config file reading
