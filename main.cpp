#include <iostream>
#include <cctype>
#include "bitboard.hpp"
#include "state.hpp"

int main(int argc, char *argv[]) {
    BitBoards *b = new BitBoards();

    int result;
    State *s = new State("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", b);
    s->PrintGame();

    result = s->Run();
    s->PrintGame();
    std::cout << "Result: " << result << std::endl;

    int max_moves = 1 << 12;
    int i = 0;
    while(i++ < max_moves && s->next_n > 0) {
        int it = (uint64)(random()) % s->next_n;
        s = s->next[it];
        s->PrintGame();
        result = s->Run();
        std::cout << "Result: " << result << std::endl;
        BB c;
        if (c = s->isCheck()) {
            break;
        }
    }

    /*
    State *s = new State("R6R/3Q4/1Q4Q1/4Q3/2Q4Q/Q4Q2/pp1Q4/kBNN1KB1 w - - 0 1", b);
    s->PrintGame();
    result = s->Run();
    std::cout << "Result: " << result << std::endl;
    */

    /*
    s->Change("4R3/2Q5/6R1/1K1B1N2/3B4/3N4/P6P/1k6 w - - 0 1");
    s->PrintGame();
    result = s->Run();
    std::cout << "Result: " << result << std::endl;
    */

    return 0;
}
