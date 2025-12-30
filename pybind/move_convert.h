#pragma once
#include "libpentobi_base/Board.h"
#include <vector>
#include <optional>
#include <unordered_map>
#include <string>
#include <cstdint>

struct Block {
    char name;
    int width;
    int height;
    int size;
    std::vector<int> corners; 
    std::vector<std::vector<std::pair<int, int>>> coords;

    Block(char name, int size, int width, int height, const std::vector<std::vector<std::pair<int, int>>> & coords, const std::vector<int>& corners)
        : name(name), size(size), width(width), height(height), coords(coords), corners(corners){}
};

extern const Block rotatedPieces[21];
void pbd_vec(const std::vector<int>& b);

void pbd(const libpentobi_base::Board& b);

extern std::unordered_map<std::string, int> pieceMap;

struct TurnBaseMove{
  uint8_t pieceId;
  uint8_t row;
  uint8_t col;
  uint8_t rotation;
};

std::vector<TurnBaseMove> generateMap();

std::optional<std::pair<int, int>> findMove(const std::vector<int>& inBoard, const std::vector<int>& outBoard, const std::vector<std::pair<int, int>>& coords);

std::vector<int> applyMove(int row, int col, std::vector<std::pair<int, int>> coords);

std::vector<int> makeBoard(const libpentobi_base::Board& b);
