#include "solver.h"

//Boost's hash_combine
size_t hash_combine(size_t lhs, size_t rhs) {
    lhs ^= (rhs + 0x9e3779b9 + (lhs << 6) + (lhs >> 2));
}

Solver::Solver() {
}

Solver::~Solver() {
}

int Solver::heuristic(const Puzzle& p) {
    return 0;
}
