//
//  main.cpp
//  Chess Solver
//
//  Created by Caleb McFarland on 4/24/20.
//  Copyright © 2020 Caleb McFarland. All rights reserved.
//

#include <iostream>
#include <typeinfo>
#include <string>
#include "Header.h"
using namespace std;

int main(int argc, const char * argv[]) {
    Chess game;
    BoardState* currentBoard = game.startingPosition;
    BoardState* tempBoard = nullptr;
    
    int sizeTesting = 4;
    ComputerPlayer opponent(true);
    opponent.createEvaluateDestroyGT(sizeTesting);
    
    cout << "Computer's rating of starting position: " << opponent.currentPosition->position->evaluation << "\n";
    /*
    ComputerPlayer testOpponent(false);
    testOpponent.createEvaluateDestroyGT(4);
    
    cout << "testOpponent's rating of starting position: " << testOpponent.currentPosition->position->evaluation << "\n";
    */
    
    cout << game.toString(game.startingPosition) << "\n";
    
    int playerMove;
    int opponentMove;
    string input;
    
    cout << "The origin is in the top left and the coordinates grow from there.\n";
    
    
    while (currentBoard->winner == 0 && not game.isTie(currentBoard)){
        
        if (currentBoard->turn == opponent.playingAsWhite){
            opponentMove = opponent.bestMove();
            cout << game.toString(currentBoard, opponentMove);
            tempBoard = currentBoard;
            currentBoard = game.moveOutcome(currentBoard, opponentMove);
            delete tempBoard;
            cout << game.toString(currentBoard) << "\n";
            
            opponent.updateCurrentPosition(opponentMove, 0);
            //don't need to update before the player plays
            
            cout << "computer's evaluation: " << opponent.currentPosition->position->evaluation << "\n";
            
            //testOpponent.updateCurrentPosition(opponentMove, sizeTesting);
        }
        else{
            /*
            opponentMove = testOpponent.bestMove();
            cout << game.toString(currentBoard, opponentMove);
            tempBoard = currentBoard;
            currentBoard = game.moveOutcome(currentBoard, opponentMove);
            delete tempBoard;
            cout << game.toString(currentBoard) << "\n";
            
            testOpponent.updateCurrentPosition(opponentMove, 0);
            //don't need to update before the player plays
            
            cout << "testOpponent's evaluation: " << testOpponent.currentPosition->position->evaluation << "\n";
            if (currentBoard->winner == 0){
                cout << "winner: 0\n";
            }
            else if (currentBoard->winner == 1){
                cout << "winner: 1\n";
            }
            else if (currentBoard->winner == -1){
                cout << "winner: -1\n";
            }
            else{
                cout << "error\n";
            }
            
            opponent.updateCurrentPosition(opponentMove, sizeTesting);
             */
            
            cout << "What is the x and y of the piece you want to move written as x,y?\n";
            cin >> input;
            
            playerMove = 64 * (8*(int(input[2])-48) + int(input[0])-48);
            
            cout << "What is the coordinates of the place you want the piece to go as x,y?\n";
            cin >> input;
            
            playerMove += 8*(int(input[2])-48) + int(input[0])-48;
            
            if (not game.validMove(currentBoard, playerMove)){
                cout << "Illegal move\n";
                //cout << "You tried to move the piece: " << game.toLetter(currentBoard->piecePositions[(playerMove / 64) % 8][(playerMove / 64) / 8]) << "\n";
                return 0;
            }
            
            cout << game.toString(currentBoard, playerMove);
            tempBoard = currentBoard;
            currentBoard = game.moveOutcome(currentBoard, playerMove);
            delete tempBoard;
            cout << game.toString(currentBoard) << "\n";
            
            opponent.updateCurrentPosition(playerMove, sizeTesting);
             
        }
        
        
    }
    
    
    delete currentBoard;
    return 0;
}
