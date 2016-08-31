#ifndef SOLVER_H_INCLUDED
#define SOLVER_H_INCLUDED

#include <queue>
#include <unordered_set>

#include "puzzle.h"

extern void hash_combine(size_t& lhs, size_t rhs);

namespace std {
    template<>
    struct hash<Block> {
        typedef Block argument_type;
        typedef size_t result_type;

        result_type operator()(const argument_type& b) const {
            size_t h = 0;
            hash_combine(h, std::hash<uint8_t>()(b.r));
            hash_combine(h, std::hash<uint8_t>()(b.c));
            hash_combine(h, std::hash<uint8_t>()(b.length));
            hash_combine(h, std::hash<bool>()(b.orientation));
            return h;
        }
    };

    template<>
    struct hash<Puzzle> {
        typedef Puzzle argument_type;
        typedef size_t result_type;

        result_type operator()(const argument_type& p) const {
            size_t h = 0;
            for (const auto& block : p.m_blocks) {
                hash_combine(h, std::hash<Block>()(block));
            }
            return h;
        }
    };
}

struct Path {
    Path(const Puzzle& puz) : m_puz(puz), m_tail(nullptr) {
    }
    ~Path() {
        if (m_tail) {
            delete m_tail;
        }
    }
    const Puzzle& get_end() const {
        return m_tail ? m_tail->get_end() : m_puz;
    }
    int length() const {
        return m_tail ? 1 + m_tail->length() : 0;
    }

    const Puzzle& m_puz;
    Path* m_tail;
};

class Solver {
public:
    Solver();
    ~Solver();
    Path solve_puzzle(const Puzzle& p);
private:
    struct PathCompare {
        bool operator()(const Path* lhs, const Path* rhs) {
            return (lhs->length() + heuristic(lhs->get_end())) > (rhs->length() + heuristic(rhs->get_end()));
        }
    };
    static bool puzzle_is_solved(const Puzzle& p);
    static int heuristic(const Puzzle& p); //lower bound for number of remaining moves

    std::priority_queue<Path*, std::vector<Path*>, PathCompare> m_paths;
    std::unordered_set<Puzzle> m_seen;
};

#endif // SOLVER_H_INCLUDED