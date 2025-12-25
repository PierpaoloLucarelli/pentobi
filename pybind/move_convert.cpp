#include <vector>
#include <iostream>
#include "move_convert.h"
#include <cstdint>
#include <stdexcept>
#include <algorithm>

constexpr int WIDTH  = 20;
constexpr int HEIGHT = 20;



/* ===========================
   Board helpers
   =========================== */

std::vector<int> applyMove(
    const std::vector<std::pair<int, int>>& move,
    const std::vector<int>& board,
    int player
) {
    std::vector<int> out = board;

    for (const auto& [x, y] : move) {
        if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT)
            throw std::out_of_range("Out of bounds");

        int idx = y * WIDTH + x;
        if (out[idx] != 0)
            throw std::logic_error("Cell occupied");

        out[idx] = player;
    }
    return out;
}

bool boardsEqual(const std::vector<int>& a, const std::vector<int>& b) {
    return a == b;
}

/* ===========================
   Diff detection
   =========================== */

std::vector<std::pair<int,int>> getPlacedCells(
    const std::vector<int>& boardIn,
    const std::vector<int>& boardOut,
    int playerId
) {
    std::vector<std::pair<int,int>> cells;

    for (int i = 0; i < WIDTH * HEIGHT; ++i) {
        if (boardIn[i] == 0 && boardOut[i] == playerId) {
            cells.emplace_back(i % WIDTH, i / WIDTH);
        }
    }
    return cells;
}

/* ===========================
   Move reconstruction
   =========================== */

TurnBaseMove convertMove(
    int pieceId,
    int playerId,
    const std::vector<int>& boardIn,
    const std::vector<int>& boardOut
) {
    const Block& piece = rotatedPieces[pieceId];
    std::cout << "start" << std::endl;
    auto placed = getPlacedCells(boardIn, boardOut, playerId);
    if ((int)placed.size() != piece.size)
        throw std::logic_error("Placed cell count mismatch");

    for (uint8_t r = 0; r < 8; ++r) {
        const auto& rel = piece.coords[r];

        for (const auto& [px, py] : placed) {
            for (const auto& [rx, ry] : rel) {

                int dx = px - rx;
                int dy = py - ry;

                try {
                    std::vector<std::pair<int,int>> abs;
                    abs.reserve(piece.size);

                    for (const auto& [x, y] : rel)
                        abs.emplace_back(x + dx, y + dy);

                    auto test = applyMove(abs, boardIn, playerId);
                    if (boardsEqual(test, boardOut)) {
                        return TurnBaseMove{
                            pieceId,
                            dy,                 // row
                            dx,                 // col
                            r,                  // rotation
                            (uint8_t)playerId
                        };
                    }
                } catch (...) {
                    // invalid placement, ignore
                }
            }
        }
    }
    
    std::cout << "end" << std::endl;
    throw std::logic_error("No valid move found");
}
