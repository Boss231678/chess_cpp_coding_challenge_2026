#ifndef PLAYER2_CHESS_AI_H
#define PLAYER2_CHESS_AI_H

#include <vector>
#include <string>
#include <cctype>
#include "ChessAIBase.h" // Include base class

class Player2_ChessAI : public ChessAIBase { // Fix: Inherit from base
public:
    // Fix: Mark method as override
    ChessMove makeMove(const std::vector<std::vector<char>>& board, char aiColor) override {
        ChessMove decidedMove;
        int dir = (aiColor == 'w') ? -1 : 1;
        int startRow = (aiColor == 'w') ? 6 : 1;

        int centerFiles[] = {4, 3, 2, 5, 1, 6, 0, 7}; 
        for (int c : centerFiles) {
            if (board[startRow][c] != '.' && toupper(board[startRow][c]) == 'P') {
                if (board[startRow + dir][c] == '.' && board[startRow + 2 * dir][c] == '.') {
                    decidedMove.fromSquare = std::string(1, 'a' + c) + std::to_string(8 - startRow);
                    decidedMove.toSquare = std::string(1, 'a' + c) + std::to_string(8 - (startRow + 2 * dir));
                    return decidedMove;
                }
            }
        }

        for (int r = 0; r < 8; ++r) {
            for (int c = 0; c < 8; ++c) {
                char p = board[r][c];
                if ((aiColor == 'w' && isupper(p)) || (aiColor == 'b' && islower(p))) {
                    if (r + dir >= 0 && r + dir < 8 && board[r + dir][c] == '.') {
                        decidedMove.fromSquare = std::string(1, 'a' + c) + std::to_string(8 - r);
                        decidedMove.toSquare = std::string(1, 'a' + c) + std::to_string(8 - (r + dir));
                        return decidedMove;
                    }
                }
            }
        }
        return decidedMove;
    }
};

REGISTER_CHESS_AI(Player2_ChessAI, "player2");
#endif
