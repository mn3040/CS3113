# Night Blade 🗡️🌙

A 2D platformer game built with C++ using OpenGL, SDL2, and GLM, featuring a stealth-action mechanics and multiple levels with increasing difficulty.

![image](https://github.com/user-attachments/assets/b38116e6-416a-4ec1-aebb-90a74ae001cf)


## Overview

Night Blade follows a protagonist fighting through three challenging levels to rescue hostages across different environments. The game features fluid combat mechanics, visual effects, and progressive difficulty scaling.

![image](https://github.com/user-attachments/assets/523d18b0-2c1b-4f03-9445-dc014c7bed75)


## Features

### Gameplay Mechanics
- **Fluid Combat System**: Multiple attack types including basic attacks, kicks, and combos
- **Character Abilities**:
  - Running (Shift)
  - Jumping (Space)
  - Crouch walking (Down arrow + movement)
  - Rolling (X)
  - Attack combos (Z key)
  - Powerful kicks (C key)
- **Enemy AI**: Three types of enemies with different behaviors:
  - Guards: Track and attack player when close
  - Walkers: Patrol designated areas
  - Flyers: Floating bat enemies with unique movement patterns

### Visual Effects
- **Dynamic Camera**: Follows player movement with shake effects during combat
- **Screen Effects**:
  - Fade in/out transitions
  - Flash effects during attacks
  - Shake effects for impact
- **Animation System**: Detailed character animations for all actions

### Level Design
- **Level 1**: Tutorial-like environment with basic enemies
- **Level 2**: Increased complexity with more enemies and challenges
- **Level 3**: Nighttime finale with maximum difficulty and the final boss

## Controls

| Action | Key |
|--------|-----|
| Move | Arrow Keys |
| Jump | Space |
| Run | Left Shift |
| Attack | Z |
| Roll | X |
| Kick | C |
| Crouch | Down Arrow |
| Pause/Menu | Enter |
| Exit | Q |

## Technical Details

### Architecture
```
Project 5 Night Blade/
├── src/                  # Source files
│   ├── main.cpp         # Entry point and game loop
│   ├── Entity.cpp       # Player and enemy logic
│   ├── Effects.cpp      # Visual effects system
│   ├── Map.cpp          # Level management
│   ├── LevelA.cpp       # First level
│   ├── LevelB.cpp       # Second level
│   ├── LevelC.cpp       # Final level
│   ├── Menu.cpp         # Main menu
│   ├── Instructions.cpp # Tutorial screen
│   └── Winner/Loser.cpp # End screens
├── assets/              # Game assets
│   ├── sprites/         # Character animations
│   ├── sounds/          # Audio files
│   └── fonts/           # Text rendering
└── shaders/             # OpenGL shaders
```

### Dependencies
- **SDL2**: Window management and input handling
- **SDL2_mixer**: Audio processing
- **SDL2_image**: Image loading
- **OpenGL**: Graphics rendering
- **GLM**: Mathematics library
- **STB Image**: Texture loading

### Build Requirements
- Visual Studio 2019 or later
- SDL2 development libraries
- OpenGL development libraries

## Building the Project

1. **Prerequisites**: Install Visual Studio with C++ support
2. **SDL2 Setup**: Place SDL2 libraries in `C:\SDL\`
3. **Build**: Open the Visual Studio solution and build in Debug or Release mode

## Game Flow

1. **Main Menu**: Choose to start or view instructions
2. **Level 1**: Learn basic mechanics while rescuing the first hostage
3. **Level 2**: Face increased enemy count and complexity
4. **Level 3**: Navigate through the darkest level to save the final hostage
5. **Victory**: Rescue all hostages and complete the mission

## Credits

Developed as Project 5 in the Game Programming course.

### Assets Used
- Character sprites and animations
- Industrial tileset for environments
- Sound effects and background music

### Development Team
- Programming and game design
- Level design and balancing
- Visual effects implementation

## License

Educational project - Not for commercial use

## Controls Reference Card

```
┌─────────────────────────┐
│    NIGHT BLADE CONTROLS │
├─────────────────────────┤
│ ← → ↑ ↓ : Move          │
│ Space  : Jump           │
│ Shift  : Run            │
│ Z      : Attack         │
│ X      : Roll           │
│ C      : Kick           │
│ Enter  : Menu/Pause     │
│ Q      : Quit           │
│ I      : Instructions   │
└─────────────────────────┘
```

## Tips for Success
- Master the combo system by chaining attacks with kicks
- Use rolling to evade enemy attacks
- Different enemies require different strategies
- Health management is crucial in later levels
- Explore levels thoroughly to find the best routes

Good luck, Night Blade!
