#ifndef XAVIER_CHESS_AI_H
#define XAVIER_CHESS_AI_H

#include <vector>
#include <string>
#include <cctype>
#include "ChessAIBase.h" // Include base class

class Xavier_ChessAI : public ChessAIBase { // Fix: Inherit from base
public:
    // Fix: Mark method as override
    ChessMove makeMove(const std::vector<std::vector<char>>& board, char aiColor) override {
        // Good Luck! Start Here! :) - Ved
    }
};

REGISTER_CHESS_AI(Xavier_ChessAI, "Xavier");
#endif
