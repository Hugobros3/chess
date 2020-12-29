#include <cstdbool>
#include "chess.h"

#include <cstdio>
#include <cstdlib>
#include <ctime>

#include <unistd.h>

#define ANSI_BLACK      "\33[38;5;0;48;5;255m"
#define ANSI_DARK_GRAY  "\33[38;5;255;48;5;238m"
#define ANSI_LIGHT_GRAY "\33[38;5;0;48;5;250m"
#define ANSI_WHITE      "\33[38;5;255;48;5;0m"
#define ANSI_WHITERED   "\33[38;5;9;48;5;0m"
#define ANSI_BLACKRED   "\33[38;5;0;48;5;9m"
#define ANSI_RESET      "\33[m"

extern "C" {
    void print_nbr(int i) {
        printf("%d\n", i);
    }

    void illegal_state(int i) {
        printf("Illegal state: %d\n", i);
        abort();
    }

    void debug_print_move(Move* move) {
        switch (move->tag) {
            case 0: {
                MovePiece mp = move->data.MovePiece;
                if (mp.from.column > 8)
                    abort();
                printf("Move from %d %d to %d %d\n", mp.from.column, mp.from.row, mp.to.column, mp.to.row);
                break;
            }
            case 1: {
                TakePiece tp = move->data.TakePiece;
                printf("Attack from %d %d to %d %d\n", tp.from.column, tp.from.row, tp.to.column, tp.to.row);
                break;
            }
            case 2: {
                Promote p = move->data.Promote;
                printf("Promote from %d %d to %d %d (%d)\n", p.from.column, p.from.row, p.to.column, p.to.row, p.to_what.tag);
                break;
            }
            default: {
                printf("??? move\n");
            }
        }
    }
}

bool compare(BoardSquare a, BoardSquare b) {
    return (a.tag == 0 && b.tag == 0) ||
    (a.tag == b.tag && a.data.Occupied.e0.tag == b.data.Occupied.e0.tag && a.data.Occupied.e1.tag == b.data.Occupied.e1.tag);
}

void print_board(Board& board, Board* old_board) {
    printf("\n ");
    for (int col = 0; col < 8; col++) {
        printf("%c", 'a' + col);
    }
    printf("\n");
    for (int row = 7; row >= 0; row--) {
        printf("%d",row+1);
        for (int col = 0; col < 8; col++) {
            auto square = board.squares.e[col].e[row];
            if (square.tag == 1) {
                auto color = square.data.Occupied.e0.tag;

                if (old_board != nullptr && !compare(old_board->squares.e[col].e[row], square)) {
                    if (color == 0) printf(ANSI_WHITERED);
                    else printf(ANSI_BLACKRED);
                } else {
                    if (color == 0) printf(ANSI_WHITE);
                    else printf(ANSI_BLACK);
                }

                auto piece = square.data.Occupied.e1.tag;
                char c;
                switch (piece) {
                    case 0:
                        c = 'P';
                        break;
                    case 1:
                        c = 'B';
                        break;
                    case 2:
                        c = 'N';
                        break;
                    case 3:
                        c = 'R';
                        break;
                    case 4:
                        c = 'Q';
                        break;
                    case 5:
                        c = 'K';
                        break;
                }
                printf("%c", c);
                //printf("%d", piece);
                printf(ANSI_RESET);
            } else {
                if ((col + row) % 2 == 0)
                    printf(ANSI_DARK_GRAY);
                else printf(ANSI_LIGHT_GRAY);
                printf(" ");
                printf(ANSI_RESET);
            }
            //if (col < 7) printf(" ");
        }
        printf("\n");
    }
}

void stress_test() {
    boogey:
    while (true) {
        GameState game = new_game();
        for (int turn = 0; turn < 16000; turn++) {
            int old_turn = game.turn;
            play_a_random_move(&game);
            if (old_turn == game.turn) {
                printf("No more moves possible (%d, t = %d)\n", is_player_in_check(&game), turn);
                goto boogey;
            }
        }
        printf("Game took more than 16k turns\n");
    }
}

void random_game() {
    GameState game = new_game();
    print_board(game.board, nullptr);
    for (int turn = 0; turn < 16000; turn++) {
        // sleep(1);
        if (game.color_to_play.tag == 0) {
            printf("Black to play\n");
        } else {
            printf("White to play\n");
        }

        int old_turn = game.turn;
        //auto played_move = play_a_random_move(&game);
        GameState old_state = game;
        play_a_dumb_move(&game);
        if (old_turn == game.turn || only_kings_remain(&game)) {
            printf("No more moves possible (%d, t = %d)\n", is_player_in_check(&game), turn);
            break;
        }
        print_board(game.board, &old_state.board);
        //debug_print_move(played_move.data.Some);
    }
}

int main(int argc, char **argv) {
    srand(time(NULL));

    random_game();
    return 0;
}

