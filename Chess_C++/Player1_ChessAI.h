#ifndef PLAYER1_CHESS_AI_H
#define PLAYER1_CHESS_AI_H

#include <vector>
#include <string>
#include <cctype>
#include "ChessAIBase.h" // Include base class

class Player1_ChessAI : public ChessAIBase { // Fix: Inherit from base
public:
    // Fix: Mark method as override
    ChessMove makeMove(const std::vector<std::vector<char>>& board, char aiColor) override {
        ChessMove decidedMove;
        std::vector<ChessMove> legalCaptures;
        std::vector<ChessMove> legalQuietMoves;

        auto checkAddMove = [&](int fR, int fC, int tR, int tC) {
            if (tR < 0 || tR >= 8 || tC < 0 || tC >= 8) return;
            char target = board[tR][tC];
            if (aiColor == 'w' && target != '.' && isupper(target)) return;
            if (aiColor == 'b' && target != '.' && islower(target)) return;

            ChessMove mv;
            mv.fromSquare = std::string(1, 'a' + fC) + std::to_string(8 - fR);
            mv.toSquare = std::string(1, 'a' + tC) + std::to_string(8 - tR);

            if (target != '.') legalCaptures.push_back(mv);
            else legalQuietMoves.push_back(mv);
        };

        for (int r = 0; r < 8; ++r) {
            for (int c = 0; c < 8; ++c) {
                char p = board[r][c];
                if ((aiColor == 'w' && isupper(p)) || (aiColor == 'b' && islower(p))) {
                    if (toupper(p) == 'P') {
                        int dir = (aiColor == 'w') ? -1 : 1;
                        checkAddMove(r, c, r + dir, c);
                        checkAddMove(r, c, r + dir, c - 1);
                        checkAddMove(r, c, r + dir, c + 1);
                    } else if (toupper(p) == 'N') {
                        int dr[] = {-2, -2, -1, -1, 1, 1, 2, 2};
                        int dc[] = {-1, 1, -2, 2, -2, 2, -1, 1};
                        for (int i = 0; i < 8; ++i) checkAddMove(r, c, r + dr[i], c + dc[i]);
                    }
                }
            }
        }

        if (!legalCaptures.empty()) return legalCaptures[0];
        if (!legalQuietMoves.empty()) return legalQuietMoves[0];
        return decidedMove;
    }
};

REGISTER_CHESS_AI(Player1_ChessAI, "player1");
#endif
