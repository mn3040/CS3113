# Lunar Lander Game

## Overview
Lunar Lander is a physics-based 2D game where players must carefully navigate a lunar module to a safe landing on the moon's surface. The game challenges players to manage their fuel, control thrust, and maintain proper orientation to achieve a successful landing on designated platforms while avoiding obstacles.

## Game Mechanics

### Controls
- **Thrust**: Apply upward thrust to counteract gravity
- **Rotate Left/Right**: Control the orientation of the lander
- **Landing**: Must be achieved at a safe speed and angle to avoid crashing

### Physics Elements
- **Gravity**: Constant downward force (weaker than Earth gravity)
- **Thrust**: Counteracts gravity when activated, consumes fuel
- **Rotation**: Controlled by the player, affects thrust direction
- **Collision Detection**: Handles interactions with platforms and obstacles

### Gameplay Features
- **Fuel Management**: Limited fuel supply that depletes when thrusting
- **Safe Landing Requirements**:
  - Landing speed below 0.3 units per second
  - Lander must be oriented within 30 degrees of vertical
  - Must land on a designated platform
- **Obstacles**: Meteor that causes immediate crash on collision
- **Game States**: Win (successful landing) or lose (crash)

## Technical Implementation

### Entity System
The game uses an entity-based system with the following key components:
- **Position & Movement**: Tracks position, velocity, and acceleration vectors
- **Rotation**: Handles orientation with appropriate physics for rotation acceleration/deceleration
- **Animation States**: 
  - IDLE: Default state
  - THRUST: Active when engines are firing
  - CRASHED: Shown after a collision or crash

### Rendering
- Uses OpenGL for rendering via SDL
- Sprite-based animation system for different entity states
- Texture atlas support for efficient rendering

### Physics Simulation
- Realistic physics with gravity, thrust, and rotational dynamics
- Collision detection for platforms and obstacles
- Velocity and orientation checks for safe landing conditions

## Project Structure
- **Entity Class**: Core class handling physics, rendering, and game logic
- **Animation System**: Manages sprite animations for different states
- **Collision Detection**: Handles interactions with platforms and meteors
- **Input Handling**: Processes player controls for thrust and rotation

## Building and Running
1. Ensure you have SDL2 and OpenGL dependencies installed
2. Build the project using your preferred compiler
3. Run the executable to start the game

## Controls
- **Up Arrow**: Apply thrust
- **Left/Right Arrows**: Rotate the lander
- **R**: Restart the game (after landing or crashing)

## Game Objectives
1. Guide the lunar lander to one of the landing platforms
2. Maintain proper orientation (within 30° of vertical)
3. Reduce speed to below the safe landing threshold
4. Conserve fuel for a successful mission
5. Avoid colliding with the meteor and ground

## Tips for Success
- Use short, controlled bursts of thrust rather than continuous firing
- Begin slowing your descent early to avoid excessive fuel use
- Keep the lander oriented vertically when approaching landing platforms
- Monitor your fuel level, velocity, and orientation carefully

---

*This project was developed using SDL2 and OpenGL for educational purposes.*
