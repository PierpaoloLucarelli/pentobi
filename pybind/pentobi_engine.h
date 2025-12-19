#pragma once

#include <libpentobi_mcts/Float.h>
#include <cstddef>
#include <string>


class PentobiEngine{
    public:
        PentobiEngine(libpentobi_mcts::Float max_n_iterations, size_t min_n_sims, double max_time);
        void parse_move_str(const std::string& moves_str);

    private:
        libpentobi_mcts::Float max_n_iterations;
        size_t min_n_sims;
        double max_time;


};