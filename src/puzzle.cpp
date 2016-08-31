#include <algorithm>
#include <iostream>
#include <fstream>
#include <tuple>

#include <CImg.h>

#include "puzzle.h"

using namespace cimg_library;

bool line_contains_blocks(const CImg<unsigned char>& red_channel, const CImg<unsigned char>& green_channel, const CImg<unsigned char>& blue_channel) {
    auto red_count = std::count_if(red_channel.begin(), red_channel.end(), [](auto red) {
        return red >= 200;
    });
    return red_count >= .26*red_channel.width();
}

bool line_is_blank(const CImg<unsigned char>& red_channel, const CImg<unsigned char>& green_channel, const CImg<unsigned char>& blue_channel) {
    static const int threshold = .91*red_channel.width();
    auto red_count = std::count_if(red_channel.begin(), red_channel.end(), [](auto red) {
        return 60 <= red && red <= 80;
    });
    auto green_count = std::count_if(green_channel.begin(), green_channel.end(), [](auto green) {
        return 30 <= green && green <= 50;
    });
    auto blue_count = std::count_if(blue_channel.begin(), blue_channel.end(), [](auto blue) {
        return blue <= 20;
    });
    return red_count >= threshold && blue_count >= threshold && green_count >= threshold;
}

std::tuple<int, int, int, int> get_game_region(const CImg<unsigned char>& img) {
    int y_start = -1, y_end = img.height()-1;
    for (size_t line = 0; line < img.height(); ++line) {
        auto red_channel = img.get_crop(0, line, 0, 0, img.width()-1, line, 0, 0);
        auto green_channel = img.get_crop(0, line, 0, 1, img.width()-1, line, 0, 1);
        auto blue_channel = img.get_crop(0, line, 0, 2, img.width()-1, line, 0, 2);

        if (line_contains_blocks(red_channel, green_channel, blue_channel) || line_is_blank(red_channel, green_channel, blue_channel)) {
            if (y_start == -1) {
                y_start = line;
            } else {
                y_end = line;
            }
        }
    }
    y_start = std::max(y_start, 0);

    int x_start = -1, x_end = img.width()-1;
    int y = (y_start+3*y_end)/4; //Get a line in the lower part, since it will be more "typical" than the top or middle
    for (size_t column = 0; column < img.width(); ++column) {
        int red = img(column, y, 0, 0);
        int green = img(column, y, 0, 1);
        int blue = img(column, y, 0, 2);

        if (red + green + blue < 220) {
            if (x_start == -1) {
                int nearby_red = img(std::max<int>(column, 5)-5, y, 0, 0);
                x_start = nearby_red > 120 ? column : x_start;
            } else {
                int nearby_red = img(std::max<int>(column, 5)+5, y, 0, 0);
                int nearby_blue = img(std::max<int>(column, 5)+5, y, 0, 2);
                x_end = (nearby_red >= 150 && nearby_blue < 30) || (nearby_blue < 70 && nearby_red >= 100) ? column : x_end;
            }
        }
    }
    x_start = std::max(x_start, 0);

    return std::make_tuple(x_start, y_start, x_end, y_end);
}

std::vector<Block> get_blocks(uint8_t game_grid[11][11]) {
    //Extract block positions
    std::vector<Block> ret;
    bool visited[6][6] = {false};
    for (size_t row = 0; row < 6; ++row) {
        for (size_t col = 0; col < 6; ++col) {
            if (!visited[row][col]) {
                visited[row][col] = true;
                if (game_grid[2*row][2*col]) {
                    //There's a block here
                    Block block;
                    block.r = row; block.c = col;
                    block.length = 1;

                    int curr_row = row, curr_col = col;
                    while (curr_row < 5 && !game_grid[2*curr_row+1][2*curr_col] && game_grid[2*(curr_row+1)][2*curr_col]) {
                        //There's a part of the same block beneath this part
                        ++block.length;
                        block.orientation = false;
                        visited[++curr_row][curr_col] = true;
                    }
                    while (curr_col < 5 && !game_grid[2*curr_row][2*curr_col+1] && game_grid[2*curr_row][2*(curr_col+1)]) {
                        //There's a part of the same block next to this part
                        ++block.length;
                        block.orientation = true;
                        visited[curr_row][++curr_col] = true;
                    }

                    ret.push_back(block);
                }
            }
        }
    }

    //move red block to the end
    for (auto& block : ret) {
        if (game_grid[block.r*2][block.c*2] == 2) {
            std::swap(block, ret.back());
            break;
        }
    }
    /**
    for (const auto& block : ret) {
        std::cout<<"Block {"<<std::endl;
        std::cout<<"\tr: "<<(int)block.r<<std::endl;
        std::cout<<"\tc: "<<(int)block.c<<std::endl;
        std::cout<<"\tlength: "<<(int)block.length<<std::endl;
        std::cout<<"\torientation: "<<block.orientation<<std::endl;
        std::cout<<"}"<<std::endl;
        std::cout<<std::endl;
    }
    /**/
    return ret;
}

std::vector<Block> get_blocks(const CImg<unsigned char>& game_image) {
    static const int center_xs[] = {50, 150, 250, 350, 450, 550};
    static const int center_ys[] = {50, 150, 250, 350, 450, 550};
    static const int boarder_xs[] = {100, 200, 300, 400, 500};
    static const int boarder_ys[] = {100, 200, 300, 400, 500};

    uint8_t game_grid[11][11]; ///1 means block/boarder, 2 means red block
    //Detect blocks
    for (const auto& row : center_ys) {
        for (const auto& col : center_xs) {
            game_grid[(row-50)/50][(col-50)/50] = game_image(col, row, 0, 0) < 180 ? 0 :
                                                  game_image(col, row, 0, 1) >= 50 ? 1 : 2;
        }
    }
    //Detect horizontal boarders
    for (const auto& row : boarder_ys) {
        for (const auto& col : center_xs) {
            game_grid[(row-50)/50][(col-50)/50] = game_image(col, row, 0, 0) < 180;
        }
    }
    //Detect vertical boarders
    for (const auto& row : center_ys) {
        for (const auto& col : boarder_xs) {
            game_grid[(row-50)/50][(col-50)/50] = game_image(col, row, 0, 0) < 180;
        }
    }

    return get_blocks(game_grid);
}

Puzzle::Puzzle() {
}

Puzzle::~Puzzle() {
    m_blocks.clear();
}

Puzzle Puzzle::from_image(const std::string& path, float blur) {
    CImg<unsigned char> image(path.c_str());

    int x0, y0, x1, y1;
    std::tie(x0, y0, x1, y1) = get_game_region(image.blur(blur));

    Puzzle ret;
    ret.m_blocks = get_blocks(image.crop(x0, y0, x1, y1).resize(600, 600));
    return ret;
}

Puzzle Puzzle::from_file(const std::string& path) {
    std::ifstream file(path.c_str());
    char* file_grid[11];

    std::string line;
    std::getline(file, line); //First line is useless
    for (size_t i = 0; i < 11; ++i) {
        std::getline(file, line);

        file_grid[i] = new char[line.size()];
        strcpy(file_grid[i], line.c_str());
    }
    file.close();

    uint8_t game_grid[11][11];
    for (size_t row = 0; row < 11; ++row) {
        for (size_t col = 0; col < 11; ++col) {
            char c = file_grid[row][2*(col+1)];
            game_grid[row][col] = (c == '*' || c == '|' || c == '-') ? 1 :
                                  (c == '$')                         ? 2 : 0;
        }
    }

    for (size_t i = 0; i < 11; ++i) {
        delete[] file_grid[i];
    }

    Puzzle ret;
    ret.m_blocks = get_blocks(game_grid);
    return ret;
}

bool Puzzle::is_overlapping(const Block& blk1, uint8_t index_to_ignore) const {
    uint8_t r00 = blk1.r, r01 = blk1.r,     //range of rows occupied by blk1
            c00 = blk1.c, c01 = blk1.c;
    if (blk1.orientation) {
        c01 += blk1.length - 1;
    } else {
        r01 += blk1.length - 1;
    }

    for (size_t i = 0; i < m_blocks.size(); ++i) {
        if (i == index_to_ignore) continue; //Dont wanna check against yourself

        const Block& blk2 = m_blocks[i];
        uint8_t r10 = blk2.r, r11 = blk2.r,
                c10 = blk2.c, c11 = blk2.c;
        if (blk2.orientation) {
            c11 += blk2.length - 1;
        } else {
            r11 += blk2.length - 1;
        }

        if (r00 > r11 || c00 > c11 || r10 > r01 || c10 > c01) continue;
        return true;
    }
    return false;
}

std::vector<Move> Puzzle::get_valid_moves() const {
    std::vector<Move> ret;
    for (size_t i = 0; i < m_blocks.size(); ++i) {
        Move curr;
        curr.index = i;
        for (const auto& dir : {false, true}) {
            curr.direction = dir;
            curr.num_steps = 0;

            Block temp = m_blocks[i];
            while (temp.advance(dir).is_in_bounds() && !is_overlapping(temp, i)) {
                ++curr.num_steps;
                ret.push_back(curr);
            }
        }
    }
    return ret;
}

Puzzle Puzzle::make_move(const Move& m) const {
    Puzzle ret = *this;
    if (m.index < ret.m_blocks.size()) {
        Block temp = ret.m_blocks[m.index];
        uint8_t num_steps = m.num_steps;

        while (num_steps--) temp.advance(m.direction);
        if (temp.is_in_bounds() && !is_overlapping(temp, m.index)) ret.m_blocks[m.index] = temp;
    }
    return ret;
}

const Block& Puzzle::get_red_block() const {
    return m_blocks.back();
}

bool Puzzle::to_file(const std::string& path) const {
    std::ofstream file(path.c_str());
    if (!file.is_open()) return false;
    file<<*this;
    file.close();
    return true;
}

#include <iostream>

bool Puzzle::operator==(const Puzzle& rhs) const {
    if (m_blocks.size() != rhs.m_blocks.size() || m_blocks.back() != rhs.m_blocks.back()) return false;
    for (size_t i = 0; i < m_blocks.size()-1; ++i) {
        if (std::find(rhs.m_blocks.begin(), rhs.m_blocks.end()-1, m_blocks[i]) == rhs.m_blocks.end()-1) return false;
    }
    return true;
}

std::ostream& operator<<(std::ostream& out, const Puzzle& p) {
    char grid[11][11];
    //block locations
    for (size_t row = 0; row < 11; row += 2) {
        for (size_t col = 0; col < 11; col += 2) {
            grid[row][col] = ' ';
        }
    }
    //horizontal boarders
    for (size_t row = 1; row < 11; row += 2) {
        for (size_t col = 0; col < 11; col += 2) {
            grid[row][col] = '-';
        }
    }
    //vertical boarders
    for (size_t row = 0; row < 11; row += 2) {
        for (size_t col = 1; col < 11; col += 2) {
            grid[row][col] = '|';
        }
    }
    //boarder intersections
    for (size_t row = 1; row < 11; row += 2) {
        for (size_t col = 1; col < 11; col += 2) {
            grid[row][col] = '+';
        }
    }

    //blocks
    for (const auto& block : p.m_blocks) {
        for (size_t i = 0; i < block.length; i++) {
            if (block.orientation) {
                grid[2*block.r][2*(block.c + i)] = '*';
                if (2*(block.c + i) + 1 < 11) {
                    grid[2*block.r][2*(block.c + i) + 1] = i + 1 < block.length ? ' ' : '|';
                }
            } else {
                grid[2*(block.r + i)][2*block.c] = '*';
                if (2*(block.r + i) + 1 < 11) {
                    grid[2*(block.r + i) + 1][2*block.c] = i + 1 < block.length ? ' ' : '-';
                }
            }
        }
    }
    for (size_t i = 0; i < p.m_blocks.back().length; i++) {
        const Block& red_block = p.m_blocks.back();
        if (red_block.orientation) {
            grid[2*red_block.r][2*(red_block.c + i)] = '$';
        } else {
            grid[2*(red_block.r + i)][2*red_block.c] = '$';
        }
    }

    out<<"/-----------------------\\"<<std::endl;
    for (size_t row = 0; row < 11; ++row) {
        out<<"| ";
        for (size_t col = 0; col < 11; ++col) {
            out<<grid[row][col]<<' ';
        }
        out<<'|'<<std::endl;
    }
    out<<"\\-----------------------/";
    return out;
}
