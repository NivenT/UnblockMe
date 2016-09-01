#ifndef PUZZLE_H_INCLUDED
#define PUZZLE_H_INCLUDED

#include <ostream>
#include <vector>

#include <CImg.h>

struct Block {
    Block& advance(bool dir) {
        switch((orientation << 1) | dir) {
            case 0: --r; break;
            case 1: ++r; break;
            case 2: --c; break;
            case 3: ++c; break;
        }
        return *this;
    }
    bool is_in_bounds() const {
        return c < 6 && r < 6 && (orientation ? c + length < 7 : r + length < 7);
    }
    bool operator==(const Block& rhs) const {
        return r == rhs.r && c == rhs.c && length == rhs.length && orientation == rhs.orientation;
    }
    bool operator!=(const Block& rhs) const {
        return !(*this == rhs);
    }

    uint8_t r, c;           //location of top left block
    uint8_t length;
    bool orientation;       //true for horizontal and false for vertical
};

struct Move {
    uint8_t index;          //the block to move
    uint8_t num_steps;
    bool direction;
};

class Puzzle {
public:
    ~Puzzle();

    std::vector<Move> get_valid_moves() const;
    Puzzle make_move(const Move& m) const;
    const Block& get_red_block() const;
    bool to_file(const std::string& path) const;
    bool is_overlapping(const Block& blk1, uint8_t index_to_ignore = -1) const;
    cimg_library::CImg<unsigned char>& draw(cimg_library::CImg<unsigned char>& img) const;

    static Puzzle from_image(const std::string& path, float blur = 1.5);
    static Puzzle from_file(const std::string& path);

    bool operator==(const Puzzle& rhs) const;

    friend std::ostream& operator<<(std::ostream&, const Puzzle&);
    friend std::hash<Puzzle>;
private:
    Puzzle();   ///Puzzles only constructable using static methods

    std::vector<Block> m_blocks;    //The last block is the red one
};

std::ostream& operator<<(std::ostream& out, const Puzzle& p);

#endif // PUZZLE_H_INCLUDED
