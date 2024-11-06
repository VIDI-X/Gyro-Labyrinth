# Labyrinth Game for VIDI X Microcontroller

This code is for a labyrinth game on the VIDI X microcontroller, utilizing an LSM9DS1TR gyroscope connected to the VIDIIC connector to allow player movement through tilt-based controls. Optionally, the player can use buttons for debugging purposes. This setup provides a responsive and interactive way to navigate a labyrinth, complete with obstacles, collectable items, and enemies.

![Gyro-Labyrinth (2)](https://github.com/user-attachments/assets/c3541adb-0711-4502-ad1b-55c46b1097a8)

## Hardware Requirements
1. **VIDI X Microcontroller**
2. **VIDIIC I2C Connector Cable**:
   - Ensure micro switches 4 and 5 in the S2 switch group are set to "USE EXP" to enable proper I2C communication.
[3. **LSM9DS1TR Gyroscope** ](https://soldered.com/product/accelerometer-gyroscope-magnetometer-lsm9ds1tr-9-dof-breakout/)

![Gyro-Labyrinth (1)](https://github.com/user-attachments/assets/e3380834-d7ed-4333-9502-5c0b9ea9cb3e)

## Game Setup
The labyrinth layout is stored as a 2D array, with specific values representing different elements of the game. The player moves through the maze to collect items, avoid or fight vampires, and find the exit.

### Game Map Elements
- `wall`: Obstacles the player cannot pass through.
- `empty`: Open passage for the player.
- `Player`: Represents the player's position.
- `key`: Collectable keys required to open doors.
- `door`: Locked paths that require keys to pass.
- `garlic`: Collectable items used to fend off vampires.
- `vampire`: Enemy that follows the player; the player needs garlic to defeat them.
- `end`: The goal or exit of the labyrinth.

## Code Structure and Functionality

### Initialization
- **TFT Display**: Sets up an Adafruit ILI9341 display for rendering the labyrinth and player movement.
- **LED Strip**: A WS2812B LED strip is configured with 5 LEDs. The LEDs change colors to represent events such as the collection of keys or garlic, winning the game, or dying.
- **Sound**: A speaker outputs sounds to indicate game events, such as winning or losing.

### Movement
Player movement is controlled through either:
- **Gyroscope Tilting**: Tilting the gyroscope (connected via I2C) updates the player's position based on the tilt direction.
- **Buttons**: Optional left-right and up-down buttons allow manual movement, useful for debugging.

### Game Logic
The game logic includes:
- **Item Collection**: Collect keys and garlic by moving over them. Keys open doors, while garlic is used to defeat vampires.
- **Doors**: When the player reaches a door, they can pass if they have collected keys.
- **Vampire Encounters**: Vampires move towards the player every 1500 ms. The player needs garlic to survive encounters with vampires; otherwise, the player dies.
- **Game Win**: If the player reaches the exit, they win, and the game triggers a victory sound and message.
- **Game Over**: If the player encounters a vampire without garlic, the player dies, triggering a losing sound and message.

### Saving and Loading
The game state can be saved and loaded using SPIFFS:
- **Save**: Saves the labyrinth state to the file system, allowing the game to resume from the same point.
- **Load**: Loads a saved game state.

## Functions Overview

### `fill_world_edges()`
Defines the boundaries of the labyrinth to ensure the player remains within the play area.

### `generateRandomPattern()`
Creates random obstacles within the labyrinth, adding variety to each game.

### `drawWorld()`
Renders the labyrinth and the current game state to the TFT display.

### `moveVampires()`
Moves vampires towards the player based on their relative positions. Vampires avoid walls, doors, and garlic. If they encounter garlic, they are removed from the game.

### `go_right()`, `go_left()`, `go_up()`, `go_down()`
Functions for moving the player in each direction. Each function includes checks for collecting items, opening doors, and encountering vampires.

### `saveWorld()` and `loadWorld()`
Uses SPIFFS to save and load the labyrinth's state, allowing the game to be paused and resumed.

### `SoundSetup()`, `DiedSound()`, `WinnerSound()`
Controls sounds for game events:
- **DiedSound()**: Plays when the player encounters a vampire without garlic.
- **WinnerSound()**: Plays when the player reaches the exit.

### `RGBloop()`, `RGBGreen()`, `RGBRed()`, `RGBBlue()`, `RGBOff()`
Functions for LED effects to represent game states:
- **Green**: Indicates item collection.
- **Red**: Indicates danger or game over.
- **Blue**: General effect during the game.

## Setup and Loop Functions

### `setup()`
Initializes the gyroscope, TFT display, LED strip, and buttons. Loads any saved game state if available and prepares the labyrinth for play.

### `loop()`
Continuously checks for:
- **Movement inputs** from the gyroscope or buttons.
- **Vampire movement** based on a timed interval.
- **Game state updates** for drawing and saving/loading.

The loop also converts the gyroscope's pitch and roll values to movement directions and updates the game state accordingly.

---

This code creates a complete, immersive game experience on the VIDI X microcontroller, leveraging the gyroscope for dynamic control and providing a unique labyrinth adventure. The use of LED effects, sounds, and saving/loading options enhances gameplay, making it an engaging project on the VIDI X platform.

![Gyro-Labyrinth (3)](https://github.com/user-attachments/assets/274bcd5c-167f-4095-901b-5465c6c7fd8f)
