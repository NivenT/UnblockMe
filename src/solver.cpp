#include <algorithm>

#include "solver.h"

//Boost's hash_combine
void hash_combine(size_t& lhs, size_t rhs) {
    lhs ^= (rhs + 0x9e3779b9 + (lhs << 6) + (lhs >> 2));
}

Solver::Solver() {
}

Solver::~Solver() {
    m_seen.clear();
    while (!m_paths.empty()) {
        m_paths.pop();
    }
}

bool Solver::puzzle_is_solved(const Puzzle& p) {
    return p.get_red_block().c == 4;
}

int Solver::heuristic(const Puzzle& p) {
    return 0; ///Previous heurisitc was not a strict lower bound and the program runs fast enough without needing a heurisitic
}

Path* Solver::solve_puzzle(const Puzzle& p) {
    //Reset solver
    while (!m_paths.empty()) m_paths.pop();
    m_seen.clear();

    m_seen.insert(p);
    m_paths.push(new Path(p));
    while (!m_paths.empty()) {
        Path* curr = m_paths.top(); m_paths.pop();
        const Puzzle& last = curr->m_puz;

        if (puzzle_is_solved(last)) return curr;
        for (const auto& m : last.get_valid_moves()) {
            Puzzle next = last.make_move(m);
            if (m_seen.find(next) != m_seen.end()) continue;

            Path* child = new Path(next);
            child->m_prior = curr;

            m_paths.push(child);
            m_seen.insert(child->m_puz);
        }
    }
    return nullptr;
}
