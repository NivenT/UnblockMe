#include <iostream>
#include <tuple>

#include <CImg.h>

#include "puzzle.h"

using namespace cimg_library;

extern std::tuple<int, int, int, int> get_game_region(const CImg<unsigned char>&);

int main() {
    srand(time(nullptr));
    int which_puzzle = rand()%5;

    std::string image_path = "puzzles/images/puzzle";
    image_path += (char)(which_puzzle + '1');
    image_path += (which_puzzle && which_puzzle != 4 ? ".png" : ".jpg");

    CImg<unsigned char> image(image_path.c_str()), visu(500,400,1,3,0);
    const unsigned char red[] = {255, 0, 0}, green[] = {0, 255, 0}, blue[] = {0, 0, 255};
    const unsigned char white[] = {255, 255, 255};

    Puzzle puzzle = Puzzle::from_image(image_path);

    if (puzzle.to_file("puzzles/files/puzzle.puz")) {
        std::cout<<"Saved puzzle to file"<<std::endl;
        puzzle = Puzzle::from_file("puzzles/files/puzzle.puz");
    } else {
        std::cout<<"Could not save puzzle"<<std::endl;
    }
    std::cout<<puzzle<<std::endl;

    int x0, y0, x1, y1;
    std::tie(x0, y0, x1, y1) = get_game_region(image.blur(1.5));
    /**/
    image.draw_rectangle(x0, y0, x1, y1, white, 1, ~0);
    /**
    image.crop(x0, y0, x1, y1).resize(600, 600);
    for (int x = 0; x < 11; x++) {
        for (int y = 0; y < 11; y++) {
            image.draw_circle(x*50+50, y*50+50, 1, white);
        }
    }
    /**/

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
