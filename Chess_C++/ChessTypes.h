#ifndef CHESS_TYPES_H
#define CHESS_TYPES_H

#include <string>

// Global structures shared across the entire framework
struct ChessMove {
    std::string fromSquare; // e.g., "e2"
    std::string toSquare;   // e.g., "e4"
};

#endif
