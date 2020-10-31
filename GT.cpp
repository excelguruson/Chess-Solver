//
//  GT.cpp
//  Chess Solver
//
//  Created by Caleb McFarland on 4/24/20.
//  Copyright © 2020 Caleb McFarland. All rights reserved.
//

#include <stdio.h>
#include <iostream>
#include "Header.h"
using namespace std;

GTNode::GTNode(){
    position = nullptr;
    possibleMoves = nullptr;
}

GTNode::~GTNode(){
    delete position;
    delete possibleMoves;
}

GT::GT(){
    root = nullptr;
}

void GT::setRoot(BoardState *startingPosition){
    root = new GTNode;
    root->position = startingPosition;
    root->possibleMoves = new LL;
}

void GT::updateRoot(GTNode *newRoot){
    root = newRoot;
}

void GT::addPath(GTNode *currentBoard, int m, BoardState *newPosition){
    if (currentBoard == nullptr){
        cout << "ERROR: GT: addPath: currentBoard cannot be NULL\n";
        return;
    }
    if (currentBoard->possibleMoves->findNode(m) == nullptr){
        currentBoard->possibleMoves->addNode(m);
        
        GTNode* temp = new GTNode;
        temp->position = newPosition;
        temp->possibleMoves = new LL;
        
        currentBoard->possibleMoves->tail->correspondingPosition = temp;
        temp = nullptr;
    }
}

GTNode* GT::makeMove(GTNode *currentBoard, int m){
    return currentBoard->possibleMoves->findNode(m)->correspondingPosition;
}

GTNode* GT::findPosition(LL *moveList, GTNode *currentBoard){
    if (moveList->head != nullptr){
        int nextMove = moveList->head->move;
        moveList->deleteHead();
        return findPosition(moveList, makeMove(currentBoard, nextMove));
    }
    else{
        return currentBoard;
    }
}

GTNode* GT::findPosition(LL *moveList){
    return findPosition(moveList, root);
}

float GT::evaluate(GTNode *currentBoard,bool player, bool correct){
    if (currentBoard->possibleMoves->head == nullptr){
        return currentBoard->position->evaluation;
    }
    
    double bestEvaluation;
    double tempEvaluation;
    bool playersTurn;
    
    playersTurn = (currentBoard->position->turn == player);
    
    LLNode* temp = currentBoard->possibleMoves->head;
    bestEvaluation = evaluate(temp->correspondingPosition, player, correct);
    
    while (temp != nullptr){
        tempEvaluation = evaluate(temp->correspondingPosition, player, correct);
        if (playersTurn && tempEvaluation > bestEvaluation){
            bestEvaluation = tempEvaluation;
            //returns highest evaluation of possible future positions if it is the computers turn
            //could have it exit loop once it found a guaranteed win,
            //but then it would not be able to find the fastest win.
        }
        else if (!playersTurn && tempEvaluation < bestEvaluation){
            bestEvaluation = tempEvaluation;
            //and returns the worst evaluation if it isn't (assumes opponent will make optimal move)
        }
        temp = temp->nextNodePtr;
    }
    if (correct){
        currentBoard->position->evaluation = bestEvaluation;
    }
    return bestEvaluation;
}

void GT::fixEvaluations(bool player){
    evaluate(root, player, true);
}

void GT::destroyTree(GTNode *subroot){
    while (subroot->possibleMoves->head != nullptr){
        destroyTree(subroot->possibleMoves->tail->correspondingPosition);
        subroot->possibleMoves->deleteTail();
    }
    //delete subroot->possibleMoves;
    //delete subroot->position;
    delete subroot;
}

void GT::destroyTopOfTree(GTNode *savedSubroot, GTNode *subroot){
    while (subroot->possibleMoves->head != nullptr){
        if (subroot->possibleMoves->tail->correspondingPosition == savedSubroot){
            subroot->possibleMoves->tail->correspondingPosition = nullptr;
            //if the tail points to the subroot we want to save, then the pointer is deassigned and the tail is deleted
            //nothing points to the saved subroot any longer
        }
        else{
            destroyTopOfTree(savedSubroot, subroot->possibleMoves->tail->correspondingPosition);
        }
        
        subroot->possibleMoves->deleteTail();
    }
    //delete subroot->possibleMoves;
    delete subroot;
}

void GT::destroyTopOfTree(GTNode *savedSubroot){
    if (savedSubroot == root){
        return;
    }
    destroyTopOfTree(savedSubroot, root);
    updateRoot(savedSubroot);
}

void GT::destroyTree(){
    destroyTree(root);
}

GT::~GT(){
    destroyTree();
}
