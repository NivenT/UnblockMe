#include <iostream>
#include <stack>

#include "solver.h"

using namespace std;

int main() {
    Solver solver;
    Puzzle puzzle = Puzzle::from_file("puzzles/files/puzzle5.puz");

    cout<<puzzle<<endl;

    Path* solution = solver.solve_puzzle(puzzle);
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
