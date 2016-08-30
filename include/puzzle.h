#ifndef PUZZLE_H_INCLUDED
#define PUZZLE_H_INCLUDED

#include <cinttypes>
#include <ostream>
#include <vector>

struct Block {
    uint8_t r, c;           //location of top left block
    uint8_t length;
    bool orientation;       //true for horizontal and false for vertical
};

class Puzzle {
public:
    static Puzzle from_image(const std::string& path, float blur = 1.5);

    friend std::ostream& operator<<(std::ostream&, const Puzzle&);
private:
    Puzzle(); ///Puzzles only constructable using static methods

    std::vector<Block> m_blocks;
    Block m_red_block;
};

std::ostream& operator<<(std::ostream& out, const Puzzle& p);

#endif // PUZZLE_H_INCLUDED
