# Rise of the AI

Welcome to **Rise of the AI**, a 2D platformer created using C++, SDL2, and OpenGL for **CS-UY 3113** at NYU Tandon.

## 🎮 Game Description

"Rise of the AI" is a retro-inspired side-scrolling platformer that challenges players to survive three distinct levels full of animated enemies and more. Featuring immersive sound effects and pixel-perfect collisions, the game tests reflexes, timing, and strategic thinking.

## 📸 Screenshots
![Screenshot 2025-04-06 003238](https://github.com/user-attachments/assets/0a1d6267-1a89-44f6-b414-637a823ff539)
![Screenshot 2025-04-06 003258](https://github.com/user-attachments/assets/1c997ac1-3e07-4763-98c9-ccfc66443f52)


## 🛠️ Features

- 🧠 **Three AI Types**:  
  - **WALKER** – patrols back and forth  
  - **GUARD** – follows the player when in range  
  - **FLYER** – moves in a sinusoidal pattern

- 🎵 **Custom Audio System**:
  - Jump, coin collection, hurt, victory sounds
  - Background music ("Skyline Pulse") fades in on each level

- 🔥 **Visual Effects**:
  - Fade-in transitions on level load using an `Effects` engine

- 💡 **Polished Game Design**:
  - Multi-layered level layout with strategic enemy placements

- 🎮 **Controls**:
  - `← →` to move
  - `Space` to jump
  - `Esc` to quit

## 📁 File Structure
    ├── assets/ # Textures, audio, fonts 
    ├── src/ │ 
    ├── Entity.* # Player & Enemy logic │ 
    ├── Map.* # Tilemap logic │ 
    ├── LevelA.* / B.* / C.* # Per-level logic │ 
    ├── Effects.* # Fade in/out visual effect │ 
    └── Utility.* # Texture loading, font rendering 
    ├── main.cpp # Game loop & scene transitions 
    └── README.md

## 🧩 Dependencies

- **SDL2**
- **SDL2_image**
- **SDL2_mixer**
- **GLM**
- **OpenGL**
- **stb_image**

Ensure these are installed and properly linked during compilation.

## 🚀 Build Instructions (Mac/Linux)

```bash
g++ -std=c++11 -framework OpenGL -framework SDL2 -framework SDL2_image -framework SDL2_mixer \
src/*.cpp -o RiseOfTheAI
./RiseOfTheAI
```
For Windows users, ensure proper linking to .dll and .lib files for SDL and OpenGL.

💻 Developers
Matthew Nunez – Game design, programming, and visual assets

Created as part of the Spring 2025 course: CS-UY 3113 - Intro to Game Programming

🏁 Future Improvements
Add a global UI showing the coin counter

Track high scores and time

Boss fight in Level C?

Expand enemy behaviors (e.g., projectiles, patrolling platforms)


