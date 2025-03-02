# Project 2 - Pong Clone

## Overview
This project is a **Pong Clone** implemented using **SDL2 and OpenGL**. The game features:
- Classic Pong-style gameplay with UFOs instead of paddles.
- A dynamically rotating background for added aesthetics.
- Support for **one-player and two-player modes**.
- Adjustable ball count (1, 2, or 3 balls).
- Collision detection and physics to simulate ball movement.
- Simple graphics with textures for UFOs, the ball, and background.

## Features
- **Single and Multiplayer Mode**: Play against another player or challenge yourself in a one-player mode.
- **Dynamic Ball Movement**: The ball moves and bounces realistically off walls and paddles.
- **Collision Detection**: Handles paddle-ball collisions with realistic rebound effects.
- **Rotating Background**: A rotating visual element adds aesthetic appeal.
- **Game Over Detection**: When the ball passes a player's boundary, the game ends with a winner displayed.

## Controls
| Key | Action |
| --- | ------ |
| `W` | Move Amber UFO Up |
| `S` | Move Amber UFO Down |
| `Arrow Up` | Move Blue UFO Up |
| `Arrow Down` | Move Blue UFO Down |
| `T` | Toggle between One-Player and Two-Player mode |
| `1` | Play with 1 Ball |
| `2` | Play with 2 Balls |
| `3` | Play with 3 Balls |
| `Q` | Quit the game |

## Installation & Setup
1. **Clone the Repository**:
   ```sh
   git clone https://github.com/mn3040/CS3113.git
   cd CS3113
2. **Install Dependencies:**:
   Ensure you have:
   - SDL2 installed
   - OpenGL development libraries
   - A C++ compiler supporting C++11 or later
3. **Compile the Project**:
   ```sh
   g++ main.cpp ShaderProgram.cpp -o pong -lSDL2 -lGL -lGLEW
4. **Run the Game**:
   ```sh
   ./pong
   
## File Structure
    📂 Pong Clone
    ├── 📂 assets                # Game textures and assets
    │   ├── amber_ufo.png
    │   ├── blue_ufo.png
    │   ├── game_over.gif
    │   ├── meteor.png
    │   ├── background.png
    │   ├── rotating_background.jpg
    │   ├── blue_wins.png
    │   ├── red_wins.png
    ├── 📂 shaders              # GLSL Shader Programs
    │   ├── vertex_textured.glsl
    │   ├── fragment_textured.glsl
    ├── main.cpp                # Core game logic
    ├── ShaderProgram.h/.cpp     # Shader handling
    ├── stb_image.h             # Image loading for textures
    └── README.md               # Project documentation


## Technologies Used
- C++
- SDL2
- OpenGL
- GLM (OpenGL Mathematics Library)

## Author
- Matthew Nunez
- Date Due: March 1, 2025
All work is completed in accordance with the NYU School of Engineering's Academic Integrity policies.

## Acknowledgments
- SDL2: For graphics rendering and input handling.
- OpenGL: For rendering the game objects.
- GLM: For handling 3D transformations.
