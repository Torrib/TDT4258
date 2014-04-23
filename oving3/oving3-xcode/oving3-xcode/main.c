//
//  main.c
//  oving3-xcode
//
//  Created by Pia Lindkjølen on 22.04.14.
//  Copyright (c) 2014 Pia Lindkjølen. All rights reserved.
//

#include <stdio.h>

//char* initializeBoard();
//void printBoard();

int main(int argc, const char * argv[])
{
    //Interaction with the user (From the driver) Use the FILE pointer?
    
    //initializeBoard();
    //hasTurn is 1 when it is player 1, and 2 when it is player two
    int hasTurn = 1;
    int column, row, x, y;
    char board[3][3];
    
    for (x = 0; x < 3; x++) {
        for (y = 0; y < 3; y++) {
            //For some reason the values on the board is "J" instead of "." as they should be. And if you add that the middle one should be "P" The values are "R" and the middle one "T"
            board[x][y] = ".";
            board[1][1] = "P";
        }
    }
    
    for (int i = 0; i < 3; i++) {
        for (int x = 0; x < 3; x++) {
            printf("%c", board[i][x]);
            
        }
        printf("\n");
    }
    
    while (hasTurn != 0) {
        printf("Player 1 - Choose column 0 - 2 :");
        scanf("%d", &column);
        printf("Player 1 - Choose row 0 - 2 :");
        scanf("%d", &row);
        
    }
    return 0;
}
/*
char* initializeBoard(){
    char board[3][3];
    char (*boardPointer)[3];
    boardPointer = board;
    for (int i = 0; i < 2; i++) {
        for (int x = 0; x < 2; x++) {
            board[i][x] = "i";
        }
    }
    return boardPointer[0];
};

void printBoard(){
    for (int i = 0; i < 3; i++) {
        for (int x = 0; x < 3; x++) {
        //    printf( "[%d][%d]=%d", i, x, board[i][x] );
        }
    }
};*/

