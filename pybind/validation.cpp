#include "validation.h"
#include <cctype>

bool is_valid_move_string(const std::string& s) {
    if (s.empty() || s.size() > 64) {
        return false;
    }

    for (char c : s) {
        if (!std::isalnum(c) && c != ',' && c != '-') {
            return false;
        }
    }
    return true;
}

