//
//  main.c
//  oving3-xcode
//
//  Created by Pia Lindkjølen on 22.04.14.
//  Copyright (c) 2014 Pia Lindkjølen. All rights reserved.
//

#include <stdio.h>

void initializeBoard(char[][3]);
void printBoard(char[][3]);
int hasWon();

//Here comes the ulgiest thing in the world:
int main(int argc, const char * argv[])
{
    //Interaction with the user (From the driver) Use the FILE pointer?
    
    //initializeBoard();
    //hasTurn is 1 when it is player 1, and 2 when it is player two
    int hasTurn = 1;
    int play = 1;
    int column, row, x, y;
    char board[3][3];
    
    initializeBoard(board);
    printBoard(board);
    
    while (play == 1) {
        while (hasTurn == 1) {
            printf("Player 1 - Choose column 0 - 2 :\n");
            scanf("%d", &column);
            printf("Player 1 - Choose row 0 - 2 :\n");
            scanf("%d", &row);
            
            //Check if the column and row is allowed
            //Should be in a method
            if((column > 2
                || column < 0
                || row > 2
                || row < 0)) {
                   printf("Column or row not valid - try again.\n");
               }
            
            if(board[column][row] == 'X'
               || board[column][row] == '0'){
                printf("Column or row not valid - try again.\n");
                break;
            }
            //Set the right field to "X"
            else
                board[column][row] = 'X';
            
            
            //Check if player 1 has won, if not - switch to next player. This is ugly, need fixing. And methods - glorious methods.
            
            //Check rows
            for (x = 0; x < 3; x++) {
                if ((board[x][0] == 'X') && (board[x][0] == board[x][1]) && (board[x][1] == board[x][2])) {
                    printf("%s", "Player 1 won the row \n");
                    play = 0;
                }
            }
            //Check columns
            for (y = 0; y < 3; y++) {
                if ((board[0][y] == 'X') && (board[0][y] == board[1][y]) && (board[1][y] == board[2][y])) {
                    printf("%s", "Player 1 won the column! \n");
                    play = 0;
                }
            }
            
            if((board[0][0] == 'X') && (board[0][0] == board[1][1]) && (board[1][1] == board[2][2])){
                printf("%s", "Player 1 won the diagonal! \n");
                play = 0;
            }
            
            if((board[0][2] == 'X') && (board[0][2] == board[1][1]) && (board[1][1] == board[2][0])){
                printf("%s", "Player 1 won! \n");
                play = 0;
            }
            
            hasTurn = 2;
            
        }
        
        //Player two does exactly the same as player one. Shit this is so ugly
        while (hasTurn == 2) {
            printf("Player 2 - Choose column 0 - 2 :\n");
            scanf("%d", &column);
            printf("Player 2 - Choose row 0 - 2 :\n");
            scanf("%d", &row);
            
            //Check if the column and row is allowed
            //Should be in a method
            if((column > 2
               || column < 0
               || row > 2
               || row < 0)) {
               printf("Column or row not valid - try again.\n");
            }
            
            //Check if the spot is already taken
            
            if(board[column][row] == 'X'
               || board[column][row] == '0'){
                printf("Column or row not valid - try again.\n");
                break;
            }
            //Set the right field to "0"
            else
                board[column][row] = '0';
            
            
            //Check if player 2 has won, if not - switch to next player. This is ugly, need fixing. And methods - glorious methods.
            
            
            //Check rows
            for (x = 0; x < 3; x++) {
                if ((board[x][0] == '0') && (board[x][0] == board[x][1]) && (board[x][1] == board[x][2])) {
                    printf("%s", "Player 2 won! \n");
                    play = 0;
                }
            }
            //Check columns
            for (y = 0; y < 3; y++) {
                if ((board[0][y] == '0') && (board[0][y] == board[1][y]) && (board[1][y] == board[2][y])) {
                    printf("%s", "Player 2 won! \n");
                    play = 0;
                }
            }
            
            if((board[0][0] == '0') && (board[0][0] == board[1][1]) && (board[1][1] == board[2][2])){
                printf("%s", "Player 2 won! \n");
                play = 0;
            }
            
            if((board[0][2] == '0') && (board[0][2] == board[1][1]) && (board[1][1] == board[2][0])){
                printf("%s", "Player 2 won! \n");
                play = 0;
            }
            
            hasTurn = 1;
            
        }
    }
    
     return 0;
}


/*int hasWon(int hasTurn){
    
};*/

 
void initializeBoard(char board[][3]){
    for (int x = 0; x < 3; x++) {
        for (int y = 0; y < 3; y++) {
            board[x][y] = '.';
        }
    }

};

void printBoard(char board[][3]){
    for (int i = 0; i < 3; i++) {
        for (int x = 0; x < 3; x++) {
            printf("%c", board[i][x]);
        }
        printf("\n");
    }};

