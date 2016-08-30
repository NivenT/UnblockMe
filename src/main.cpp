#include <tuple>
#include <iostream>
#include <algorithm>

#include <CImg.h>

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

void print_board(const CImg<unsigned char>& game) {
    static const int center_xs[] = {50, 150, 250, 350, 450, 550};
    static const int center_ys[] = {50, 150, 250, 350, 450, 550};
    static const int boarder_xs[] = {100, 200, 300, 400, 500};
    static const int boarder_ys[] = {100, 200, 300, 400, 500};

    char game_grid[11][11];
    //Detect blocks
    for (const auto& row : center_ys) {
        for (const auto& col : center_xs) {
            int red = game(col, row, 0, 0);
            if (red >= 150) {
                int green = game(col, row, 0, 1);
                game_grid[(row-50)/50][(col-50)/50] = green > 50 ? '*' : '$';
            } else {
                game_grid[(row-50)/50][(col-50)/50] = ' ';
            }
        }
    }
    //Detect vertical boarders
    for (const auto& row : center_ys) {
        for (const auto& col : boarder_xs) {
            int red = game(col, row, 0, 0);
            if (red < 150) {
                game_grid[(row-50)/50][(col-50)/50] = '|';
            } else {
                game_grid[(row-50)/50][(col-50)/50] = ' ';
            }
        }
    }
    //Detect horizontal boarders
    for (const auto& row : boarder_ys) {
        for (const auto& col : center_xs) {
            int red = game(col, row, 0, 0);
            if (red < 150) {
                game_grid[(row-50)/50][(col-50)/50] = '-';
            } else {
                game_grid[(row-50)/50][(col-50)/50] = ' ';
            }
        }
    }
    //Clear everything else
    for (const auto& row : boarder_ys) {
        for (const auto& col : boarder_xs) {
            game_grid[(row-50)/50][(col-50)/50] = '+';
        }
    }

    std::cout<<"/----------------------\\"<<std::endl;
    for (int row = 0; row < 11; ++row) {
        std::cout<<'|';
        for (int col = 0; col < 11; ++col) {
            std::cout<<game_grid[row][col]<<' ';
        }
        std::cout<<'|'<<std::endl;
    }
    std::cout<<"\\----------------------/"<<std::endl;
}

int main() {
    CImg<unsigned char> image("puzzles/images/puzzle3.png"), visu(500,400,1,3,0);

    std::cout<<"Image dimensions: "<<image.width()<<" X "<<image.height()<<std::endl;

    const unsigned char red[] = { 255,0,0 }, green[] = { 0,255,0 }, blue[] = { 0,0,255 };
    const unsigned char white[] = {255, 255, 255};

    int x1, y1, x2, y2;
    std::tie(x1, y1, x2, y2) = get_game_region(image.get_blur(1.5));
    print_board(image.get_crop(x1, y1, x2, y2).get_resize(600, 600));
    image.draw_rectangle(x1, y1, x2, y2, white, 1, ~0);

    CImgDisplay main_disp(image,"Click a point"), draw_disp(visu,"Intensity profile");
    while (!main_disp.is_closed() && !draw_disp.is_closed()) {
        main_disp.wait();
        if (main_disp.button() && main_disp.mouse_y()>=0) {
            const int y = main_disp.mouse_y();
            visu.fill(0).draw_graph(image.get_crop(0,y,0,0,image.width()-1,y,0,0),red,1,1,0,255,0);
            visu.draw_graph(image.get_crop(0,y,0,1,image.width()-1,y,0,1),green,1,1,0,255,0);
            visu.draw_graph(image.get_crop(0,y,0,2,image.width()-1,y,0,2),blue,1,1,0,255,0).display(draw_disp);
        }
    }
    return 0;
}
