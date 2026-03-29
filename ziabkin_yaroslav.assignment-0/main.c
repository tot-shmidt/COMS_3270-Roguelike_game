// Knight's Tour by Yaroslav Ziabkin. COMS 3270 assignment 0.
#include <stdio.h>

#define M 5
#define N 5

int board[M][N];

void populate_board() {
    int i, j;

    for (i = 0; i < M; i++) {
        for (j = 0; j < N; j++) {
            board[i][j] = 5 * i + j + 1;  
        }
    }
}

/*
void print_board() {
    int i, j;

    for (i = 0; i < M; i++) {
        for (j = 0; j < N; j++) {
            printf("%-2d ", board[i][j]);
        }
        printf("\n");
    }
}
*/

void initialize_visited(int visitedSquares[M][N]) {
    int i;
    int j;

    for (i = 0; i < M; i++) {
        for (j = 0; j < N; j++) {
            visitedSquares[i][j] = 0;
        }
    }
}

/*
void print_visited( int visitedSquares[M][N]) {
    int i;
    int j;

    for (i = 0; i < M; i++) {
        for (j = 0; j < N; j++) {
            printf("%d ",visitedSquares[i][j]);
        }
        printf("\n");
    }
}
*/

void find_tour(int squareRow, int squareColumn, int visitedSquares[M][N], int currentPath[], int moveCounter) {
    visitedSquares[squareRow][squareColumn] = 1;

    currentPath[moveCounter] = board[squareRow][squareColumn];

    if (moveCounter == 24) {
        int i;
        for (i = 0; i < 24; i++) {
            printf("%d,", currentPath[i]);
        }
        printf("%d\n", currentPath[24]);
    } else {
        if (squareRow - 2 >= 0 && squareColumn - 1 >= 0 && visitedSquares[squareRow - 2][squareColumn - 1] != 1) {
            find_tour(squareRow - 2, squareColumn - 1, visitedSquares, currentPath, moveCounter + 1);
        }
        if(squareRow - 2 >= 0 && squareColumn + 1 < N && visitedSquares[squareRow - 2][squareColumn + 1] != 1) {
            find_tour(squareRow - 2, squareColumn + 1, visitedSquares, currentPath, moveCounter + 1);
        }
        if (squareRow - 1 >= 0 && squareColumn + 2 < N && visitedSquares[squareRow - 1][squareColumn + 2] != 1) {
            find_tour(squareRow - 1, squareColumn + 2, visitedSquares, currentPath, moveCounter + 1);
        }
        if (squareRow + 1 < M && squareColumn + 2 < N && visitedSquares[squareRow + 1][squareColumn + 2] != 1) {
            find_tour(squareRow + 1, squareColumn + 2, visitedSquares, currentPath, moveCounter + 1);
        }
        if (squareRow + 2 < M && squareColumn + 1 < N && visitedSquares[squareRow + 2][squareColumn + 1] != 1) {
            find_tour(squareRow + 2, squareColumn + 1, visitedSquares, currentPath, moveCounter + 1);
        }
        if (squareRow + 2 < M && squareColumn - 1 >= 0 && visitedSquares[squareRow + 2][squareColumn - 1] != 1) {
            find_tour(squareRow + 2, squareColumn - 1, visitedSquares, currentPath, moveCounter + 1);
        }
        if (squareRow + 1 < M && squareColumn - 2 >= 0 && visitedSquares[squareRow + 1][squareColumn - 2] != 1) {
            find_tour(squareRow + 1, squareColumn - 2, visitedSquares, currentPath, moveCounter + 1);
        }
        if (squareRow - 1 >= 0 && squareColumn - 2 >= 0 && visitedSquares[squareRow - 1][squareColumn - 2] != 1) {
            find_tour(squareRow - 1, squareColumn - 2, visitedSquares, currentPath, moveCounter + 1);
        }
    }

    visitedSquares[squareRow][squareColumn] = 0;
}

int main(int argc, char* argv[]) {
    populate_board();
    // print_board();

    int startSquare = 1;
    int squareRow;
    int squareColumn;
    int moveCounter = 0;
    int currentPath[25];

    int visitedSquares[M][N]; // 0 == unvisited, 1 == visited
    initialize_visited(visitedSquares);
    // print_visited(visitedSquares);

    while (startSquare < 26) {
        squareRow = (startSquare - 1) / M;
        squareColumn = (startSquare - 1) % N;

        find_tour(squareRow, squareColumn, visitedSquares, currentPath, moveCounter);

        startSquare++;
    }

    return 0;
}
