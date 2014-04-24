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
    
    //Should be in a method
    for (x = 0; x < 3; x++) {
        
        for (y = 0; y < 3; y++) {
            board[x][y] = '.';
        }

    }
    //Should be a method printBoard()
    for (int i = 0; i < 3; i++) {
        for (int x = 0; x < 3; x++) {
            printf("%c", board[i][x]);
        }
        printf("\n");
    }
    
    while (hasTurn == 1) {
        printf("Player 1 - Choose column 0 - 2 :\n");
        scanf("%d", &column);
        printf("Player 1 - Choose row 0 - 2 :\n");
        scanf("%d", &row);
        
        //Check if the column and row is allowed
        //Should be in a method
        if(column > 2
           || column < 0
           || row > 2
           || row < 0) {
            printf("Column or row not valid - try again.\n");
        }
        //Set the right field to "X"
        board[column][row] = 'X';
        
        //Print the new board
        for (int i = 0; i < 3; i++) {
            for (int x = 0; x < 3; x++) {
                printf("%c", board[i][x]);
            }
            printf("\n");
        }
        //Check if player 1 has won, if not - switch to next player. This is ugly, need fixing
        //Check rows
        for (x = 0; x < 3; x++) {
            if (board[]) {
                <#statements#>
            }
        }
    
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

