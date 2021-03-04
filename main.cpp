#include <iostream>
#include <cctype>
#include "bitboard.hpp"
#include "state.hpp"

void test1(BitBoards *b);
void test2(BitBoards *b);
void test3(BitBoards *b);

int main(int argc, char *argv[]) {
    BitBoards *b = new BitBoards();

    int result;

    State *s = new State("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", b);
    result = s->Run();

    for(int depth = 1; depth <= 7; depth++) {
        int total_moves = 0;

        total_moves = s->TotalMoves(depth);

        printf("Depth %d => %d moves\n", depth, total_moves);
    }

    return 0;
}

void test1(BitBoards *b) {
    State *s = new State("R6R/3Q4/1Q4Q1/4Q3/2Q4Q/Q4Q2/pp1Q4/kBNN1KB1 w - - 0 1", b);
    s->PrintGame();
    int result = s->Run();
    std::cout << "Result: " << result << std::endl;
}

void test2(BitBoards *b) {
    State *s = new State("R6R/3Q4/1Q4Q1/4Q3/2Q4Q/Q4Q2/pp1Q4/kBNN1KB1 w - - 0 1", b);
    s->PrintGame();
    int result = s->Run();
    std::cout << "Result: " << result << std::endl;
    s->Change("4R3/2Q5/6R1/1K1B1N2/3B4/3N4/P6P/1k6 w - - 0 1");
    s->PrintGame();
    result = s->Run();
    std::cout << "Result: " << result << std::endl;
}

void test3(BitBoards *b) {
    State *s = new State("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", b);
    int result = s->Run();

    int max_moves = 1 << 12;
    int i = 0;
    while(i++ < max_moves && s->next_n > 0) {
        int it = (uint64)(random()) % s->next_n;
        int c = s->colorOf(s->fromS[it]);
        if (s->plies % 2 == 0) {
            printf("%d.%s%s%s", (1 + s->plies/2), notation(s->getPiece(s->fromS[it]), c), notation(s->fromS[it]), notation(s->toS[it]));
        } else {
            printf(" %s%s%s\n", notation(s->getPiece(s->fromS[it]), c), notation(s->fromS[it]), notation(s->toS[it]));
        }
        s = s->next[it];
        
        
        //s->PrintGame();
        result = s->Run();
        //std::cout << "Result: " << result << std::endl;
    }
    printf("\n");
}
