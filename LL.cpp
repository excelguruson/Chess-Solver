//
//  LL.cpp
//  Chess Solver
//
//  Created by Caleb McFarland on 4/24/20.
//  Copyright © 2020 Caleb McFarland. All rights reserved.
//

#include <stdio.h>
#include <iostream>
#include "Header.h"
using namespace std;

LL::LL(){
    head = nullptr;
    tail = nullptr;
}

void LL::addNode(int m){
    LLNode* temp = new LLNode;
    temp->move = m;
    temp->nextNodePtr = nullptr;
    temp->correspondingPosition = nullptr;
    if (head == nullptr){
        head = temp;
        tail = temp;
        temp = nullptr;
    }
    else{
        tail->nextNodePtr = temp;
        tail = temp;
    }
}

void LL::insertHead(int m){
    LLNode* temp = new LLNode;
    temp->move = m;
    temp->correspondingPosition = nullptr;
    temp->nextNodePtr = head;
    head = temp;
}

void LL::insert(int index, int m){
    if (index == 0){
        insertHead(m);
    }
    else if (index < length()){
        LLNode* currentNodePtr = head;
        LLNode* previousNodePtr = nullptr;
        LLNode* temp = new LLNode;
        for(int i=0;i<index;i++){
            previousNodePtr = currentNodePtr;
            currentNodePtr = currentNodePtr->nextNodePtr;
        }
        temp->move = m;
        temp->correspondingPosition = nullptr;
        temp->nextNodePtr = currentNodePtr;
        previousNodePtr->nextNodePtr = temp;
        
    }
    else if (index == length()){
        addNode(m);
    }
    else{
        cout << "ERROR: MoveLL: insert: index out of range\n";
        return;
    }
}

void LL::deleteNode(int index){
    if (head == nullptr) return;
    else if (length() == 1){
        LLNode* temp = head;
        head = nullptr;
        delete temp;
    }
    else if (index < 0){
        cout << "ERROR: MoveLL: deleteNode: invalid index\n";
    }
    else if (index == 0){
        deleteHead();
    }
    else if (index == length() - 1){
        deleteTail();
    }
    else if (index < length()){
        LLNode* currentNodePtr = head;
        LLNode* previousNodePtr = nullptr;
        for(int i=0;i<index;i++){
            previousNodePtr = currentNodePtr;
            currentNodePtr = currentNodePtr->nextNodePtr;
        }
        previousNodePtr->nextNodePtr = currentNodePtr->nextNodePtr;
        delete currentNodePtr;
    }
    else{
        cout << "ERROR: MoveLL: deleteNode: invalid index\n";
        return;
    }
}

void LL::deleteHead(){
    if (head == nullptr) return;
    else if(length() == 1){
        LLNode* temp = head;
        head = nullptr;
        tail = nullptr;
        delete temp;
        return;
    }
    LLNode* temp = head;
    head = head->nextNodePtr;
    delete temp;
}

void LL::deleteTail(){
    if (head == nullptr) return;
    else if(length() == 1){
        delete head;
        head = nullptr;
        tail = nullptr;
        return;
    }
    LLNode* currentNodePtr = head;
    LLNode* previousNodePtr = nullptr;
    while (currentNodePtr->nextNodePtr != nullptr){
        previousNodePtr = currentNodePtr;
        currentNodePtr = currentNodePtr->nextNodePtr;
    }
    tail = previousNodePtr;
    tail->nextNodePtr = nullptr;
    delete currentNodePtr;
}

void LL::print(){
    LLNode* temp = head;
    while (temp != nullptr){
        if (temp->nextNodePtr != nullptr){
            cout << temp->move << ", ";
        }
        else{
            cout << temp->move << "\n";
        }
        temp = temp->nextNodePtr;
    }
}

int LL::length(){
    if (head == nullptr){
        return 0;
    }
    int l = 1;
    LLNode* temp = head;
    while (temp->nextNodePtr != nullptr){
        temp = temp->nextNodePtr;
        l++;
    }
    return l;
}

int LL::getValue(int index){
    if (index < 0){
        cout << "ERROR: MoveLL: getValue: invalid index\n";
        return 0;
    }
    else if (index == 0){
        return head->move;
    }
    else if (index == length() - 1){
        return tail->move;
    }
    else if (index < length()){
        LLNode* temp = head;
        for(int i=0;i<index;i++){
            temp = temp->nextNodePtr;
        }
        return temp->move;
    }
    else{
        cout << "ERROR: MoveLL: getValue: invalid index\n";
        return 0;
    }
}

LLNode* LL::findNode(int m){
    LLNode* temp = head;
    while (temp != nullptr){
        if (temp->move == m){
            return temp;
        }
        else{
            temp = temp->nextNodePtr;
        }
    }
    return nullptr;
}

LL::~LL(){
    while (head != nullptr){
        deleteHead();
    }
}
