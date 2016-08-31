#include <iostream>

#include "solver.h"

using namespace std;

int main() {
    Puzzle puzzle = Puzzle::from_file("puzzles/files/puzzle1.puz");

    auto moves = puzzle.get_valid_moves();
    cout<<puzzle<<endl
        <<"There are "<<moves.size()<<" possible moves"<<endl;
    for (size_t i = 0; i < moves.size(); i++) {
        cout<<"Move "<<i<<":"<<endl
            <<puzzle.make_move(moves[i])<<endl
            <<"Direction: "<<moves[i].direction<<endl;
        cin.get();
    }
    return 0;
}
