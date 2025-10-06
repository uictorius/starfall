# Starfall 2D 🚀

![GitHub Actions Workflow Status](https://img.shields.io/github/actions/workflow/status/uictorius/starfall/build.yml?branch=main&logo=github)
![Language](https://img.shields.io/badge/Language-C-blue.svg)
![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)
![Release Version](https://img.shields.io/github/v/release/uictorius/starfall?label=version)

A personal C game project developed for study purposes, showcasing a clean, decoupled architecture using the SDL2 library.

  <p align="center">
    <img src="https://raw.githubusercontent.com/uictorius/starfall/dd1dd1e1d66800fcfa4d7c97f5fd92c5d35a94be/screenshots/main-menu.png" alt="Main Menu" width="32%">
    <img src="https://raw.githubusercontent.com/uictorius/starfall/dd1dd1e1d66800fcfa4d7c97f5fd92c5d35a94be/screenshots/starfall.png" alt="Gameplay" width="32%">
    <img src="https://raw.githubusercontent.com/uictorius/starfall/dd1dd1e1d66800fcfa4d7c97f5fd92c5d35a94be/screenshots/game-over.png" alt="Game Over Screen" width="32%">
  </p>

---

### Project Context

**Starfall 2D** is an amateur learning project. The primary goal was to study the fundamentals of game development in C.

The project emphasizes a clear separation between the generic **game engine** (`core`) and the specific **game logic** (`game`), creating a modular foundation that is easy to extend and maintain.

### Code Architecture

The codebase is organized into a clean, three-tiered structure:

- `📁 core`: Contains the engine's low-level subsystems, which are decoupled from any specific game rules.

  - `renderer.c`: Manages window creation, texture/font loading, and all drawing operations. Implements a dynamic stretch-to-fill rendering pipeline.
  - `audio.c`: Manages loading and playback of music and sound effects.
  - `input.c`: Polls and processes all user input each frame.
  - `game.c`: Orchestrates the main game loop and state management (Menu, Playing, Game Over).

- `📁 game`: Contains the gameplay logic and rules unique to Starfall 2D.

  - `world.c`: Manages the state of all game entities (player, enemies, projectiles) and their behaviors.
  - `world_collisions.c`: A dedicated module for handling all collision detection and resolution.

- `📁 utils`: Contains shared data structures and constants used across the entire project.
  - `types.h`: Defines the core `structs` and `enums`.
  - `constants.h`: Centralizes all "magic numbers" for easy tweaking and balancing.

#### Doxygen Standardization

The entire codebase is documented using Doxygen-style comments. This practice allows for the automatic generation of comprehensive technical documentation, making it easier for developers to understand the project's structure and API.

### Controls

| Action                | Key(s)                            |
| --------------------- | --------------------------------- |
| **Move**              | `W`, `A`, `S`, `D`                |
| **Aim**               | `Mouse Cursor`                    |
| **Fire**              | `Right Mouse Button` / `Spacebar` |
| **Toggle Fullscreen** | `F11`                             |
| **Menu Navigation**   | `Arrow Keys` + `Enter` or `Mouse` |

### Key Features

The current version of the project is fully operational with the following features:

- **Complete Game Loop:** Seamless transitions between the main menu, active gameplay, and the game over screen.
- **Player Controls:**
  - Fluid movement with `W`, `A`, `S`, `D` keys.
  - Precise aiming based on the mouse cursor's position.
  - Shooting mechanic mapped to the **Right Mouse Button** and **Spacebar**.
- **Dynamic Enemies:**
  - Enemies spawn automatically over time.
  - AI enables enemies to move towards the player and fire back.
  - Movement logic is periodically updated to re-target the player.
- **Robust Collision System:** Functional collision detection between player, enemies, and projectiles.
- **Interactive UI & Menus:**
  - Fully interactive main and game over menus with both keyboard and mouse support.
  - A real-time HUD (Heads-Up Display) showing current Score and Lives.
- **Advanced Rendering:**
  - A dynamic rendering engine that stretches the game to fill 100% of the window at any resolution.
  - Fullscreen functionality can be toggled with `F11`.
- **Audio System:**
  - Background music for game ambiance.
  - Dynamic sound effects for player/enemy shots and explosions.

### Getting Started

#### 1. Dependencies

Ensure you have SDL2 and its extension libraries installed. On Debian/Ubuntu-based systems:

```sh
sudo apt-get install build-essential libsdl2-dev libsdl2-image-dev libsdl2-ttf-dev libsdl2-mixer-dev
```

Optional development utilities (to format code and generate documentation):

```sh
sudo apt-get install clang-format doxygen doxygen graphviz
```

#### 2\. Compilation & Running

Clone the repository, navigate to the project directory, and use the provided `Makefile`.

```sh
# Clone the repository
git clone https://github.com/uictorius/starfall.git
cd starfall

# Compile and run the game with a single command
make run

# Alternatively, compile without running
make

# Clean the build files
make clean

# Generate documentation (requires Doxygen)
make docs
```

### Contributing & Code Style

As this is a learning project, contributions, suggestions, and feedback are highly welcome\! If you wish to contribute, please adhere to the following guidelines to maintain code consistency:

- **Code Formatting:** Follow the existing code style (brace placement, variable naming, etc.).
- **Modularity:** Respect the separation between the `core` (engine) and `game` (logic) modules.
- **Documentation:** Add Doxygen-style comments to any new functions, structs, or complex code blocks.

### License

This project is licensed under the [**MIT License**](https://opensource.org/licenses/MIT).
