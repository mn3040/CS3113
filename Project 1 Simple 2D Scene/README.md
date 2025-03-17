# Simple 2D Scene - Project README

## Overview

This project is a simple 2D scene rendered using OpenGL, SDL, and GLSL shaders. It demonstrates basic 2D transformations, including translation, rotation, and scaling, along with texture mapping. The scene features two animated characters: "Invincible" and "Mordecai", which move and rotate independently. The scene is created using **C++**, **OpenGL**, and **SDL**. 

## Features
- **Texture Mapping**: Loads and displays two textures (Invincible and Mordecai sprites).
- **Transformations**: Applies rotation, scaling, and translation to both sprites.
- **Animation**: The sprites move and rotate continuously, with some of their properties being updated over time.
- **Shaders**: Uses vertex and fragment shaders for rendering textures with transformations.
- **OpenGL and SDL**: Utilizes OpenGL for rendering and SDL for window management and input handling.

## Requirements

- **C++** (C++11 or later recommended)
- **SDL2** (for window creation and input handling)
- **OpenGL** (for rendering)
- **GLM** (for matrix transformations)
- **stb_image** (for loading textures)

### Dependencies

- **SDL2**
- **GLM**
- **stb_image**
- **GLEW** (on Windows)

## Key Components

### 1. **ShaderProgram Class**:
   - This class loads and uses the vertex and fragment shaders for rendering the textured objects.
   - The shaders used here handle transforming vertex positions and texture coordinates.

### 2. **Texture Loading**:
   - Textures are loaded using `stbi_load` from **stb_image** and applied to the objects (Invincible and Mordecai sprites).

### 3. **Transformation Logic**:
   - The translation, rotation, and scaling of the objects are calculated each frame based on the elapsed time.
   - The matrices for these transformations are set via `glm::mat4` and applied in the `update()` function.

### 4. **Rendering**:
   - The rendering process involves setting up vertices and texture coordinates, binding the appropriate textures, and drawing the objects with transformations applied.

## Code Structure

### `main.cpp`:
   - Contains the main loop, initialization, input handling, game logic, rendering logic, and shutdown procedures.
   - Functions in this file include:
     - `initialise()`: Initializes the window, OpenGL, shaders, and textures.
     - `process_input()`: Handles user input and window events.
     - `update()`: Updates the game logic and transformations.
     - `render()`: Draws the objects with applied transformations.
     - `shutdown()`: Cleans up SDL resources.

### Shader Files:
- **vertex_textured.glsl**: Vertex shader that applies transformations to object vertices.
- **fragment_textured.glsl**: Fragment shader that maps the loaded texture to the object's surface.

### Assets:
- **invincible.png**: Texture for the Invincible sprite.
- **mordecai.png**: Texture for the Mordecai sprite.

## How the Program Works

1. **Initialization**:
   - The program initializes the SDL window, OpenGL context, and shaders.
   - Textures are loaded and applied to the two sprites (Invincible and Mordecai).
   
2. **Input Handling**:
   - The program listens for window events and user input. If the user closes the window, the application terminates.

3. **Update Loop**:
   - The position, rotation, and scale of the sprites are updated each frame based on elapsed time (`delta_time`).
   - The sprites rotate and translate across the screen.

4. **Rendering**:
   - The shaders are used to render the sprites with the transformations applied.
   - The screen is cleared each frame, and the updated scene is drawn to the window.

5. **Termination**:
   - When the user closes the window, SDL cleans up the resources and the program exits.

## Future Improvements
- Add more sprites or objects to the scene for a richer 2D environment.
- Implement keyboard or mouse controls for interactive movement of objects.
- Introduce lighting, shaders, and more advanced graphics techniques.
