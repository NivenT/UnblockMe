#include "solver.h"

//Boost's hash_combine
void hash_combine(size_t& lhs, size_t rhs) {
    lhs ^= (rhs + 0x9e3779b9 + (lhs << 6) + (lhs >> 2));
}

Solver::Solver() {
}

Solver::~Solver() {
}

bool Solver::puzzle_is_solved(const Puzzle& p) {

}

int Solver::heuristic(const Puzzle& p) {
    return 0;
}

Path Solver::solve_puzzle(const Puzzle& p) {
    //Reset solver
    while (!m_paths.empty()) m_paths.pop();
    m_seen.clear();

    Path* curr = new Path(p);
    m_seen.insert(p);
    m_paths.push(curr);
    do {
        0;
    } while (false);
}
