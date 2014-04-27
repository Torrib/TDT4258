//  main.c
//  oving3-xcode
//
//  Created by Pia Lindkjølen on 22.04.14.
//  Copyright (c) 2014 Pia Lindkjølen. All rights reserved.
//

#include "tictactoe.h"
#include <stdio.h>

// Prototypes
void initializeBoard(char[][3]);
void printBoard(char[][3]);
int hasWon(char[][3], int hasTurn);
int moveAllowed(char[][3], int *column,int *row);

/*
 * The main init code for tictactoe.
 *
 */
int init_tictactoe()
{

	printf("Game initializing");
    //Interaction with the user (From the driver) Use the FILE pointer?

    //hasTurn is 1 when it is player 1, and 2 when it is player two
    int hasTurn = 1;
    int play = 1;
    int column, row;
    char board[3][3];

    initializeBoard(board);
    while (play == 1) {
        while (hasTurn == 1) {
            printBoard(board);
            printf("Player 1 - Choose column 0 - 2 :\n");
            scanf("%d", &column);
            printf("Player 1 - Choose row 0 - 2 :\n");
            scanf("%d", &row);

            //Check if the column and row is allowed
            //Should be in a method

            if(moveAllowed(board, &column, &row) == 1)
                board[column][row] = 'X';
            else
                printf("Column or row not valid - try again.\n");

            //Check if player 1 won
            if (hasWon(board,hasTurn) == 1){
                printf("%s", "Player 1 won!");
                play = 0;
            }
            else
                hasTurn = 2;
        }

        //Do the same for player two - is there a way to do this without the loops?

        while (hasTurn == 2) {
            printBoard(board);
            printf("Player 2 - Choose column 0 - 2 :\n");
            scanf("%d", &column);
            printf("Player 2 - Choose row 0 - 2 :\n");
            scanf("%d", &row);

            //Check if the column and row is allowed
            if(moveAllowed(board, &column, &row) == 1)
                board[column][row] = '0';
            else
                printf("Column or row not valid - try again.\n");

            //Check if player 2 has won
            if (hasWon(board,hasTurn) == 1){
                printf("%s", "Player 1 won!");
                play = 0;
            }
            else
                hasTurn = 1;

        }
    }

     return 0;
}

int moveAllowed(char board[][3], int *column,int *row){
    if((*column > 2
        || *column < 0
        || *row > 2
        || *row < 0)) {

        return 0;
    }

    if(board[*column][*row] == 'X'
       || board[*column][*row] == '0'){
        return 0;
    }

    return 1;
}

int hasWon(char board[][3], int hasTurn){
    //Check rows
    for (int x = 0; x < 3; x++) {
        if ((board[x][0] == 'X') && (board[x][0] == board[x][1]) && (board[x][1] == board[x][2])) {
            printf("%s", "Player 1 won the row \n");
            return 1;
        }
    }
    //Check columns
    for (int y = 0; y < 3; y++) {
        if ((board[0][y] == 'X') && (board[0][y] == board[1][y]) && (board[1][y] == board[2][y])) {
            printf("%s", "Player 1 won the column! \n");
            return 1;
        }
    }

    if((board[0][0] == 'X') && (board[0][0] == board[1][1]) && (board[1][1] == board[2][2])){
        printf("%s", "Player 1 won the diagonal! \n");
        return 1;
    }

    if((board[0][2] == 'X') && (board[0][2] == board[1][1]) && (board[1][1] == board[2][0])){
        printf("%s", "Player 1 won! \n");
        return 1;
    }
    else
        return 0;
}

void initializeBoard(char board[][3]){
    for (int x = 0; x < 3; x++) {
        for (int y = 0; y < 3; y++) {
            board[x][y] = '.';
        }
    }
}

void printBoard(char board[][3]){
    //Use this to add things in the buffer?
    for (int i = 0; i < 3; i++) {
        for (int x = 0; x < 3; x++) {
            printf("%c", board[i][x]);
        }
        printf("\n");
    }
}

void tictactoe_event(uint8_t event)
{

    printf(event);
}
