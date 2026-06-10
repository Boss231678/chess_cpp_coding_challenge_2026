# ♟️ Chess AI Challenge Developer Guide

Welcome to the Chess AI Tournament Framework! This repository provides a cross-platform graphical chess arena written in C++ using SFML 3. 

You do not need to look at or modify the graphical engine files (`main.cpp`, `ChessAIBase.h`). Your only goal is to create your own header file, inherit from the master template, and write an algorithm to calculate the best moves!

---

## 📁 Project Architecture & Files

*   `main.cpp` — **Do not touch.** Handles window rendering, timers, mouse clicks, move logs, and strict rule validation.
*   `ChessAIBase.h` — **Do not touch.** Declares the abstract engine blueprint and the automated factory string registry map.
*   `ChessTypes.h` — Contains simple struct definitions shared by the entire framework.
*   `Assets/` — Folder containing the font file and the visual chess pieces.
*   `[your_name]_ChessAI.h` — **Your Workspace!** This is where you write your logic.

---

## 🚀 How to Create Your Custom AI File

To build your bot, create a new header file named `[yourname]_ChessAI.h` (e.g., `alex_ChessAI.h`) and paste this exact template inside it:

```cpp
#ifndef ALEX_CHESS_AI_H
#define ALEX_CHESS_AI_H

#include "ChessAIBase.h"

// 1. Name your unique class layout
class AlexChessAI : public ChessAIBase {
public:
    // 2. Implement the pure virtual method
    ChessMove makeMove(const std::vector<std::vector<char>>& board, char aiColor) override {
        ChessMove decidedMove;

        // WRITE YOUR CALCULATIONS AND STRATEGY ALGORITHMS HERE
        // Example: myMove.fromSquare = "e2"; myMove.toSquare = "e4";

        return decidedMove;
    }
};

// 3. Register your bot's nickname string so the prompt can recognize it
REGISTER_CHESS_AI(AlexChessAI, "alex");

#endif
```

*Crucial Step:* Once your file is ready, open the very top of `main.cpp` and include your file string name right next to the others so it registers on launch:
```cpp
#include "alex_ChessAI.h"
```

---

## 🔍 How to Read the Chess Board State

The framework automatically hands your `makeMove` function an `8x8` grid matrix of standard primitive characters (`const std::vector<std::vector<char>>& board`).

### 1. Understanding Coordinate Mapping
The vector indices range from `0` to `7`. The array behaves like an inverted coordinate grid relative to traditional chess ranks:
*   `board[0][0]` represents square **a8** (Top Left)
*   `board[0][7]` represents square **h8** (Top Right)
*   `board[7][0]` represents square **a1** (Bottom Left)
*   `board[7][7]` represents square **h1** (Bottom Right)

### 2. Piece Tracking Notation Symbols
*   **Empty Spaces:** Represented by a period (`'.'`).
*   **White Pieces:** Represented by **UPPERCASE** characters (`'P'`, `'R'`, `'N'`, `'B'`, `'Q'`, `'K'`).
*   **Black Pieces:** Represented by **lowercase** characters (`'p'`, `'r'`, `'n'`, `'b'`, `'q'`, `'k'`).

### 3. Knowing Your Color Identity
The parameter `char aiColor` passes either `'w'` (White) or `'b'` (Black). Use this variable to ensure your bot doesn't accidentally move an opponent's piece!

---

## 💾 Function Return Specifications & Formatting

The `makeMove` function **MUST** return a structured `ChessMove` object. This object tells the main controller application exactly which piece you are selecting and where you want to drop it. 

### 1. Value Definitions
The `ChessMove` struct contains two string parameters:
*   `fromSquare` (std::string): The square of the piece your bot intends to pick up.
*   `toSquare` (std::string): The square where your piece will land.

### 2. Strict Layout Format
The strings must follow standard chess algebraic notation exactly:
*   **Length:** Exactly 2 characters long.
*   **Character 1:** A lowercase column file letter from `'a'` to `'h'`.
*   **Character 2:** A row rank digit character from `'1'` to `'8'`.

```cpp
// CORRECT FORMATTING EXAMPLES
ChessMove correctMove;
correctMove.fromSquare = "e2"; // Lowercase letter + single digit string
correctMove.toSquare   = "e4"; 

// INCORRECT FORMATTING EXAMPLES (Will crash or skip your turn)
ChessMove badMove;
badMove.fromSquare = "E2";     // ERROR: Uppercase letters are not recognized!
badMove.toSquare   = "e-4";    // ERROR: Special characters or extra spaces will fail validation.
```
## 🤖 Concrete Bot Implementation Example

To help you get started, here is a complete, functional example of a basic engine. This bot scans the board, identifies all pieces that belong to it, looks at the squares directly ahead, and makes a legal forward step or a simple knight jump.

```cpp
#include "ChessAIBase.h"
#include <vector>
#include <string>
#include <cctype>

class BeginnerChessAI : public ChessAIBase {
public:
    ChessMove makeMove(const std::vector<std::vector<char>>& board, char aiColor) override {
        ChessMove decidedMove;

        // Step 1: Establish our direction vector (White moves up, Black moves down)
        int direction = (aiColor == 'w') ? -1 : 1;

        // Step 2: Double loop to read every single square of the 8x8 grid
        for (int r = 0; r < 8; ++r) {
            for (int c = 0; c < 8; ++c) {
                char piece = board[r][c];

                // Step 3: Check if the piece on this square actually belongs to us
                bool isOurPiece = (aiColor == 'w' && isupper(piece)) || 
                                  (aiColor == 'b' && islower(piece));

                if (isOurPiece) {
                    // --- STRATEGY A: IF IT'S A PAWN, TRY TO STEP FORWARD ---
                    if (toupper(piece) == 'P') {
                        int targetRow = r + direction;
                        
                        // Ensure the square directly in front is within bounds and empty
                        if (targetRow >= 0 && targetRow < 8 && board[targetRow][c] == '.') {
                            
                            // Step 4: Convert array coordinates (r, c) to string notation ("e2")
                            decidedMove.fromSquare = std::string(1, 'a' + c) + std::to_string(8 - r);
                            decidedMove.toSquare   = std::string(1, 'a' + c) + std::to_string(8 - targetRow);
                            
                            return decidedMove; // Found a valid move! Send it to the engine.
                        }
                    }
                    
                    // --- STRATEGY B: IF IT'S A KNIGHT, TRY AN L-SHAPE JUMP ---
                    else if (toupper(piece) == 'N') {
                        int moveRow[] = {-2, -2, -1, -1, 1, 1, 2, 2};
                        int moveCol[] = {-1, 1, -2, 2, -2, 2, -1, 1};
                        
                        for (int i = 0; i < 8; ++i) {
                            int targetRow = r + moveRow[i];
                            int targetCol = c + moveCol[i];
                            
                            if (targetRow >= 0 && targetRow < 8 && targetCol >= 0 && targetCol < 8) {
                                char targetPiece = board[targetRow][targetCol];
                                
                                // Jump is safe if target is empty OR holds an enemy piece
                                bool emptyOrEnemy = (targetPiece == '.') || 
                                                    (aiColor == 'w' && islower(targetPiece)) || 
                                                    (aiColor == 'b' && isupper(targetPiece));
                                
                                if (emptyOrEnemy) {
                                    decidedMove.fromSquare = std::string(1, 'a' + c) + std::to_string(8 - r);
                                    decidedMove.toSquare   = std::string(1, 'a' + targetCol) + std::to_string(8 - targetRow);
                                    
                                    return decidedMove; // Send the knight move!
                                }
                            }
                        }
                    }
                }
            }
        }

        return decidedMove; // Fallback
    }
};

REGISTER_CHESS_AI(BeginnerChessAI, "beginner_bot");
```

---

### 📋 Step-by-Step Code Walkthrough

1. **Establish Perspective (`direction`):** Because the 2D matrix treats row `0` as the top (Black's side) and row `7` as the bottom (White's side), moving forward means different things for each color. White moves up the board by subtracting rows (`-1`), while Black moves down by adding rows (`+1`).
2. **Scan the Board Array (`for` loops):** The code sets up a standard nested `for` loop to look at every row (`r`) and column (`c`) from index 0 to 7 to inspect what character sits on that square.
3. **Verify Piece Ownership (`isOurPiece`):** White pieces are uppercase, and Black pieces are lowercase. The code uses `isupper()` and `islower()` to identify pieces that belong to our assigned team color, skipping empty squares (`'.'`) and opponent pieces.
4. **Coordinate Conversions:** To send the instructions back to `main.cpp`, array indexes must be mapped to strings:
    * **Column to File Letter:** Adding our column index integer (`0` to `7`) to the base character `'a'` shifts it into standard text characters (`'a'`, `'b'`, `'c'`, etc.).
    * **Row to Rank Number:** Because index `0` represents rank `8` on a real board, subtracting the row index from 8 (`8 - r`) mirrors it back into traditional numbers (`8` down to `1`).

---

## ⚠️ Crucial Cheat-Prevention Rule Engine

The graphical layer runs a **Strict Verification Filter** before any board state updates. It validates standard chess movement geometries:
*   **Pawn:** Single steps forward, double steps from the starting rank, and diagonal path captures.
*   **Knight:** Standard L-shape jump configurations.
*   **Sliders (Rooks, Bishops, Queens):** Linear paths with full piece-collision detection.
*   **Friendly Fire:** You cannot capture pieces of your own color.

**What happens if your bot generates an illegal move?**
The simulation will instantly reject the move, print a warning to the debugging terminal console, **skip your turn**, and pass control back to the opponent. Ensure your calculation loops include basic validation to avoid throwing faults!

---

## 🖥️ Compilation Commands

Open your terminal workspace folder and execute the appropriate command for your operating system:

### 🍏 On macOS (Requires Homebrew SFML):
```bash
g++ -std=c++17 main.cpp -o chess_popup -I/opt/homebrew/include -L/opt/homebrew/lib -lsfml-graphics -lsfml-window -lsfml-system
./chess_popup
```

### 🪟 On Windows (MinGW/GCC compiler):
```bash
g++ -std=c++17 main.cpp -o chess_popup.exe -I"C:\SFML\include" -L"C:\SFML\lib" -lsfml-graphics -lsfml-window -lsfml-system
.\chess_popup.exe
```

Good luck, and may the best engine win!
