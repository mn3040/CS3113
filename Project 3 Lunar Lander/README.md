# Lunar Lander Game

## Overview
Lunar Lander is a physics-based 2D game where players must carefully navigate a lunar module to a safe landing on the moon's surface. The game challenges players to manage their fuel, control thrust orientation, and maintain proper landing angle to achieve a successful touchdown on designated platforms while avoiding obstacles including a dangerous meteor.

![image](https://github.com/user-attachments/assets/d8cdd7b0-2acc-4a61-ab94-09e491f7c30c)


## Game Mechanics

### Controls
- **Thrust (Up Arrow)**: Apply thrust in the direction the lander is pointing to counteract gravity
- **Rotate Left (Left Arrow)**: Rotate the lander counter-clockwise
- **Rotate Right (Right Arrow)**: Rotate the lander clockwise
- **Q Key**: Quit the game

### Physics Elements
- **Gravity**: Constant downward force (set to -0.5f, weaker than Earth gravity)
- **Thrust**: Directional force based on lander orientation, consumes fuel at rate of 0.01 units per application
- **Rotation**: Gradual acceleration and deceleration with maximum rotation velocity of 40 degrees per second
- **Drag**: Small amount of drag (0.998 multiplier) applied to velocity each frame
- **Collision Detection**: Handles interactions with platforms, moving platform, ground, and meteor

### Gameplay Features
- **Fuel Management**: 100 units of fuel that depletes when thrusting
- **Safe Landing Requirements**:
  - Landing speed below 0.3 units per second
  - Lander must be oriented within 30 degrees of vertical (or within 30 degrees of 360)
  - Must land on a designated platform (fixed or moving)
- **Obstacles**: 
  - Meteor that causes immediate crash on collision (setting meteor_collision flag)
  - Irregular lunar terrain that will cause a crash if not landing on platforms
- **Moving Platform**: Dynamic landing target adding challenge to the gameplay
- **Game States**: 
  - Win (successful landing on platform)
  - Lose (crash due to excessive speed, wrong angle, meteor collision, or hitting the ground)

## Technical Implementation

### Entity System
The game uses an entity-based system with the following key components:
- **Position & Movement**: Tracks 3D vectors for position, velocity, and acceleration
- **Rotation Physics**: 
  - Rotation acceleration (20.0f degrees/sec²)
  - Rotation deceleration (10.0f degrees/sec²)
  - Maximum rotation velocity (40.0f degrees/sec)
- **Animation States**: 
  - IDLE: Default state (single frame)
  - THRUST: Active when engines are firing (3 frames animation)
  - CRASHED: Shown after a collision or crash (single frame)

### Rendering
- Uses OpenGL for rendering via SDL
- Texture atlas support for animation frames
- Different textures for each animation state:
  - index 0: Idle texture
  - index 1: Thrust texture
  - index 2: Crashed texture

### Physics Simulation
- Frame-rate independent physics using delta time
- Acceleration-based movement with proper vector calculations
- Thrust direction calculated based on rotation angle (in radians)
- Angle wrapping to keep rotation between 0-360 degrees

### Collision System
- Platform collision detection with width and height parameters
- Ground collision detection (at y-position -2.95f)
- Meteor collision detection using distance threshold (0.2 units)
- Landing criteria validation (speed and angle checks)

## Project Structure
- **Entity Class**: 
  - Core class handling physics, rendering, and game logic
  - Manages state (landed, crashed)
  - Controls animation state transitions
- **Animation System**: 
  - Time-based animation with configurable frames per second
  - Support for different animation dimensions (rows, columns)
  - Frame index tracking and cycling
- **Collision Detection**: 
  - Platform-specific collision handling
  - Moving platform support
  - Meteor collision with specialized handling
- **Input Handling**: 
  - Thrust activation/deactivation
  - Rotation with proper acceleration/deceleration

## Building and Running
1. Ensure you have SDL2, OpenGL, and GLEW dependencies installed
2. Build the project using your preferred compiler
3. Run the executable to start the game

## Game Objectives
1. Guide the lunar lander to one of the landing platforms (fixed or moving)
2. Maintain proper orientation (within 30° of vertical)
3. Reduce speed to below the safe landing threshold (0.3 units/sec)
4. Conserve fuel for a successful mission (starting with 100 units)
5. Avoid colliding with:
   - The meteor (instant crash)
   - The ground (instant crash)
   - Platforms at unsafe speeds or angles

## Tips for Success
- Use short, controlled bursts of thrust rather than continuous firing
- Begin slowing your descent early to avoid excessive fuel use
- Keep the lander oriented vertically when approaching landing platforms
- Account for the movement of the moving platform when planning your approach
- Monitor your fuel level, velocity, and orientation carefully
- Rotate slowly and deliberately, as rotation has acceleration and takes time to change

## Technical Details
- **CONSTANTS**:
  - GRAVITY = -0.5f
  - THRUST_POWER = 1.0f
  - ROTATION_SPEED = 1.2f
  - FUEL_BURN_RATE = 0.01f
  - SAFE_LANDING_SPEED = 0.3f
  - SECONDS_PER_FRAME = 6 (for animation timing)

- **Model Matrix Transformations**:
  - Translation (position)
  - Rotation (around Z-axis)
  - Scale (0.5f default scale)

---

*This project was developed using SDL2 and OpenGL for educational purposes.*
