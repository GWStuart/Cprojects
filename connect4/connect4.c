#include <stdio.h>

#define ROWS 6
#define COLUMNS 7


void printBoard(int board[ROWS][COLUMNS]) {
    for (int i=0; i<ROWS; i++) {
        printf("         ");
        for (int j=0; j<COLUMNS; j++) {
            int token = board[i][j];
            switch (token) {
                case 0: printf("-"); break;
                case 1: printf("X"); break;
                case -1: printf("O"); break;
            }
        }
        printf("\n");
    }
}

void printTurn(int turn) {
    if (turn == 1) {
        printf("It is X's turn\n");
    } else {
        printf("It is O's turn\n");
    }
}

int validColumn(int board[ROWS][COLUMNS], int column) {
    if (column < 0 || column >= COLUMNS) {
        return 0;
    }
    return board[0][column] == 0 ? 1 : 0;
}

// ensures that the given column is valid
int getColumn(int board[ROWS][COLUMNS]) {
    int column;
    int valid = 0;
    while (!valid) {
        printf("Enter a column: ");
        scanf("%d", &column);
        column --;
        if (validColumn(board, column)) {
            valid = 1;
        } else {
            printf("Invalid column. Try again\n");
        }
    }
    return column;
}

void placeToken(int board[ROWS][COLUMNS], int column, int turn) {
    for (int i=ROWS-1; i<ROWS; i--) {
        if (board[i][column] == 0) {
            board[i][column] = turn;
            return;
        }
    }
}

// returns token of winning player
// 0 if none
int getWinner(int board[ROWS][COLUMNS]) {
    return 0;
}

void printWinner(int winner) {
    if (winner == 1) {
        printf("X WON!\n");
    } else {
        printf("O WON!\n");
    }
}

int main() {
    // print welcome
    printf("\n~~~~~~~ Connect 4 ~~~~~~~\n\n");

    int board[ROWS][COLUMNS] = {0};
    int turn = 1;

    for (;;) {
        printBoard(board);
        printTurn(turn);
        int column = getColumn(board);
        placeToken(board, column, turn);

        int winner = getWinner(board);
        if (winner) {
            printWinner(winner);
            return 0;
        }

        turn *= -1;  // swap  the turn
    }
}
