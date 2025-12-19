#include "pentobi_engine.h"
#include <iostream>

PentobiEngine::PentobiEngine(libpentobi_mcts::Float max_n_iterations,  size_t min_n_sims, double max_time): 
    max_n_iterations(max_n_iterations), min_n_sims(min_n_sims), max_time(max_time){

};

void PentobiEngine::parse_move_str(const std::string& moves_str){
    std::cout << moves_str << std::endl;
};

