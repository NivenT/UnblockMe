#include <iostream>

#include "solver.h"

using namespace std;

int main() {
    Solver solver;
    Puzzle puzzle = Puzzle::from_file("puzzles/files/puzzle3.puz");

    cout<<puzzle<<endl;

    Path* solution = solver.solve_puzzle(puzzle);
    if (solution) {
        cout<<"Solved puzzle in "<<solution->length()<<" moves"<<endl;
    } else {
        cout<<"Could not solve puzzle"<<endl;
    }

    return 0;
}
