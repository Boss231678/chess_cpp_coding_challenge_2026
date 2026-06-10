#include <SFML/Graphics.hpp>
#include <iostream>
#include <optional>
#include <vector>
#include <string>
#include <map>
#include <memory>
#include <cmath>

//Remember to add the appropriate header files for the participants AI engines here!
#include "ChessTypes.h"
#include "ChessAIBase.h"
#include "Player1_ChessAI.h"
#include "Player2_ChessAI.h"
#include "Ved_ChessAI.h"

enum class GameMode { HumanVsAI, AIVsAI };

// Converts standard algebraic strings ("e2") to 2D array row/column coordinates
std::pair<int, int> squareToIndices(const std::string& square) {
    int col = square[0] - 'a';
    int row = 8 - (square[1] - '0');
    return {row, col};
}

// MOVE LOG HISTORY HELPER UTILITY
void logMove(std::vector<std::string>& logList, int& totalMoves, const std::string& playerLabel, const std::string& from, const std::string& to) {
    totalMoves++;
    std::string entry = std::to_string((totalMoves + 1) / 2) + ". " + playerLabel + ": " + from + " -> " + to;
    logList.push_back(entry);
    
    if (logList.size() > 15) {
        logList.erase(logList.begin());
    }
}

// CHESS RULE VALIDATION ENGINE
bool isMoveLegal(int fromR, int fromC, int toR, int toC, const std::vector<std::vector<char>>& board) {
    if (toR < 0 || toR >= 8 || toC < 0 || toC >= 8) return false;
    if (fromR == toR && fromC == toC) return false; 

    char piece = board[fromR][fromC];
    char target = board[toR][toC];
    if (piece == '.') return false; 

    if (target != '.') {
        if (isupper(piece) && isupper(target)) return false;
        if (islower(piece) && islower(target)) return false;
    }

    int deltaR = toR - fromR;
    int deltaC = toC - fromC;
    int absDeltaR = std::abs(deltaR);
    int absDeltaC = std::abs(deltaC);

    switch (std::toupper(piece)) {
        case 'P': { 
            int direction = isupper(piece) ? -1 : 1;
            int startRow = isupper(piece) ? 6 : 1;
            if (deltaC == 0 && deltaR == direction && target == '.') return true;
            if (deltaC == 0 && fromR == startRow && deltaR == 2 * direction) {
                if (board[fromR + direction][fromC] == '.' && target == '.') return true;
            }
            if (absDeltaC == 1 && deltaR == direction && target != '.') return true; 
            return false;
        }
        case 'N': 
            return (absDeltaR == 2 && absDeltaC == 1) || (absDeltaR == 1 && absDeltaC == 2);
        case 'B': 
            if (absDeltaR != absDeltaC) return false;
            for (int i = 1; i < absDeltaR; ++i) {
                if (board[fromR + i * (deltaR > 0 ? 1 : -1)][fromC + i * (deltaC > 0 ? 1 : -1)] != '.') return false;
            }
            return true;
        case 'R': 
            if (deltaR != 0 && deltaC != 0) return false;
            if (deltaR == 0) {
                for (int i = 1; i < absDeltaC; ++i) {
                    if (board[fromR][fromC + i * (deltaC > 0 ? 1 : -1)] != '.') return false;
                }
            } else {
                for (int i = 1; i < absDeltaR; ++i) {
                    if (board[fromR + i * (deltaR > 0 ? 1 : -1)][fromC] != '.') return false;
                }
            }
            return true;
        case 'Q': 
            if (absDeltaR != absDeltaC && deltaR != 0 && deltaC != 0) return false;
            if (absDeltaR == absDeltaC) {
                for (int i = 1; i < absDeltaR; ++i) {
                    if (board[fromR + i * (deltaR > 0 ? 1 : -1)][fromC + i * (deltaC > 0 ? 1 : -1)] != '.') return false;
                }
            } else {
                if (deltaR == 0) {
                    for (int i = 1; i < absDeltaC; ++i) {
                        if (board[fromR][fromC + i * (deltaC > 0 ? 1 : -1)] != '.') return false;
                    }
                } else {
                    for (int i = 1; i < absDeltaR; ++i) {
                        if (board[fromR + i * (deltaR > 0 ? 1 : -1)][fromC] != '.') return false;
                    }
                }
            }
            return true;
        case 'K': 
            return absDeltaR <= 1 && absDeltaC <= 1;
    }
    return false;
}
int main() {
    // 1. Terminal Interactive Menu Configuration
    std::cout << "=== Welcome to the Ultimate Chess Arena ===\n\n";
    std::cout << "Registered Friend Engines Available:\n";
    AIRegistry::getInstance().printRegisteredNames();
    std::cout << "\nSelect Game Mode:\n1) Human (White) vs Friend AI (Black)\n2) Friend AI vs Friend AI Match\nChoice: ";
    
    int choice = 1;
    std::cin >> choice;

    GameMode activeMode = (choice == 2) ? GameMode::AIVsAI : GameMode::HumanVsAI;
    std::unique_ptr<ChessAIBase> botWhite = nullptr;
    std::unique_ptr<ChessAIBase> botBlack = nullptr;
    std::string whiteLabel = "Human";
    std::string blackLabel = "AI";

    // Dynamic engine mapping via string input matching
    if (activeMode == GameMode::HumanVsAI) {
        std::cout << "Enter the name of the friend bot to play against: ";
        std::cin >> blackLabel;
        botBlack = AIRegistry::getInstance().createAI(blackLabel);
        if (!botBlack) {
            std::cerr << "Error: Bot name '" << blackLabel << "' not found! Exiting...\n";
            return -1;
        }
    } else {
        std::cout << "Enter the name of the White bot (Player 1): ";
        std::cin >> whiteLabel;
        std::cout << "Enter the name of the Black bot (Player 2): ";
        std::cin >> blackLabel;

        botWhite = AIRegistry::getInstance().createAI(whiteLabel);
        botBlack = AIRegistry::getInstance().createAI(blackLabel);

        if (!botWhite || !botBlack) {
            std::cerr << "Error: One or both bot names not found! Exiting...\n";
            return -1;
        }
    }

    // 2. Load Visual Environment Assets
    sf::Texture boardTexture;
    if (!boardTexture.loadFromFile("Assets/chess_board.png")) {
        std::cerr << "Error: Missing Assets/chess_board.png\n";
        return -1;
    }

    sf::Font font;
    if (!font.openFromFile("Assets/Roboto-Regular.ttf")) {
        std::cerr << "Error: Missing Assets/Roboto-Regular.ttf\n";
        return -1;
    }

    sf::Vector2u imageSize = boardTexture.getSize();
    sf::RenderWindow window(sf::VideoMode({imageSize.x + 300, imageSize.y + 50}), "Chess AI Battleground & Move Log", sf::Style::Close);
    sf::Sprite boardSprite(boardTexture);

    // 3. Side Panels & Interface Layout Parameters
    sf::RectangleShape sidebarBg(sf::Vector2f(300.f, static_cast<float>(imageSize.y + 50)));
    sidebarBg.setFillColor(sf::Color(35, 35, 40));
    sidebarBg.setPosition({static_cast<float>(imageSize.x), 0.f});

    sf::Text sidebarHeader(font, "MOVE HISTORY LOG", 18);
    sidebarHeader.setFillColor(sf::Color(200, 200, 200));
    sidebarHeader.setStyle(sf::Text::Bold);
    sidebarHeader.setPosition({static_cast<float>(imageSize.x) + 20.f, 20.f});

    sf::Text statusText(font, "White's Turn", 22);
    statusText.setFillColor(sf::Color::White);
    sf::RectangleShape textBg(sf::Vector2f(static_cast<float>(imageSize.x), 50.f));
    textBg.setFillColor(sf::Color(25, 25, 25));
    textBg.setPosition({0.f, static_cast<float>(imageSize.y)});
    statusText.setPosition({20.f, static_cast<float>(imageSize.y) + 12.f});

    sf::Text timerText(font, "Match Time: 00:00", 22);
    timerText.setFillColor(sf::Color(200, 200, 100)); 
    timerText.setPosition({static_cast<float>(imageSize.x) - 230.f, static_cast<float>(imageSize.y) + 12.f});

    std::map<char, std::string> pieceFiles = {
        {'P', "Assets/white_pawn.png"},   {'R', "Assets/white_rook.png"},   {'N', "Assets/white_knight.png"},
        {'B', "Assets/white_bishop.png"}, {'Q', "Assets/white_queen.png"},  {'K', "Assets/white_king.png"},
        {'p', "Assets/black_pawn.png"},   {'r', "Assets/black_rook.png"},   {'n', "Assets/black_knight.png"},
        {'b', "Assets/black_bishop.png"}, {'q', "Assets/black_queen.png"},  {'k', "Assets/black_king.png"}
    };

    float borderPaddingPct = 0.045f;
    float boardStartX = imageSize.x * borderPaddingPct;
    float boardEndX   = imageSize.x * (1.0f - borderPaddingPct);
    float boardStartY = imageSize.y * borderPaddingPct;
    float boardEndY   = imageSize.y * (1.0f - borderPaddingPct);

    float squareWidth = (boardEndX - boardStartX) / 8.0f;
    float squareHeight = (boardEndY - boardStartY) / 8.0f;

    std::map<char, sf::Texture> pieceTextures;
    std::map<char, std::unique_ptr<sf::Sprite>> pieceSprites;

    for (const auto& [pieceChar, fileName] : pieceFiles) {
        if (!pieceTextures[pieceChar].loadFromFile(fileName)) {
            std::cerr << "Error: Missing file -> " << fileName << "\n";
            return -1;
        }
        pieceSprites[pieceChar] = std::make_unique<sf::Sprite>(pieceTextures[pieceChar]);
        sf::Vector2u assetSize = pieceTextures[pieceChar].getSize();
        pieceSprites[pieceChar]->setScale({squareWidth / assetSize.x, squareHeight / assetSize.y});
    }

    sf::RectangleShape highlightRect(sf::Vector2f(squareWidth, squareHeight));
    highlightRect.setFillColor(sf::Color(0, 150, 255, 128));

    std::vector<std::vector<char>> chessBoard = {
        {'r', 'n', 'b', 'q', 'k', 'b', 'n', 'r'},
        {'p', 'p', 'p', 'p', 'p', 'p', 'p', 'p'},
        {'.', '.', '.', '.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.', '.', '.', '.'},
        {'.', '.', '.', '.', '.', '.', '.', '.'},
        {'P', 'P', 'P', 'P', 'P', 'P', 'P', 'P'},
        {'R', 'N', 'B', 'Q', 'K', 'B', 'N', 'R'}
    };

    char currentTurn = 'w'; 
    std::string selectedSquare = ""; 
    
    sf::Clock matchClock;  
    sf::Clock aiTimer;     
    std::vector<std::string> moveHistory;
    int moveCounter = 0;
    // 4. Main Active Window Rendering and Event Loop
    while (window.isOpen()) {
        while (const std::optional<sf::Event> event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) window.close();

            // Handle human player interaction frames (Only triggers on active player turn)
            if (activeMode == GameMode::HumanVsAI && currentTurn == 'w' && event->is<sf::Event::MouseButtonPressed>()) {
                const auto* mouse = event->getIf<sf::Event::MouseButtonPressed>();
                if (mouse->button == sf::Mouse::Button::Left) {
                    float clickX = static_cast<float>(mouse->position.x);
                    float clickY = static_cast<float>(mouse->position.y);

                    if (clickX >= boardStartX && clickX <= boardEndX && clickY >= boardStartY && clickY <= boardEndY) {
                        int fileIndex = static_cast<int>((clickX - boardStartX) / squareWidth);
                        int rankIndex = static_cast<int>((clickY - boardStartY) / squareHeight);
                        std::string clickedSquare = std::string(1, 'a' + fileIndex) + std::to_string(8 - rankIndex);

                        if (selectedSquare.empty()) {
                            // Target piece selection matrix parsing
                            if (chessBoard[rankIndex][fileIndex] != '.' && isupper(chessBoard[rankIndex][fileIndex])) {
                                selectedSquare = clickedSquare;
                            }
                        } else {
                            auto [fromRow, fromCol] = squareToIndices(selectedSquare);
                            auto [toRow, toCol] = squareToIndices(clickedSquare);

                            if (selectedSquare == clickedSquare) {
                                selectedSquare = ""; // Cancel selection state
                            } else if (isMoveLegal(fromRow, fromCol, toRow, toCol, chessBoard)) {
                                chessBoard[toRow][toCol] = chessBoard[fromRow][fromCol];
                                chessBoard[fromRow][fromCol] = '.';
                                
                                logMove(moveHistory, moveCounter, "Human", selectedSquare, clickedSquare);
                                selectedSquare = ""; 
                                currentTurn = 'b';  
                                aiTimer.restart();
                            } else {
                                selectedSquare = ""; 
                            }
                        }
                    }
                }
            }
        }
        // 5. Update Match Elapsed Duration Stopwatch
        int elapsedSeconds = static_cast<int>(matchClock.getElapsedTime().asSeconds());
        int minutes = elapsedSeconds / 60;
        int seconds = elapsedSeconds % 60;
        std::string minStr = (minutes < 10 ? "0" : "") + std::to_string(minutes);
        std::string secStr = (seconds < 10 ? "0" : "") + std::to_string(seconds);
        timerText.setString("Match Time: " + minStr + ":" + secStr);

        // Update turn context label strings dynamically
        if (currentTurn == 'w') {
            statusText.setString(activeMode == GameMode::AIVsAI ? "Turn: " + whiteLabel : "Turn: Human");
        } else {
            statusText.setString("Turn: " + blackLabel);
        }

        // 6. Dynamic Bot Evaluation (Polymorphic Reference Calls)
        if (currentTurn == 'w' && activeMode == GameMode::AIVsAI && aiTimer.getElapsedTime().asSeconds() > 0.8f) {
            auto move = botWhite->makeMove(chessBoard, 'w');
            if (!move.fromSquare.empty() && !move.toSquare.empty()) {
                auto [fR, fC] = squareToIndices(move.fromSquare);
                auto [tR, tC] = squareToIndices(move.toSquare);
                if (isMoveLegal(fR, fC, tR, tC, chessBoard)) {
                    chessBoard[tR][tC] = chessBoard[fR][fC];
                    chessBoard[fR][fC] = '.';
                    logMove(moveHistory, moveCounter, whiteLabel, move.fromSquare, move.toSquare);
                }
            }
            currentTurn = 'b';
            aiTimer.restart();
        } 
        else if (currentTurn == 'b' && aiTimer.getElapsedTime().asSeconds() > 0.8f) {
            auto move = botBlack->makeMove(chessBoard, 'b');
            if (!move.fromSquare.empty() && !move.toSquare.empty()) {
                auto [fR, fC] = squareToIndices(move.fromSquare);
                auto [tR, tC] = squareToIndices(move.toSquare);
                if (isMoveLegal(fR, fC, tR, tC, chessBoard)) {
                    chessBoard[tR][tC] = chessBoard[fR][fC];
                    chessBoard[fR][fC] = '.';
                    logMove(moveHistory, moveCounter, blackLabel, move.fromSquare, move.toSquare);
                }
            }
            currentTurn = 'w';
            aiTimer.restart();
        }

        // 7. Complete Graphics Render Draw Pipeline
        window.clear();
        window.draw(boardSprite);

        // Draw selection active frame highlight overlay
        if (!selectedSquare.empty()) {
            auto [selRow, selCol] = squareToIndices(selectedSquare);
            highlightRect.setPosition({boardStartX + (selCol * squareWidth), boardStartY + (selRow * squareHeight)});
            window.draw(highlightRect);
        }

        // Draw pieces layer
        for (int r = 0; r < 8; ++r) {
            for (int c = 0; c < 8; ++c) {
                char pieceKey = chessBoard[r][c];
                if (pieceKey != '.') {
                    pieceSprites[pieceKey]->setPosition({boardStartX + (c * squareWidth), boardStartY + (r * squareHeight)});
                    window.draw(*pieceSprites[pieceKey]);
                }
            }
        }

        // Draw lower dashboard status strip
        window.draw(textBg);
        window.draw(statusText);
        window.draw(timerText); 

        // Draw sidebar log panels
        window.draw(sidebarBg);
        window.draw(sidebarHeader);

        float textOffsetY = 60.f;
        for (const auto& logEntry : moveHistory) {
            sf::Text entryText(font, logEntry, 16);
            entryText.setFillColor(sf::Color(180, 180, 185));
            entryText.setPosition({static_cast<float>(imageSize.x) + 20.f, textOffsetY});
            window.draw(entryText);
            textOffsetY += 25.f;
        }

        window.display();
    }
    return 0;
}

