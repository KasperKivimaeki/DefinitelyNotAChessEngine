#include <strings.h>
#include "bitboard.hpp"
#include "util.hpp"

struct simple_chess_state {
    int plies;
    BB wpieces;
    BB bpieces;
    BB bishops;
    BB knights;
    BB kings;
    BB rooks;
    BB pawns;
};

typedef struct simple_chess_state cState;

class State {
public:
    State(const char *FEN, BitBoards *b_) {
        wpieces = 0;
        bpieces = 0;
        bishops = 0;
        knights = 0;
        kings   = 0;
        rooks   = 0;
        pawns   = 0;
        b = b_;

        int sq = 0;
        while(sq < 64) {
            BB i = 1ull << sq;
            int f = _FILE(sq);
            int r = _RANK(sq);
            char c = *FEN++;
            switch (c) {
                case 'q': bpieces |= i; bishops |= i; rooks |= i; ; break;
                case 'Q': wpieces |= i; bishops |= i; rooks |= i; ; break;
                case 'b': bpieces |= i; bishops |= i;             ; break;
                case 'B': wpieces |= i; bishops |= i;             ; break;
                case 'n': bpieces |= i; knights |= i;             ; break;
                case 'N': wpieces |= i; knights |= i;             ; break;
                case 'p': bpieces |= i; pawns   |= i;             ; break; 
                case 'P': wpieces |= i; pawns   |= i;             ; break;
                case 'r': bpieces |= i; rooks   |= i;             ; break;
                case 'R': wpieces |= i; rooks   |= i;             ; break;
                case 'k': bpieces |= i; kings   |= i;             ; break;
                case 'K': wpieces |= i; kings   |= i;             ; break;
                case '/': sq -= 1; break;
                default: sq += c - 49;
            }
            sq++;
        }
    }

    void PrintGame();
    int Run();
    int TotalMoves(int);
    void Change(const char*);

    State **next;
    int next_n = -1;
    bool isCheck();

    int *fromS;
    int *toS;

    int plies = 0;

    int colorOf(int sq);

    Piece getPiece(int sq);
private:
    bool move(BB src, BB dst);

    Piece getPiece(BB sq);

    BitBoards *b;

    BB wpieces = 0;
    BB bpieces = 0;
    BB bishops = 0;
    BB knights = 0;
    BB kings   = 0;
    BB rooks   = 0;
    BB pawns   = 0;
};
