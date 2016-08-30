#include <iostream>
#include <algorithm>
#include <tuple>

#include <CImg.h>

#include "puzzle.h"

using namespace cimg_library;

bool line_contains_blocks(const CImg<unsigned char>& red_channel, const CImg<unsigned char>& green_channel, const CImg<unsigned char>& blue_channel) {
    auto red_count = std::count_if(red_channel.begin(), red_channel.end(), [](auto red) {
        return red >= 200;
    });
    auto green_count = std::count_if(green_channel.begin(), green_channel.end(), [](auto green) {
        return green >= 200;
    });
    auto blue_count = std::count_if(blue_channel.begin(), blue_channel.end(), [](auto blue) {
        return blue >= 200;
    });

    return red_count >= .26*red_channel.width() && blue_count <= .13*blue_channel.width() && green_count <= .26*green_channel.width();
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
    for (std::size_t line = 0; line < img.height(); ++line) {
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

    int x_start = -1, x_end = img.width()-1;
    for (std::size_t column = 0; column < img.width(); ++column) {
        int red = img(column, y_start, 0, 0);
        int green = img(column, y_start, 0, 1);
        int blue = img(column, y_start, 0, 2);

        if (red + green + blue < 200) {
            if (x_start == -1) {
                x_start = column;
            } else {
                x_end = column;
            }
        }
    }

    return std::make_tuple(std::max(0, x_start), std::max(0, y_start), x_end, y_end);
}

std::vector<Block> get_blocks(const CImg<unsigned char>& game_image) {
    static const int center_xs[] = {50, 150, 250, 350, 450, 550};
    static const int center_ys[] = {50, 150, 250, 350, 450, 550};
    static const int boarder_xs[] = {100, 200, 300, 400, 500};
    static const int boarder_ys[] = {100, 200, 300, 400, 500};

    bool game_grid[11][11]; ///true means block/boarder
    //Detect blocks
    for (const auto& row : center_ys) {
        for (const auto& col : center_xs) {
            game_grid[(row-50)/50][(col-50)/50] = game_image(col, row, 0, 0) >= 150;
        }
    }
    //Detect horizontal boarders
    for (const auto& row : boarder_ys) {
        for (const auto& col : center_xs) {
            game_grid[(row-50)/50][(col-50)/50] = game_image(col, row, 0, 0) < 150;
        }
    }
    //Detect vertical boarders
    for (const auto& row : center_ys) {
        for (const auto& col : boarder_xs) {
            game_grid[(row-50)/50][(col-50)/50] = game_image(col, row, 0, 0) < 150;
        }
    }

    //Extract block positions
    std::vector<Block> ret;
    bool visited[6][6] = {false};
    for (int row = 0; row < 6; ++row) {
        for (int col = 0; col < 6; ++col) {
            if (!visited[row][col]) {
                visited[row][col] = true;
                if (game_grid[2*row][2*col]) {
                    //There's a block here
                    Block block;
                    block.r = row; block.c = col;
                    block.length = 1;

                    int curr_row = row, curr_col = col;
                    while (curr_row < 5 && !game_grid[2*curr_row+1][2*curr_col]) {
                        //There's a part of the same block beneath this part
                        ++block.length;
                        block.orientation = false;
                        visited[++curr_row][curr_col] = true;
                    }
                    while (curr_col < 5 && !game_grid[2*curr_row][2*curr_col+1]) {
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
        int green = game_image(block.c*100 + 50, block.r*100 + 50, 0, 1);
        if (green < 50) {
            std::swap(block, ret.back());
            break;
        }
    }
    /*
    for (const auto& block : ret) {
        std::cout<<"Block {"<<std::endl;
        std::cout<<"\tr: "<<(int)block.r<<std::endl;
        std::cout<<"\tc: "<<(int)block.c<<std::endl;
        std::cout<<"\tlength: "<<(int)block.length<<std::endl;
        std::cout<<"\torientation: "<<block.orientation<<std::endl;
        std::cout<<"}"<<std::endl;
        std::cout<<std::endl;
    }
    */
    return ret;
}

Puzzle::Puzzle() {
}

Puzzle Puzzle::from_image(const std::string& path, float blur) {
    CImg<unsigned char> image(path.c_str());

    int x0, y0, x1, y1;
    std::tie(x0, y0, x1, y1) = get_game_region(image.blur(blur));

    Puzzle ret;
    ret.m_blocks = get_blocks(image.crop(x0, y0, x1, y1).resize(600, 600));
    if (ret.m_blocks.size() > 0) {
        ret.m_red_block = ret.m_blocks.back();
        ret.m_blocks.pop_back();
    }
    return ret;
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
                if (2*(block.r + i) + 1 << 11) {
                    grid[2*(block.r + i) + 1][2*block.c] = i + 1 < block.length ? ' ' : '-';
                }
            }
        }
    }
    for (size_t i = 0; i < p.m_red_block.length; i++) {
        if (p.m_red_block.orientation) {
            grid[2*p.m_red_block.r][2*(p.m_red_block.c + i)] = '$';
            grid[2*p.m_red_block.r][2*(p.m_red_block.c + i) + 1] = i + 1 < p.m_red_block.length ? ' ' : '|';
        } else {
            grid[2*(p.m_red_block.r + i)][2*p.m_red_block.c] = '$';
            grid[2*(p.m_red_block.r + i) + 1][2*p.m_red_block.c] = i + 1 < p.m_red_block.length ? ' ' : '-';
        }
    }

    std::cout<<"/-----------------------\\"<<std::endl;
    for (int row = 0; row < 11; ++row) {
        std::cout<<"| ";
        for (int col = 0; col < 11; ++col) {
            std::cout<<grid[row][col]<<' ';
        }
        std::cout<<'|'<<std::endl;
    }
    std::cout<<"\\-----------------------/"<<std::endl;
}
