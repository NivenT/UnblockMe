#include <iostream>
#include <stack>

#include <CImg.h>

#include "solver.h"

using namespace cimg_library;
using namespace std;

int main() {
    Solver solver;
    Puzzle puzzle = Puzzle::from_image("puzzles/images/puzzle7.jpg");

    CImg<unsigned char> puzzle_image;
    CImgDisplay disp;

    cout<<puzzle<<endl;
    auto solution = solver.solve_puzzle(puzzle);
    if (solution) {
        cout<<"Solved puzzle in "<<solution->length()<<" moves"<<endl;

        stack<Puzzle> steps;
        while (solution) {
            steps.push(solution->m_puz);
            solution = solution->m_prior;
        }
        while (!steps.empty()) {
            cout<<steps.top()<<endl;
            steps.top().draw(puzzle_image).display(disp);

            cin.get();
            steps.pop();
        }
    } else {
        cout<<"Could not solve puzzle"<<endl;
    }
    return 0;
}
