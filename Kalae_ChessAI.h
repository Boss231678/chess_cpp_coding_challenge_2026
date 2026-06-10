#ifndef KALAE_CHESS_AI_H
#define KALAE_CHESS_AI_H

#include <vector>
#include <string>
#include <cctype>
#include "ChessAIBase.h" // Include base class

class Kalae_ChessAI : public ChessAIBase { // Fix: Inherit from base
public:
    // Fix: Mark method as override
    ChessMove makeMove(const std::vector<std::vector<char>>& board, char aiColor) override {
        // Good Luck! Start Here! :) - Ved
    }
};

REGISTER_CHESS_AI(Kalae_ChessAI, "Kalae");
#endif
