//
//  Computer Player.cpp
//  Chess Solver
//
//  Created by Caleb McFarland on 4/24/20.
//  Copyright © 2020 Caleb McFarland. All rights reserved.
//

#include <stdio.h>
#include <iostream>
#include "Header.h"
using namespace std;


ComputerPlayer::ComputerPlayer(bool isWhite){
    solvedGame = new GT;
    gamePlaying = new Chess;
    
    solvedGame->setRoot(gamePlaying->startingPosition);
    currentPosition = solvedGame->root;
    
    playingAsWhite = isWhite;
}

void ComputerPlayer::createGT(GTNode* fromNode, int size){
    if (fromNode == nullptr){
        cout << "ERROR: ComputerPlayer: createGT: invalid fromNode\n";
        return;
    }
    if (fromNode->position->winner != 0){
        return;
    }
    if (size <= 0){
        return;
    }
   
    LLNode* temp = gamePlaying->candidatePossibleMoves(fromNode->position)->head;
    int move;
    while (temp != nullptr){
        move = temp->move;
        if (fromNode->possibleMoves->findNode(move) != nullptr){
            createGT(solvedGame->makeMove(fromNode, move), size - 1);
            //doesn't remake nodes when updating the game tree
            //duplicates are checked from in the addPath() function, but checking here would save more time
        }
        else if (gamePlaying->validMove(fromNode->position, move)){
            BoardState* newPosition = gamePlaying->moveOutcome(fromNode->position, move);
            
            newPosition->evaluation = initialEvaluation(newPosition);
            solvedGame->addPath(fromNode, move, newPosition);
            
            createGT(solvedGame->makeMove(fromNode, move), size - 1);
        }
        temp = temp->nextNodePtr;
    }
    return;
}

void ComputerPlayer::createGT(int size){
    if (size <= 2){
        cout << "ERROR: ComputerPlayer: createGT: size must be greater than 2 for computer to know where to move\n";
        //could break if size is less than a combo the opponent does
    }
    createGT(solvedGame->root, size);
    solvedGame->fixEvaluations(playingAsWhite);
}

void ComputerPlayer::updateGT(int size){
    if (size <= 2){
        cout << "ERROR: ComputerPlayer: updateGT: size must be greater than 2 for computer to know where to move\n";
        //could break if size is less than a combo the opponent does
    }
    solvedGame->destroyTopOfTree(currentPosition);
    //makes current position the root of the new tree and cuts off the rest
    createGT(size);
    solvedGame->fixEvaluations(playingAsWhite);
}

void ComputerPlayer::createEvaluateDestroyGT(GTNode *fromNode, int size, int startDeletingAtSize){
    if (fromNode == nullptr){
        cout << "ERROR: ComputerPlayer: createGT: invalid fromNode\n";
        return;
    }
    if (fromNode->position->winner != 0){
        return;
    }
    if (size <= 0){
        return;
    }
    
    LL* candidatePossibleMoves = gamePlaying->candidatePossibleMoves(fromNode->position);
    LLNode* temp = candidatePossibleMoves->head;
    int move;
    while (temp != nullptr){
        move = temp->move;
        if (fromNode->possibleMoves->findNode(move) != nullptr){
            createEvaluateDestroyGT(solvedGame->makeMove(fromNode, move), size - 1, startDeletingAtSize);
            //doesn't remake nodes when updating the game tree
            //duplicates are checked from in the addPath() function, but checking here would save more time
        }
        else if (gamePlaying->validMove(fromNode->position, move)){
            
            BoardState* newPosition = gamePlaying->moveOutcome(fromNode->position, move, false);
            
            newPosition->evaluation = initialEvaluation(newPosition);
            solvedGame->addPath(fromNode, move, newPosition);
            
            createEvaluateDestroyGT(solvedGame->makeMove(fromNode, move), size - 1, startDeletingAtSize);
        }
        
        temp = temp->nextNodePtr;
    }
    
    if (size <= startDeletingAtSize){
        //must keep a tree of size at least 2 so that the computer can make moves within the tree
        
        //after creating all possible moves from the position, it stores thier information in the evaluation and deletes the tree past it
        solvedGame->evaluate(fromNode, playingAsWhite, true);
        while (fromNode->possibleMoves->head != nullptr){
            solvedGame->destroyTree(fromNode->possibleMoves->tail->correspondingPosition);
            fromNode->possibleMoves->deleteTail();
        }
    }
    
    delete candidatePossibleMoves;
    return;
}

void ComputerPlayer::createEvaluateDestroyGT(int size){
    if (size <= 2){
        cout << "ERROR: ComputerPlayer: createEvaluateDestroyGT: size must be greater than 2 for computer to know where to move\n";
        //could break if size is less than a combo the opponent does
    }
    solvedGame->destroyTopOfTree(currentPosition);
    createEvaluateDestroyGT(currentPosition, size, size-5);
    //size of 5 uses 1.62 Gb, any larger will be much more
    solvedGame->fixEvaluations(playingAsWhite);
}

int ComputerPlayer::bestMove(){
    if (currentPosition->position->turn != playingAsWhite){
        cout << "ERROR: ComputerPlayer: bestMove: can't move if it isn't the computer's turn\n";
        return -1;
    }
    
    int numberWithBestEvaluation = 0;
    double bestEvaluation;
    int outcome = -1;
    
    LLNode* temp = currentPosition->possibleMoves->head;
    bestEvaluation = temp->correspondingPosition->position->evaluation;

    
    while (temp != nullptr){
        if (temp->correspondingPosition->position->evaluation > bestEvaluation){
            bestEvaluation = temp->correspondingPosition->position->evaluation;
            numberWithBestEvaluation = 1;
        }
        else if (temp->correspondingPosition->position->evaluation == bestEvaluation){
            numberWithBestEvaluation++;
            //enters the while loop while temp is still the head, so numbersWithBestEvaluation doesn't need to initialize it to 1
        }
        temp = temp->nextNodePtr;
    }
    /*
    can't check the fastest way if only storing one move ahead
     
    if (bestEvaluation == 1){
        int desiredWinner = playingAsRed ? 1 : -1;
        int quickestWin = movesToWin(currentPosition, desiredWinner) - 1;
        
        temp = currentPosition->possibleMoves->head;
        while (temp != nullptr){
            if (temp->correspondingPosition->position->evaluation == 1){
                if (movesToWin(temp->correspondingPosition, desiredWinner) == quickestWin){
                    return temp->move;
                }
            }
            temp = temp->nextNodePtr;
        }
    }
    else if (bestEvaluation == 0){
        int undesiredWinner = playingAsRed ? -1 : 1;
        int slowestWin = 0;
        int tempMovesToWin = 0;
        
        temp = currentPosition->possibleMoves->head;
        
        while (temp != nullptr){
            tempMovesToWin = movesToWin(temp->correspondingPosition, undesiredWinner);
            if (tempMovesToWin > slowestWin){
                slowestWin = tempMovesToWin;
                outcome = temp->move;
            }
            temp = temp->nextNodePtr;
        }
        return outcome;
    }
    else{
        */
        int randomBestMoveOutcome = rand() % numberWithBestEvaluation;
        //random number from 0 to numberWithBestEvaluation-1
        
        temp = currentPosition->possibleMoves->head;
        while (temp != nullptr){
            if (temp->correspondingPosition->position->evaluation == bestEvaluation){
                if (randomBestMoveOutcome <= 0){
                    outcome = temp->move;
                    return outcome;
                }
                else{
                    randomBestMoveOutcome--;
                }
            }
            temp = temp->nextNodePtr;
        }
    //}
    
    if (outcome == -1){
        cout << "ERROR: ComputerPlayer: bestMove: outcome not set\n";
    }
    return outcome;
}

int ComputerPlayer::movesToWin(GTNode *someNode, int winner){
    if (!(someNode->position->evaluation == 0 or someNode->position->evaluation == 1)){
        cout << "ERROR: ComputerPlayer: distanceToWin: win is not garunteed\n";
        return -1;
    }
    if (someNode->position->winner == winner){
        return 0;
    }
    else{
        int leastNumberOfMoves = INT_MAX;
        //sets the least number to be as high as possible so that the first value that actually has a distance will replace it
        int tempNumberOfMoves;
        LLNode* temp = someNode->possibleMoves->head;
                
        while (temp != nullptr){
            if (temp->correspondingPosition->position->evaluation == ((winner == 1) ? 1 : 0)){
                 tempNumberOfMoves =  movesToWin(temp->correspondingPosition, winner) + 1;
                if (tempNumberOfMoves < leastNumberOfMoves){
                    leastNumberOfMoves = tempNumberOfMoves;
                }
            }
            temp = temp->nextNodePtr;
        }
        return leastNumberOfMoves;
        
    }
}

void ComputerPlayer::updateCurrentPosition(int m, int autoUpdateGTSize){
    currentPosition = solvedGame->makeMove(currentPosition, m);
    if (autoUpdateGTSize > 0){
        //updateGT(autoUpdateGTSize);
        createEvaluateDestroyGT(autoUpdateGTSize);
    }
}

float ComputerPlayer::initialEvaluation(BoardState *position){
    int desiredWinner;
    int undesiredWinner;
    if (playingAsWhite){
        desiredWinner = 1;
        undesiredWinner = -1;
    }
    else{
        desiredWinner = -1;
        undesiredWinner = 1;
    }
    
    if (position->winner == 0){
        float materialMultiplier = 0.9;
        float positionMultiplier = 0.1;
        //should add up to 1
        
        return materialMultiplier * materialEvaluation(position) + positionMultiplier * positionEvaluation(position);
    }
    else if (position->winner == desiredWinner){
        return 1;
    }
    else if (position->winner == undesiredWinner){
        return 0;
    }
    else{
        cout << "ERROR: ComputerPlayer: initialEvaluation: position has invalid winner\n";
        //cout << "position's winner: " << position->winner << "\n";
        return 0.5;
    }
}

float ComputerPlayer::materialEvaluation(BoardState *position){
    float computerTotalPieceValue = 0;
    float opponentTotalPieceValue = 0;
    
    int piece;
    for (int i=0;i<8;i++){
        for (int j=0;j<8;j++){
            
            piece = position->piecePositions[i][j];
            if (piece != 0){
                if (abs(piece) != 6){
                    
                    if (piece > 0 == playingAsWhite){
                        computerTotalPieceValue += pieceEvaluation(piece);
                    }
                    else{
                        opponentTotalPieceValue += pieceEvaluation(piece);
                    }
                }
            }
        }
    }
    float outcome = computerTotalPieceValue / (computerTotalPieceValue + opponentTotalPieceValue);
    return outcome;
}

int ComputerPlayer::pieceEvaluation(int piece){
    if (piece == 0){
        cout << "ERROR: ComputerPlayer: pieceEvaluation: piece is 0\n";
    }
    if (piece == 6){
        cout << "ERROR: ComputerPlayer: pieceEvalutaion: can't evaluate king\n";
    }
    if (piece < -5 || piece > 5){
        cout << "ERROR: ComputerPlayer: pieceEvalution: invalid piece\n";
        cout << "piece: " << piece << "\n";
    }
    switch (abs(piece)) {
        case 1:
            return 1;
            break;
        case 2:
            return 3;
            break;
        case 3:
            return 3;
            break;
        case 4:
            return 5;
            break;
        case 5:
            return 9;
            break;
        default:
            cout << "ERROR: ComputerPlayer: pieceEvalutaion: shouldn't get through switch\n";
            return 0;
            break;
    }
}

float ComputerPlayer::positionEvaluation(BoardState *position){
    //Goes square by square finding the number of pieces from each player attacking that square
    float computerNumAttacking = 0;
    float opponentNumAttacking = 0;
    float tempTotal = 0;
    float totalValue = 0;
    float queenDampener = 0.75;
    //how much less a space attacked by a queen should contribute
    float castleBoost = 1.1;
    //how much castling helps by, may need to adjust
    for (int x=0;x<8;x++){
        for (int y=0;y<8;y++){
            computerNumAttacking = gamePlaying->numPiecesAttacking(position, 8*y + x, playingAsWhite);
            if (computerNumAttacking < 0){
                if (abs(position->piecePositions[(-int(computerNumAttacking))%8][(-int(computerNumAttacking))/8]) == 5){
                    //trying to make it not expose the queen as readily, only works if queen is the only piece attacking which is not ideal
                    //making it so that it always knows what pieces are attacking would require changing numPiecesAttacking a lot
                    computerNumAttacking = queenDampener;
                }
                else{
                    computerNumAttacking = 1;
                }
            }
            opponentNumAttacking = gamePlaying->numPiecesAttacking(position, 8*y + x, !playingAsWhite);
            if (opponentNumAttacking < 0){
                if (abs(position->piecePositions[(-int(opponentNumAttacking))%8][(-int(opponentNumAttacking))/8]) == 5){
                    opponentNumAttacking = queenDampener;
                }
                else{
                opponentNumAttacking = 1;
                }
            }
            
            //may need to make it only matter for the first however many turns
            if (gamePlaying->hasCastled(position, playingAsWhite)) computerNumAttacking *= castleBoost;
            if (gamePlaying->hasCastled(position, !playingAsWhite)) opponentNumAttacking *= castleBoost;
            
            //could put in other things, like protecting your own piece
            tempTotal = computerNumAttacking - opponentNumAttacking;
            if ((x == 3 || x == 4) && (y == 3 || y == 4)){
                //adds value to middle squares
                if (position->previousMoves->length() < 20){
                    //if in the first 10 moves
                    tempTotal *= 1.5;
                }
                else{
                    tempTotal *= 1.25;
                }
            }
            totalValue += tempTotal;
        }
    }
    //hopefully between 0 and 1 but it's hard to know the maximum it could be
    //could just divide by INTEGER_MAX but it would be very low
    return (totalValue / 64) + 0.5;
}

ComputerPlayer::~ComputerPlayer(){
    delete solvedGame;
    delete gamePlaying;
}
