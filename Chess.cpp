//
//  Chess.cpp
//  Chess Solver
//
//  Created by Caleb McFarland on 4/24/20.
//  Copyright © 2020 Caleb McFarland. All rights reserved.
//

#include <stdio.h>
#include <iostream>
#include <cstdlib>
#include "Header.h"
using namespace std;

BoardState::BoardState(){
    previousMoves = new LL;
}

BoardState::~BoardState(){
    delete previousMoves;
}

Chess::Chess(){
    
    startingPosition = new BoardState;
    startingPosition->turn = true;
    startingPosition->evaluation = 0.5;
    startingPosition->winner = 0;
    
    //white at top, black at bottom
    int i;
    for(int j=0;j < 8;j++){
        for(i=0;i < 8;i++){
            if (j == 0){
                startingPosition->piecePositions[0][j] = 4;
                startingPosition->piecePositions[1][j] = 2;
                startingPosition->piecePositions[2][j] = 3;
                startingPosition->piecePositions[3][j] = 6;
                startingPosition->piecePositions[4][j] = 5;
                startingPosition->piecePositions[5][j] = 3;
                startingPosition->piecePositions[6][j] = 2;
                startingPosition->piecePositions[7][j] = 4;
                i = 8;
            }
            else if (j == 1){
                startingPosition->piecePositions[i][j] = 1;
                //row of pawns
            }
            else if (j == 6){
                startingPosition->piecePositions[i][j] = -1;
            }
            else if (j == 7){
                startingPosition->piecePositions[i][j] = -1 * startingPosition->piecePositions[i][0];
                //black bottom row the negative of the white;
            }
            else{
                startingPosition->piecePositions[i][j] = 0;
            }
        }
    }
}

BoardState* Chess::moveOutcome(BoardState *previousPosition, int m, bool checkIfValid, bool setGameOver){
    if (previousPosition == nullptr){
        cout << "ERROR: Chess: moveOutcome: invalid previousPosition\n";
        return nullptr;
    }
    if (checkIfValid){
        if (not validMove(previousPosition, m)){
            cout << "ERROR: Chess: moveOutcome: invalid move\n";
            return nullptr;
        }
    }
    
    BoardState* outcome = new BoardState;
    outcome->evaluation = 0.5;
    outcome->turn = not previousPosition->turn;
    
    //copies the previous moves onto the outcome's list
    LLNode* temp = previousPosition->previousMoves->head;
    while (temp != nullptr){
        outcome->previousMoves->addNode(temp->move);
        temp = temp->nextNodePtr;
    }
    delete temp;
    
    outcome->previousMoves->addNode(m);
    
    int movingPieceX = (m / 64) % 8;
    int movingPieceY = (m / 64) / 8;
    int endSpotX = (m % 64) % 8;
    int endSpotY = (m % 64) / 8;
    int movingPieceValue = previousPosition->piecePositions[movingPieceX][movingPieceY];
    
    //moves the piece
    for (int i=0;i<8;i++){
        for (int j=0;j<8;j++){
            if (i == movingPieceX && j == movingPieceY){
                //spot where piece was
                outcome->piecePositions[i][j] = 0;
            }
            else if (i == endSpotX && j == endSpotY){
                //spot where piece ends up
                outcome->piecePositions[i][j] = movingPieceValue;
            }
            else{
                outcome->piecePositions[i][j] = previousPosition->piecePositions[i][j];
            }
        }
    }
    
    //special cases
    if (abs(movingPieceValue) == 1){
        //if pawn
        if (endSpotY == 0 || endSpotY == 7){
            //if queening
            //sets place where pawn ended up to a queen
            outcome->piecePositions[endSpotX][endSpotY] = 5 * (previousPosition->turn ? 1 : -1);
        }
        else if ((endSpotX == movingPieceX + 1 || endSpotX == movingPieceX - 1) && previousPosition->piecePositions[endSpotX][endSpotY] == 0){
            //if attacking and there is no piece there (must be en passant)
            //only need to get rid of where enemy pawn was
            outcome->piecePositions[endSpotX][endSpotY - (previousPosition->turn ? 1 : -1)] = 0;
            //endSpotY - direction set to 0 (where pawn was)
        }
    }
    else if (abs(movingPieceValue) == 6){
        //if king
        if (endSpotX == movingPieceX - 2 || endSpotX == movingPieceX + 2){
            //if castling
            int castlingDirection = endSpotX > movingPieceX ? 1 : -1;
            
            outcome->piecePositions[castlingDirection > 0 ? 7 : 0][endSpotY] = 0;
            //sets where rook was to 0
            outcome->piecePositions[endSpotX - castlingDirection][endSpotY] = 4 * (previousPosition->turn ? 1 : -1);
            //sets where rook ended up to rook value
        }

    }
    
    if (setGameOver){
        outcome->winner = checkGameOver(outcome);
    }
    else{
        outcome->winner = previousPosition->winner;
    }
    
    return outcome;
}

bool Chess::validMoveWithoutCheckForCheck(BoardState *position, int m, bool checkForTurn){
    //works except doesn't check for to make sure mover isn't in check in the end
    //made in two different functions so return can be used to skip things
    if (position == nullptr){
        cout << "ERROR: Chess: validMove: invalid position\n";
        return false;
    }
    if (m < 0 || m > 4095){
        cout << "ERROR: Chess: validMove: move out of range. move: " << m << "\n";
        return false;
    }
    
    int movingPieceX = (m / 64) % 8;
    int movingPieceY = (m / 64) / 8;
    int endSpotX = (m % 64) % 8;
    int endSpotY = (m % 64) / 8;
    int movingPieceValue = position->piecePositions[movingPieceX][movingPieceY];
    
    if (movingPieceValue == 0){
        cout << "ERROR: Chess: validMove: no piece at " << movingPieceX << ", " << movingPieceY << "\n";
        return false;
    }
    if (movingPieceValue > 0 != position->turn && checkForTurn){
        cout << "ERROR: Chess: validMove: can't move opponent's piece\n";
        if (position->turn) cout << "White's turn\n";
        else cout << "Black's turn\n";
        return false;
    }
    if (movingPieceX == endSpotX && movingPieceY == endSpotY){
        //can't move to the same position
        return false;
    }
    
    switch (abs(movingPieceValue)) {
        case 1: {
            //if pawn
            int movingDirection = position->turn ? 1 : -1;
            if (endSpotX == movingPieceX){
                //if not capturing
                if (position->piecePositions[movingPieceX][movingPieceY + movingDirection] != 0){
                    //can't move forward if there is a piece in front
                    return false;
                }
                if (endSpotY == movingPieceY + movingDirection){
                    //pawn should never be on the last row
                    //if only need to move one forward
                    return true;
                }
                if (endSpotY == movingPieceY + 2*movingDirection){
                    //if trying to move two spaces
                    int playerSecondRow = position->turn ? 1 : 6;
                    if (movingPieceY != playerSecondRow || position->piecePositions[endSpotX][endSpotY] != 0){
                        //can't move if not first pawn move or if something is in the way
                        return false;
                    }
                    else{
                        //only other posibility
                        return true;
                    }
                }
            }
            if ((endSpotX == movingPieceX + 1 || endSpotX == movingPieceX - 1) && endSpotY == movingPieceY + movingDirection){
                //if capturing
                if (position->piecePositions[endSpotX][endSpotY] != 0 && position->piecePositions[endSpotX][endSpotY] > 0 != position->turn){
                    //if a piece to attack
                    return true;
                }
                int enPassantRow = position->turn ? 4 : 3;
                if (movingPieceY == enPassantRow && abs(int(position->piecePositions[endSpotX][endSpotY - movingDirection])) == 1){
                    //if could possibly do an en passant
                    int previousEnPassantMove = 8 * (enPassantRow + 2*movingDirection) + endSpotX;
                    previousEnPassantMove *= 64;
                    previousEnPassantMove += 8 * enPassantRow + endSpotX;
                    
                    if (position->previousMoves->tail->move == previousEnPassantMove){
                        //if the last move was the captured pawn moving up twice
                        return true;
                    }
                }
                //if it made it through the checks then it can't capture
                return false;
            }
            //if its trying to move somewhere farther away
            return false;
            break;
        }
            
        case 2: {
            //if Knight
            int movedX = endSpotX - movingPieceX;
            int movedY = endSpotY - movingPieceY;
            if ((abs(movedX) == 1 || abs(movedX) == 2) && (abs(movedY) == 1 || abs(movedY) == 2)){
                //if moved like a knight
                if (position->piecePositions[endSpotX][endSpotY] == 0){
                    //if empty spot to move to
                    return true;
                }
                else if (position->piecePositions[endSpotX][endSpotY] > 0 != position->turn){
                    //if piece to capture
                    return true;
                }
                else{
                    return false;
                }
            }
            else{
                return false;
            }
            break;
        }
            
        case 3: {
            //if bishop
            int movedX = endSpotX - movingPieceX;
            int movedY = endSpotY - movingPieceY;
            int directionX = movedX > 0 ? 1 : -1;
            int directionY = movedY > 0 ? 1 : -1;
            if (abs(movedX) != abs(movedY)){
                //if not a diagnol
                return false;
            }
            for (int i=1;i<abs(movedX);i++){
                //checks every square on the way
                if (position->piecePositions[movingPieceX + directionX * i][movingPieceY + directionY * i] != 0){
                    return false;
                }
            }
            if (position->piecePositions[endSpotX][endSpotY] == 0 || position->piecePositions[endSpotX][endSpotY] > 0 != position->turn){
                //if empty spot to move to or piece to take
                return true;
            }
            else{
                return false;
            }
            
            break;
        }
            
        case 4: {
            //if rook
            int movedX = endSpotX - movingPieceX;
            int movedY = endSpotY - movingPieceY;
            if (movedX == 0){
                int direction = movedY > 0 ? 1 : -1;
                for (int i=1;i<abs(movedY);i++){
                    //check every square on the way
                    //i is the number of spaces away from the starting position being checked
                    if (position->piecePositions[movingPieceX][movingPieceY + direction * i] != 0){
                        return false;
                    }
                }
                if (position->piecePositions[endSpotX][endSpotY] == 0 || position->piecePositions[endSpotX][endSpotY] > 0 != position->turn){
                    //if empty spot or piece to take
                    return true;
                }
                else{
                    return false;
                }
            }
            else if (movedY == 0){
                int direction = movedX > 0 ? 1 : -1;
                for (int i=1;i<abs(movedX);i++){
                    //check every square on the way
                    //i is the number of spaces away from the starting position being checked
                    if (position->piecePositions[movingPieceX + direction * i][movingPieceY] != 0){
                        return false;
                    }
                }
                if (position->piecePositions[endSpotX][endSpotY] == 0 || position->piecePositions[endSpotX][endSpotY] > 0 != position->turn){
                    //if empty spot or piece to take
                    return true;
                }
                else{
                    return false;
                }
            }
            else{
                return false;
            }
            break;
        }
            
        case 5: {
            //if queen
            int movedX = endSpotX - movingPieceX;
            int movedY = endSpotY - movingPieceY;
            if (abs(movedX) == abs(movedY) || movedX == 0 || movedY == 0){
                //if moved like bishop or rook
                int directionX = movedX > 0 ? 1 : -1;
                int directionY = movedY > 0 ? 1 : -1;
                
                if (movedX == 0) directionX = 0;
                if (movedY == 0) directionY = 0;
                //used to make rook and bishop part work the same (will be 0 if rook move)
                
                for (int i=1;i<max(abs(movedX), abs(movedY));i++){
                    //check every square on the way
                    if (position->piecePositions[movingPieceX + directionX * i][movingPieceY + directionY * i] != 0){
                        return false;
                    }
                }
                if (position->piecePositions[endSpotX][endSpotY] == 0 || position->piecePositions[endSpotX][endSpotY] > 0 != position->turn){
                    //if empty spot or piece to take
                    return true;
                }
                else{
                    return false;
                }
            }
            else{
                return false;
            }
            break;
        }
            
        case 6: {
            //if king
            int movedX = endSpotX - movingPieceX;
            int movedY = endSpotY - movingPieceY;
            if (abs(movedX) > 1 || abs(movedY) > 1){
                if (abs(movedX) == 2 && movedY == 0){
                    //if castle
                    int backrow = position->turn ? 0 : 7;
                    int direction = movedX > 0 ? 1 : -1;
                    int rookStartingPosition = 8*backrow + (direction > 0 ? 7 : 0);
                    int kingStartingPosition = 8*backrow + 3;
                    int moveStartingPosition;
                    
                    LLNode* temp = position->previousMoves->head;
                    while (temp != nullptr){
                        moveStartingPosition = temp->move / 64;
                        if (moveStartingPosition == rookStartingPosition || moveStartingPosition == kingStartingPosition){
                            //if the rook or king has moved yet
                            return false;
                        }
                        temp = temp->nextNodePtr;
                    }
                    
                    for (int i=1;i < (direction > 0 ? 4 : 3);i++){
                        //checks if squares between rook and king are empty
                        if (position->piecePositions[movingPieceX + direction * i][movingPieceY] != 0){
                            return false;
                        }
                    }
                    
                    for (int i=0;i < (direction > 0 ? 3 : 2);i++){
                        //checks if king will be in check in any spots
                        //has to start at 0 to be sure that king is not already in check (no castling to get out of check)
                        //end spot is later checked just like all moves so doesn't have to be checked here
                        BoardState* testBoard = new BoardState;
                        //creates test board that has the king moved to the spot that needs to be tested
                        testBoard->turn = not position->turn;
                        testBoard->evaluation = 0.5;
                        testBoard->winner = 0;
                        for (int j=0;j<8;j++){
                            for (int k=0;k<8;k++){
                                if (j == movingPieceX && k == movingPieceY){
                                    //where king used to be
                                    testBoard->piecePositions[j][k] = 0;
                                }
                                if (j == movingPieceX + direction * i && k == movingPieceY){
                                    //king new spot
                                    testBoard->piecePositions[j][k] = 6 * (position->turn ? 1 : -1);
                                }
                                else{
                                    testBoard->piecePositions[j][k] = position->piecePositions[j][k];
                                }
                            }
                        }
                        
                        if (inCheck(testBoard, position->turn)){
                            delete testBoard;
                            return false;
                        }
                        delete testBoard;
                    }
                    //if made it through all other checks
                    return true;
                }
                else{
                    //if move too far away
                    return false;
                }
            }
            else{
                if (position->piecePositions[endSpotX][endSpotY] == 0 || position->piecePositions[endSpotX][endSpotY] > 0 != position->turn){
                    return true;
                }
                else{
                    return false;
                }
            }
            break;
        }
            
        default:
            cout << "ERROR: Chess: validMove: invalid piece at " << movingPieceX << ", " << movingPieceY << ": " << movingPieceValue << "\n";
            return false;
            break;
    }
    
    cout << "ERROR: Chess: validMove: should never make it through whole function\n";
    cout << "Piece: " << movingPieceValue << "\n";
    cout << "Tested move: " << "(" + to_string(movingPieceX) + ", " + to_string(movingPieceY) + ") -> (" + to_string(endSpotX) + ", " + to_string(endSpotY) + ")" << "\n";
    return false;
}

bool Chess::validMove(BoardState *position, int m, bool checkForCheck, bool checkForTurn){
    bool outcome = validMoveWithoutCheckForCheck(position, m, checkForTurn);
    if (checkForCheck && outcome){
        //doesn't have to check if outcome is already false
        BoardState* testBoard = moveOutcome(position, m, false, false);
        //already checked if valid and winner doesn't matter
        testBoard->winner = 0;
        
        if (inCheck(testBoard, position->turn) && checkForCheck){
            //your move can't result in being in check
            //covers not getting out of check and moving into check
            
            //doesn't consider this when checkForCheck is false, useful to see if a piece can move to a spot
            //such as it is used in inCheck
            delete testBoard;
            return false;
        }
        
        delete testBoard;
    }
    return outcome;
}

LL* Chess::candidatePossibleMoves(BoardState *position){
    if (position == nullptr){
        cout << "ERROR: Chess: candidatePossibleMoves: invalid position\n";
        return nullptr;
    }
    LL* output = new LL;
    //will be deleted in function that uses this function
    
    for (int x=0;x<8;x++){
        for (int y=0;y<8;y++){
            if (position->piecePositions[x][y] != 0 && position->piecePositions[x][y] > 0 == position->turn){
                //checks for all the movers pieces
                int startingMovePosition = 64*(8*y + x);
                int movingPiece = abs(int(position->piecePositions[x][y]));
                //most are similar to in inCheck
            
                if (movingPiece == 1){
                    //if pawn
                    int pawnDirection = position->turn ? 1 : -1;
                    output->addNode(startingMovePosition + 8*(y + pawnDirection) + x);
                    //1 forward
                    output->addNode(startingMovePosition + 8*(y + pawnDirection) + x + 1);
                    //attack forward + right
                    output->addNode(startingMovePosition + 8*(y + pawnDirection) + x - 1);
                    //attack forward + left
                    if ((y == 1 && position->turn) || (y == 6 && !position->turn)){
                        output->addNode(startingMovePosition + 8*(y + 2*pawnDirection) + x);
                        //move 2 on first move
                    }
                }
                    
                else if (movingPiece == 2){
                    //if knight
                    int xdistance[8] = {-2, -1, 1, 2, 2, 1, -1, -2};
                    int ydistance[8] = {-1, -2, -2, -1, 1, 2, 2, 1};
                    //starting NW and moving clockwise
                    for (int k=0;k<8;k++){
                        if (x + xdistance[k] >= 0 && x + xdistance[k] < 8 && y + ydistance[k] >= 0 && y + ydistance[k] < 8){
                            output->addNode(startingMovePosition + 8*(y + ydistance[k]) + x + xdistance[k]);
                        }
                    }
                }
                
                else if (movingPiece == 6){
                    //if king
                    int xdistance[8] = {-1, 0, 1 , 1, 1, 0, -1, -1};
                    int ydistance[8] = {-1, -1, -1, 0, 1, 1, 1, 0};
                    //starting NW and moving clockwise
                    for (int k=0;k<8;k++){
                        if (x + xdistance[k] >= 0 && x + xdistance[k] < 8 && y + ydistance[k] >= 0 && y + ydistance[k] < 8){
                            output->addNode(startingMovePosition + 8*(y + ydistance[k]) + x + xdistance[k]);
                        }
                    }
                    
                    //check for castle
                    int backrow = position->turn ? 0 : 7;
                    if (y == backrow && x == 3){
                        //if king is in original spot
                        if (abs(int(position->piecePositions[0][backrow])) == 4){
                            //if kings side rook is in original spot
                            output->addNode(startingMovePosition + 8*y + x - 2);
                        }
                        if (abs(int(position->piecePositions[7][backrow])) == 4){
                            //if queens side rook is in original spot
                            output->addNode(startingMovePosition + 8*y + x + 2);
                        }
                    }
                }
                
                else if (movingPiece > 6){
                    cout << "ERROR: Chess: candidatePossibleMoves: invalid piece at " << x << ", " << y << ": " << position->piecePositions[x][y] << "\n";
                }
                //check bishop, rook, and queen last so others can use else statements
                if (movingPiece == 3 || movingPiece == 5){
                    //if bishop or queen
                    //NW
                    for (int k=1;k<=min(x, y);k++){
                        output->addNode(startingMovePosition + 8*(y - k) + x - k);
                    }
                    //NE
                    for (int k=1;k<=min(7 - x, y);k++){
                        output->addNode(startingMovePosition + 8*(y - k) + x + k);
                    }
                    //SE
                    for (int k=1;k<=min(7 - x, 7 - y);k++){
                        output->addNode(startingMovePosition + 8*(y + k) + x + k);
                    }
                    //SW
                    for (int k=1;k<=min(x, 7 - y);k++){
                        output->addNode(startingMovePosition + 8*(y + k) + x - k);
                    }
                }
                    
                if (movingPiece == 4 || movingPiece == 5){
                    //if rook or queen
                    //row
                    for (int k=0;k<8;k++){
                        if (k != x){
                            output->addNode(startingMovePosition + 8*y + k);
                        }
                    }
                    //column
                    for (int k=0;k<8;k++){
                        if (k != y){
                            output->addNode(startingMovePosition + 8*k + x);
                        }
                    }
                }
                
                    
                
            }
        }
    }
    
    return output;
}

bool Chess::samePiecePositions(BoardState *position1, BoardState *position2){
    for (int i=0;i<8;i++){
        for (int j=0;j<8;j++){
            if (position1->piecePositions[i][j] != position2->piecePositions[i][j]){
                return false;
            }
        }
    }
    //returns true if didn't return false already
    return true;
}

bool Chess::isTie(BoardState *position){
    if (position == nullptr){
        cout << "ERROR: Chess: isTie: invalid position\n";
        return false;
    }
    
    //check for sufficient material
    bool sufficientMaterial = false;
    for (int i=0;i<8;i++){
        for (int j=0;j<8;j++){
            int piece = abs(int(position->piecePositions[i][j]));
            if (piece == 1 || piece == 4 || piece == 5){
                sufficientMaterial = true;
                return false;
            }
        }
    }
    if (not sufficientMaterial){
        return true;
    }
    
    //check for 3 fold repition
    int timesRepeated = 0;
    //only checks if the current position has been repeated, so isTie needs to be called everytime
    
    LLNode* tempMove = position->previousMoves->head;
    BoardState* tempBoard = startingPosition;
    BoardState* tempBoard2 = nullptr;
    
    if (samePiecePositions(tempBoard, position)){
        timesRepeated++;
    }
    
    while (tempMove != nullptr){
        tempBoard2 = tempBoard;
        tempBoard = moveOutcome(tempBoard, tempMove->move);
        delete tempBoard2;
        
        if (samePiecePositions(tempBoard, position)){
            timesRepeated++;
        }
        
        tempMove = tempMove->nextNodePtr;
    }
    
    if (timesRepeated == 3){
        return true;
    }
    else if (timesRepeated > 3){
        cout << "ERROR: Chess: isTie: board has been repeated more than 3 times\n";
        return true;
    }
    
    //checks if stalemate
    bool canmove = false;
    LL* cpm = candidatePossibleMoves(position);
    LLNode* temp = cpm->head;
    while (temp != nullptr){
        if (validMove(position, temp->move)){
            canmove = true;
            temp = nullptr;
        }
    }
    delete cpm;
    
    if (not canmove){
        return true;
    }
    
    //if passed all test
    return false;
}

int Chess::checkGameOver(BoardState *position){
    if (position == nullptr){
        cout << "ERROR: Chess: checkGameOver: invalid position\n";
        return 0;
    }
    
    //new
    LL* cpm = candidatePossibleMoves(position);
    for (int i=1;i>-2;i-=2){
        //i starts at 1 and then goes to -1
        //when i is 1 checking if white is in check
        int kingX = 0;
        int kingY = 0;
        //find king's row and column
        for (int j=0;j<8;j++){
            for (int k=0;k<8;k++){
                if (abs(int(position->piecePositions[j][k])) == 6 && position->piecePositions[j][k] > 0 == i>0){
                    kingX = j;
                    kingY = k;
                    j = 8;
                    k = 8;
                    //should only be one king
                }
            }
        }
        
        int numAttackingKing = numPiecesAttacking(position, 8*kingY + kingX, (i<0));
        
        if (numAttackingKing == 0){
            //if not in check do nothing and let it check other player or exit
            //checked first because it is by far the most likely option
        }
        else if (numAttackingKing < 0){
            //only one piece attacking king (may be able to block or capture)
            bool canGetOutOfCheck = false;
            
            int attackingPiece = -1 * (numAttackingKing+1);
            int attackingPieceX = attackingPiece % 8;
            int attackingPieceY = attackingPiece / 8;
            
            int movingPiece;
            int endSpot;
            int endSpotX;
            int endSpotY;
            
            bool attackingPieceSameX = attackingPieceX == kingX;
            bool attackingPieceSameY = attackingPieceY == kingY;
            bool attackingPieceSamePD = (attackingPieceX - kingX) == (attackingPieceY - kingY);
            //if the attacking piece is on the \ positive slope under xy grid, negative normally, direction diaganol (top left to bottom right)
            bool attackingPieceSameND = (attackingPieceX - kingX) == (kingY - attackingPieceY);
            //on / slope, negative under coordinates, bottom left to top right
            
            bool couldBlock = (abs(attackingPieceX - kingX) > 1 || abs(attackingPieceY - kingY) > 1);
            //makes sure the attacking piece is not right next to the king
            
            LLNode* temp = cpm->head;
            while (temp != nullptr){
                movingPiece = temp->move / 64;
                endSpot = temp->move % 64;
                endSpotX = endSpot % 8;
                endSpotY = endSpot / 8;
                
                //can use else if because things that fall under both will be checked by a previous
                if (movingPiece / 8 == kingY && movingPiece % 8 == kingX){
                    //if moving king
                    if (validMove(position, temp->move)){
                        canGetOutOfCheck = true;
                    }
                }
                else if (endSpot == attackingPiece){
                    //if capturing
                    //still have to check if valid because cpm returns a lot of illegal options, and could move a pinned piece
                    if (validMove(position, temp->move)){
                        canGetOutOfCheck = true;
                    }
                }
                else if (couldBlock){
                    //checking for blocking
                    if (attackingPieceSameX){
                        //if could block from sides
                        if (endSpotX == kingX){
                            //if would move to king's row
                            if ((endSpotX > kingX && endSpotX < attackingPieceX) || (endSpotX < kingX && endSpotX > attackingPieceX)){
                                //if in between king and attacking piece
                                if (validMove(position, temp->move)){
                                    canGetOutOfCheck = true;
                                }
                            }
                        }
                    }
                    else if (attackingPieceSameY){
                        //if could block from top and bottom
                        if (endSpotY == kingY){
                            //if would move to king's column
                            if ((endSpotY > kingY && endSpotY < attackingPieceY) || (endSpotY < kingY && endSpotY > attackingPieceY)){
                                //if in between king and attacking piece
                                if (validMove(position, temp->move)){
                                    canGetOutOfCheck = true;
                                }
                            }
                        }
                        
                    }
                    else if (attackingPieceSamePD){
                        //if could block diaganol from one direction
                        if (endSpotX - kingX == endSpotY - kingY){
                            //if would move to correct diaganol
                            if ((endSpotX > kingX && endSpotX < attackingPieceX) || (endSpotX < kingX && endSpotX > attackingPieceX)){
                                //if in between king and attacking piece
                                //can just use same as blocking on row because already garunteed same diaganol
                                if (validMove(position, temp->move)){
                                    canGetOutOfCheck = true;
                                }
                            }
                        }
                    }
                    else if (attackingPieceSameND){
                        //if could block diaganol from other direction
                        if (endSpotX - kingX == kingY - endSpotY){
                            //if would move to correct diaganol
                            if ((endSpotX > kingX && endSpotX < attackingPieceX) || (endSpotX < kingX && endSpotX > attackingPieceX)){
                                //if in between king and attacking piece
                                //can just use same as blocking on row because already garunteed same diaganol
                                if (validMove(position, temp->move)){
                                    canGetOutOfCheck = true;
                                }
                            }
                        }
                    }
                    //knights would make couldBlock true, but don't need to be checked since they can't be blocked
                }
                
                //don't need to keep looking if already have a way out of check
                if (canGetOutOfCheck){
                    temp = nullptr;
                }
                else{
                    temp = temp->nextNodePtr;
                }
            }
            
            if (!canGetOutOfCheck){
                delete cpm;
                return i;
            }
        }
        else if (numAttackingKing > 1){
            //multiple pieces attacking king (have to get out of way)
            bool canGetOutOfCheck = false;
            int movingPiece;
            
            LLNode* temp = cpm->head;
            while (temp != nullptr){
                movingPiece = temp->move / 64;
                if (movingPiece / 8 == kingY && movingPiece % 8 == kingX){
                    //if moving king
                    if (validMove(position, temp->move)){
                        canGetOutOfCheck = true;
                    }
                }
                
                //don't need to keep looking if already have a way out of check
                if (canGetOutOfCheck){
                    temp = nullptr;
                }
                else{
                    temp = temp->nextNodePtr;
                }
            }
            
            if (!canGetOutOfCheck){
                delete cpm;
                return i;
            }
        }
    }
    
    //if found a way out of check or wasn't in check
    delete cpm;
    return 0;
    
    //old code
    /*
    bool canGetOutOfCheck;
    for (int i=1;i>-2;i-=2){
        //i starts at 1 and then goes to -1
        if (inCheck(position, i)){
            canGetOutOfCheck = false;
            
            //can defininetley do better than checking every possible move
            LL* cpm = candidatePossibleMoves(position);
            LLNode* temp = cpm->head;
            while (temp != nullptr){
                if (validMove(position, temp->move)){
                    canGetOutOfCheck = true;
                }
                temp = temp->nextNodePtr;
            }
            
            delete cpm;
            
            if (not canGetOutOfCheck){
                return i;
            }
        }
    }
    
    return 0;
     */
}

bool Chess::inCheck(BoardState *position, bool player){
    int kingY = 0;
    int kingX = 0;
    
    //find king's row and column
    for (int i=0;i<8;i++){
        for (int j=0;j<8;j++){
            if (abs(int(position->piecePositions[i][j])) == 6 && position->piecePositions[i][j] > 0 == player){
                kingY = j;
                kingX = i;
                i = 8;
                j = 8;
                //should only be one king
            }
        }
    }
    
    //checks for rooks and queens on +
    //checks king's row
    //going left
    for (int i=kingX-1;i>=0;i--){
        if (position->piecePositions[i][kingY] != 0){
            if (position->piecePositions[i][kingY] > 0 != player){
                
                int attackingPiece = abs(int(position->piecePositions[i][kingY]));
                if (attackingPiece == 4 || attackingPiece == 5){
                    return true;
                }
            }
            i = -1;
        }
    }
    //going right
    for (int i=kingX+1;i<8;i++){
        if (position->piecePositions[i][kingY] != 0){
            if (position->piecePositions[i][kingY] > 0 != player){
                
                int attackingPiece = abs(int(position->piecePositions[i][kingY]));
                if (attackingPiece == 4 || attackingPiece == 5){
                    return true;
                }
            }
            i = 8;
        }
    }
    
    //checks king's column
    //going up
    for (int i=kingY-1;i>=0;i--){
        if (position->piecePositions[kingX][i] != 0){
            if (position->piecePositions[kingX][i] > 0 != player){
                
                int attackingPiece = abs(int(position->piecePositions[kingX][i]));
                if (attackingPiece == 4 || attackingPiece == 5){
                    return true;
                }
            }
            i = -1;
        }
    }
    //going down
    for (int i=kingY+1;i<8;i++){
        if (position->piecePositions[kingX][i] != 0){
            if (position->piecePositions[kingX][i] > 0 != player){
                
                int attackingPiece = abs(int(position->piecePositions[kingX][i]));
                if (attackingPiece == 4 || attackingPiece == 5){
                    return true;
                }
            }
            i = 8;
        }
    }
    
    //checks for bishops, queens, and pawns on X
    //NW from king
    for (int i=1;i<=min(kingY, kingX);i++){
        //i is the distance in the direction it is going
        //i can't be more than the lowest distance or else it will go out of board
        //min(pieceY, pieceX) must be less than 8 so can use i=8 to exit
        int attackingPiece = position->piecePositions[kingX - i][kingY - i];
        if (attackingPiece != 0){
            if (attackingPiece > 0 != player){
                attackingPiece = abs(attackingPiece);
                if (attackingPiece == 3 || attackingPiece == 5){
                    return true;
                }
                else if (i == 1 && player && attackingPiece == 1){
                    //if white, only white pawns could attack from the NW
                    return true;
                }
            }
            i = 8;
        }
    }
    //NE from king
    for (int i=1;i<=min(kingY,7 - kingX);i++){
        //use 7 - x when going in positive direction
        int attackingPiece = position->piecePositions[kingX + i][kingY - i];
        if (attackingPiece != 0){
            if (attackingPiece > 0 != player){
                attackingPiece = abs(attackingPiece);
                if (attackingPiece == 3 || attackingPiece == 5){
                    return true;
                }
                else if (i == 1 && player && attackingPiece == 1){
                    //if white, only white pawns could attack from the NE
                    return true;
                }
            }
            i = 8;
        }
    }
    //SW from king
    for (int i=1;i<=min(7 - kingY, kingX);i++){
        //use 7 - x when going in positive direction
        int attackingPiece = position->piecePositions[kingX - i][kingY + i];
        if (attackingPiece != 0){
            if (attackingPiece > 0 != player){
                attackingPiece = abs(attackingPiece);
                if (attackingPiece == 3 || attackingPiece == 5){
                    return true;
                }
                else if (i == 1 && !player && attackingPiece == 1){
                    //if black, only black pawns could attack from the SW
                    return true;
                }
            }
            i = 8;
        }
    }
    //SE from king
    for (int i=1;i<=min(7 - kingY,7 - kingX);i++){
        //use 7 - x when going in positive direction
        int attackingPiece = position->piecePositions[kingX + i][kingY + i];
        if (attackingPiece != 0){
            if (attackingPiece > 0 != player){
                attackingPiece = abs(attackingPiece);
                if (attackingPiece == 3 || attackingPiece == 5){
                    return true;
                }
                else if (i == 1 && !player && attackingPiece == 1){
                    //if black, only black pawns could attack from the SE
                    return true;
                }
            }
            i = 8;
        }
    }
    
    //check knights
    int xdistance[8] = {-2, -1, 1, 2, 2, 1, -1, -2};
    int ydistance[8] = {-1, -2, -2, -1, 1, 2, 2, 1};
    //checks starting in NW and moves around clockwise
    for (int i=0;i<8;i++){
        //checks the 8 possible knight positions
        if ((kingX + xdistance[i] >= 0 && kingX + xdistance[i] < 8) && (kingY + ydistance[i] >= 0 && kingY + ydistance[i] < 8)){
            //if not out of range
            int attackingPiece = position->piecePositions[kingX + xdistance[i]][kingY + ydistance[i]];
            if (abs(attackingPiece) == 2 && attackingPiece > 0 != player && attackingPiece != 0){
                return true;
            }
        }
    }
    
    //if made it through everything return false
    return false;
}

signed int Chess::numPiecesAttacking(BoardState *position, int piecePosition, bool attackingPiecesPlayer){
    //works similar to inCheck, but works for any piece given the position and counts the number of attacking pieces instead of just returning a bool
    //if only one piece attacking then it returns the negative values corresponding to the location of the attacking piece as 1-64
    //checks going in each direction, stopping if it runs into a piece that could block, adding to the total if running into a piece that could attack
    int piecesAttacking = 0;
    signed int attackingPieceLocation = -65;
    //-65 is an invalid option, so if it is returned something went wrong
    
    int pieceY = piecePosition / 8;
    int pieceX = piecePosition % 8;
    
    //checks for rooks, queens, and kings on +
    //checks piece's row
    //going left
    for (int i=pieceX-1;i>=0;i--){
        if (position->piecePositions[i][pieceY] != 0){
            if (position->piecePositions[i][pieceY] > 0 == attackingPiecesPlayer){
                
                int attackingPiece = abs(int(position->piecePositions[i][pieceY]));
                if (attackingPiece == 4 || attackingPiece == 5){
                    piecesAttacking++;
                    attackingPieceLocation = 8*pieceY + i;
                }
                else if(attackingPiece == 6 && i == pieceX-1){
                    piecesAttacking++;
                    attackingPieceLocation = 8*pieceY + i;
                }
            }
            i = -1;
        }
    }
    //going right
    for (int i=pieceX+1;i<8;i++){
        if (position->piecePositions[i][pieceY] != 0){
            if (position->piecePositions[i][pieceY] > 0 == attackingPiecesPlayer){
                
                int attackingPiece = abs(int(position->piecePositions[i][pieceY]));
                if (attackingPiece == 4 || attackingPiece == 5){
                    piecesAttacking++;
                    attackingPieceLocation = 8*pieceY + i;
                }
                else if(attackingPiece == 6 && i == pieceX+1){
                    piecesAttacking++;
                    attackingPieceLocation = 8*pieceY + i;
                }
            }
            i = 8;
        }
    }
    
    //checks piece's column
    //going up
    for (int i=pieceY-1;i>=0;i--){
        if (position->piecePositions[pieceX][i] != 0){
            if (position->piecePositions[pieceX][i] > 0 == attackingPiecesPlayer){
                
                int attackingPiece = abs(int(position->piecePositions[pieceX][i]));
                if (attackingPiece == 4 || attackingPiece == 5){
                    piecesAttacking++;
                    attackingPieceLocation = 8*i + pieceX;
                }
                else if(attackingPiece == 6 && i == pieceY-1){
                    piecesAttacking++;
                    attackingPieceLocation = 8*i + pieceX;
                }
            }
            i = -1;
        }
    }
    //going down
    for (int i=pieceY+1;i<8;i++){
        if (position->piecePositions[pieceX][i] != 0){
            if (position->piecePositions[pieceX][i] > 0 == attackingPiecesPlayer){
                
                int attackingPiece = abs(int(position->piecePositions[pieceX][i]));
                if (attackingPiece == 4 || attackingPiece == 5){
                    piecesAttacking++;
                    attackingPieceLocation = 8*i + pieceX;
                }
                else if(attackingPiece == 6 && i == pieceX+1){
                    piecesAttacking++;
                    attackingPieceLocation = 8*i + pieceX;
                }
            }
            i = 8;
        }
    }
    
    //checks for bishops, queens, and pawns on X
    //NW from piece
    for (int i=1;i<=min(pieceY, pieceX);i++){
        //i is the distance in the direction it is going
        //i can't be more than the lowest distance or else it will go out of board
        //min(pieceY, pieceX) must be less than 8 so can use i=8 to exit
        int attackingPiece = position->piecePositions[pieceX - i][pieceY - i];
        if (attackingPiece != 0){
            if (attackingPiece > 0 == attackingPiecesPlayer){
                attackingPiece = abs(attackingPiece);
                if (attackingPiece == 3 || attackingPiece == 5){
                    piecesAttacking++;
                    attackingPieceLocation = 8*(pieceY - i) + pieceX - i;
                }
                else if (i == 1){
                    if (attackingPiecesPlayer && attackingPiece == 1){
                        //if white, only white pawns could attack from the NW
                        piecesAttacking++;
                        attackingPieceLocation = 8*(pieceY - i) + pieceX - i;
                    }
                    else if (attackingPiece == 6){
                        piecesAttacking++;
                        attackingPieceLocation = 8*(pieceY - i) + pieceX - i;
                    }
                }
            }
            i = 8;
        }
    }
    //NE from piece
    for (int i=1;i<=min(pieceY,7 - pieceX);i++){
        //use 7 - x when going in positive direction
        int attackingPiece = position->piecePositions[pieceX + i][pieceY - i];
        if (attackingPiece != 0){
            if (attackingPiece > 0 == attackingPiecesPlayer){
                attackingPiece = abs(attackingPiece);
                if (attackingPiece == 3 || attackingPiece == 5){
                    piecesAttacking++;
                    attackingPieceLocation = 8*(pieceY - i) + pieceX + i;
                }
                else if (i == 1){
                    if (attackingPiecesPlayer && attackingPiece == 1){
                        //if white, only white pawns could attack from the NE
                        piecesAttacking++;
                        attackingPieceLocation = 8*(pieceY - i) + pieceX + i;
                    }
                    else if (attackingPiece == 6){
                        piecesAttacking++;
                        attackingPieceLocation = 8*(pieceY - i) + pieceX + i;
                    }
                }
            }
            i = 8;
        }
    }
    //SW from piece
    for (int i=1;i<=min(7 - pieceY,pieceX);i++){
        //use 7 - x when going in positive direction
        int attackingPiece = position->piecePositions[pieceX - i][pieceY + i];
        if (attackingPiece != 0){
            if (attackingPiece > 0 == attackingPiecesPlayer){
                attackingPiece = abs(attackingPiece);
                if (attackingPiece == 3 || attackingPiece == 5){
                    piecesAttacking++;
                    attackingPieceLocation = 8*(pieceY + i) + pieceX - i;
                }
                else if (i == 1){
                    if (!attackingPiecesPlayer && attackingPiece == 1){
                        //if black, only black pawns could attack from the SW
                        piecesAttacking++;
                        attackingPieceLocation = 8*(pieceY + i) + pieceX - i;
                    }
                    else if (attackingPiece == 6){
                        piecesAttacking++;
                        attackingPieceLocation = 8*(pieceY + i) + pieceX - i;
                    }
                }
            }
            i = 8;
        }
    }
    //SE from piece
    for (int i=1;i<=min(7 - pieceY,7 - pieceX);i++){
        //use 7 - x when going in positive direction
        int attackingPiece = position->piecePositions[pieceX + i][pieceY + i];
        if (attackingPiece != 0){
            if (attackingPiece > 0 == attackingPiecesPlayer){
                attackingPiece = abs(attackingPiece);
                if (attackingPiece == 3 || attackingPiece == 5){
                    piecesAttacking++;
                    attackingPieceLocation = 8*(pieceY + i) + pieceX + i;
                }
                else if (i == 1){
                    if (!attackingPiecesPlayer && attackingPiece == 1){
                        //if black, only black pawns could attack from the SE
                        piecesAttacking++;
                        attackingPieceLocation = 8*(pieceY + i) + pieceX + i;
                    }
                    else if (attackingPiece == 6){
                        piecesAttacking++;
                        attackingPieceLocation = 8*(pieceY + i) + pieceX + i;
                    }
                }
            }
            i = 8;
        }
    }
    
    //check knights
    int xdistance[8] = {-2, -1, 1, 2, 2, 1, -1, -2};
    int ydistance[8] = {-1, -2, -2, -1, 1, 2, 2, 1};
    //checks starting in NW and moves around clockwise
    for (int i=0;i<8;i++){
        //checks the 8 possible knight positions
        if ((pieceX + xdistance[i] >= 0 && pieceX + xdistance[i] < 8) && (pieceY + ydistance[i] >= 0 && pieceY + ydistance[i] < 8)){
            //if not out of range
            int attackingPiece = position->piecePositions[pieceX + xdistance[i]][pieceY + ydistance[i]];
            if (abs(attackingPiece) == 2 && attackingPiece > 0 == attackingPiecesPlayer && attackingPiece != 0){
                piecesAttacking++;
                attackingPieceLocation = 8*(pieceY + ydistance[i]) + pieceX + xdistance[i];
            }
        }
    }
    
    if (piecesAttacking == 1){
        //before it is in standard 0-63 location, but has to shift to allow 0 to represent no pieces attacking
        return -attackingPieceLocation - 1;
    }
    else{
        return piecesAttacking;
    }
}

bool Chess::hasCastled(BoardState *position, bool player){
    if (position == nullptr){
        cout << "ERROR: Chess: hasCastled: invalid position\n";
        return false;
    }
    
    int moveStartingPosition;
    int moveEndingPosition;
    int movedX;
    int movedY;
    int backrow = player ? 0 : 7;
    int kingStartingPosition = 8*backrow + 3;
    
    LLNode* temp = position->previousMoves->head;
    while (temp != nullptr){
        moveStartingPosition = temp->move / 64;
        moveEndingPosition = temp->move % 64;
        movedX = (moveEndingPosition % 8) - (moveStartingPosition % 8);
        movedY = (moveEndingPosition / 8) - (moveStartingPosition / 8);
        if (moveStartingPosition == kingStartingPosition){
            //checks whether anything in the king's original spot has moved
            if (abs(movedX) == 2 && movedY == 0){
                //checks whether that move was a castle
                return true;
            }
            //if king has moved but didn't castle then never will
            return false;
        }
        temp = temp->nextNodePtr;
    }
    //if nothing was ever found, aka the king hasn't moved
    return false;
}

string Chess::toString(BoardState *position){
    if (position == nullptr){
        cout << "ERROR: Chess: toString: invalid position\n";
        return "ERROR\n";
    }
    int i;
        
    string outcome = "\n    0 1 2 3 4 5 6 7\n\n";
    for(int j=0;j<8;j++){
        outcome += to_string(j) + "   ";
        for(i=0;i<8;i++){
            outcome += toLetter(position->piecePositions[i][j]) + " ";
        }
        outcome += "\n";
    }

    return outcome + "\n";
}

string Chess::toString(BoardState *position, int m){
    if (position == nullptr){
        cout << "ERROR: Chess: toString: invalid position\n";
        return "ERROR\n";
    }
    if (not validMove(position, m)){
        cout << "ERROR: Chess: toString: invalid move\n";
        return "ERROR\n";
    }
    
    string output;
    
    int movingPieceX = (m / 64) % 8;
    int movingPieceY = (m / 64) / 8;
    int endSpotX = (m % 64) % 8;
    int endSpotY = (m % 64) / 8;
    
    switch (abs(position->piecePositions[movingPieceX][movingPieceY])) {
        case 1:
            break;
        case 2:
            output += "N";
            break;
        case 3:
            output += "B";
            break;
        case 4:
            output += "R";
            break;
        case 5:
            output += "Q";
            break;
        case 6:
            output += "K";
            break;
            
        default:
            cout << "ERROR: Chess: toString: invalid piece referred to by move\n";
            output += "(unknown piece)";
            break;
    }
    
    string letterOfRow(1, char(65 + endSpotY));
    //uses ascii to make letters count down based on row
    output += letterOfRow;
    
    output += to_string(endSpotX);
    
    if (position->piecePositions[movingPieceX][movingPieceY] == 6){
        if (endSpotX == movingPieceX - 2){
            //king's side castle
            output = "0-0";
        }
        else if (endSpotX == movingPieceX + 2){
            //queen's side castle
            output = "0-0-0";
        }
    }
    
    //check resulting positing for being in check or checkmate
    BoardState* tempOutcome = moveOutcome(position, m, false);
    if (inCheck(tempOutcome, !position->turn)){
        if(tempOutcome->winner != 0){
            output += "#";
        }
        else{
            output += "+";
        }
    }
    delete tempOutcome;
    
    //add coordinate version of output just in case, also because can't check if 2 pieces could go to the same spot without much more process
    output += "  (" + to_string(movingPieceX) + ", " + to_string(movingPieceY) + ") -> (" + to_string(endSpotX) + ", " + to_string(endSpotY) + ")";
    
    return output + "\n";
}

string Chess::toLetter(signed char piece){
    string output;
    switch (abs(int(piece))) {
        case 0:
            output = "-";
            break;
        case 1:
            output = "P";
            break;
        case 2:
            output = "N";
            break;
        case 3:
            output = "B";
            break;
        case 4:
            output = "R";
            break;
        case 5:
            output = "Q";
            break;
        case 6:
            output = "K";
            break;
        default:
            cout << "ERROR: Chess: toLetter: invalid piece\n";
            return "ERROR";
            break;
    }
    if (piece < 0){
        string s(1, tolower(output[0]));
        output = s;
    }
    return output;
}
