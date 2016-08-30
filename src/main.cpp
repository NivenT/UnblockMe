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

    return red_count >= 400 && blue_count <= 100 && green_count <= 200;
}

std::tuple<int, int, int, int> get_game_region(const CImg<unsigned char>& img) {
    int y_start = 0, y_end = img.height()-1;
    for (std::size_t line = 0; line < img.height(); ++line) {
        auto red_channel = img.get_crop(0, line, 0, 0, img.width()-1, line, 0, 0);
        auto green_channel = img.get_crop(0, line, 0, 1, img.width()-1, line, 0, 1);
        auto blue_channel = img.get_crop(0, line, 0, 2, img.width()-1, line, 0, 2);

        if (line_contains_blocks(red_channel, green_channel, blue_channel)) {
            if (y_start == 0) {
                y_start = line;
            } else {
                y_end = line;
            }
        }
    }

    int x_start = 0, x_end = img.width()-1;
    for (std::size_t column = 0; column < img.width(); ++column) {
        int red = img(column, y_start, 0, 0);
        int green = img(column, y_start, 0, 1);
        int blue = img(column, y_start, 0, 2);

        if (red + green + blue < 200) {
            if (x_start == 0) {
                x_start = column;
            } else {
                x_end = column;
            }
        }
    }

    return std::make_tuple(x_start, y_start, x_end, y_end);
}

int main() {
    CImg<unsigned char> image("puzzles/images/puzzle1.jpg"), visu(500,400,1,3,0);
    const unsigned char red[] = { 255,0,0 }, green[] = { 0,255,0 }, blue[] = { 0,0,255 };
    const unsigned char white[] = {255, 255, 255}, black[] = {0, 0, 0};

    int x1, y1, x2, y2;
    std::tie(x1, y1, x2, y2) = get_game_region(image.get_blur(2.5));
    //image.draw_rectangle(x1, y1, x2, y2, white, 1, ~0);
    image.crop(x1, y1, x2, y2);

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
