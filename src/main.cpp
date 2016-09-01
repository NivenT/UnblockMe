#include <iostream>
#include <stack>

#include "solver.h"

using namespace std;

int main() {
    Solver solver;
    Puzzle puzzle = Puzzle::from_image("puzzles/images/puzzle7.jpg");

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
            cin.get();
            steps.pop();
        }
    } else {
        cout<<"Could not solve puzzle"<<endl;
    }
    return 0;
}
