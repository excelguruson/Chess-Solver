//
//  Header.h
//  Chess Solver
//
//  Created by Caleb McFarland on 4/24/20.
//  Copyright © 2020 Caleb McFarland. All rights reserved.
//

#ifndef Header_h
#define Header_h


#endif /* Header_h */
using namespace std;

struct LLNode;
struct BoardLLNode;
struct GTNode;
struct BoardState;
class LL;
class GT;
class Chess;
class ComputerPlayer;

struct LLNode
{
public:
    signed short int move;
    /*
    each space is numbered 0 to 63
    each move is the first space of the piece you want to move * 64
    plus the second space of where you want the piece to go
    0 <= move <= 4095
    */
    LLNode* nextNodePtr;
    GTNode* correspondingPosition;
};

struct GTNode
{
    GTNode();
    ~GTNode();
    
    BoardState* position;
    LL* possibleMoves;
    
};

struct BoardState
{
    signed char piecePositions[8][8];
    /*
     can experiment with storing a list of the number of blank spaces or the piece in that space going through the board like reading
     00 10 20 30 40...
     01
     02
     03
     ...
     0 - blank
     1 - pawn
     2 - knight
     3 - bishop
     4 - rook
     5 - queen
     6 - king
     black negative
     white at top (white is usually on side starting with 1 and black at 8)
     */
    bool turn;
    //turn is true if it is white's turn, false if it is black's turn
    float evaluation;
    //a number between 0 and 1, 1 being a won game
    signed char winner;
    //1 if white won, -1 if black won, and 0 if nobody has won
    LL* previousMoves;
    //need to store previous moves for each BoardState to see if it is a tie
    //(threefold repetition and checking if can castle)
    
    BoardState();
    ~BoardState();
};

class LL
{
public:
    LLNode* head;
    LLNode* tail;
    
    LL();
    
    void addNode(int m);
    void insertHead(int m);
    void insert(int index, int m);
    
    void deleteNode(int index);
    void deleteHead();
    void deleteTail();
    
    void print();
    int length();
    int getValue(int index);
    //really get move in this case
    LLNode* findNode(int m);
    //returns the pointer to the first node with a given move
    
    ~LL();
};

class GT
{
public:
//private:
    void destroyTree(GTNode* subroot);
    //recursively destroys the tree
    void destroyTopOfTree(GTNode* savedSubroot, GTNode* subroot);
    //recursively destroys all of the tree except one of the subroots
    
//public:
    GTNode* root;
    
    GT();
    
    void setRoot(BoardState* startingPosition);
    //called when initially creating the GT. The root changes as game progresses
    void updateRoot(GTNode* newRoot);
    //sets the root to be a different node. Must only be done after the top of the tree is destroyed.
    void addPath(GTNode* currentBoard, int m, BoardState* newPosition);
    GTNode* makeMove(GTNode* currentBoard, int m);
    
    GTNode* findPosition(LL* moveList, GTNode* currentBoard);
    //starts from beginning position by default, or specified position
    GTNode* findPosition(LL* moveList);
    //moveList is a linked list of moves, starting with the head
    //not a list of possible moves, rather an order of moves from the start
    
    float evaluate(GTNode* currentBoard, bool player, bool correct);
    //recursively evaluates the position as the best or worse
    //of the possible future positions, depending on the turn.
    //if player is true it evaluates for the player playing as 1.
    //if correct is true it will set each evaluation
    //to the highest of possible evaluations.
    void fixEvaluations(bool player);
    
    void destroyTopOfTree(GTNode* savedSubroot);
    void destroyTree();
    //destroys tree from root
    ~GT();
};

class Chess
{
public:
    
    BoardState* startingPosition;
    
    Chess();
    
    BoardState* moveOutcome(BoardState* previousPosition, int m, bool checkIfValid=true, bool setGameOver=true);
    //the int m correspondes to the index the column the player wants to drop their piece
    //checkIfValid can be false if already checked if valid but just want to move pieces
    //setGameOver can be set to false if it is not needed in a different function in order to save time
    bool validMoveWithoutCheckForCheck(BoardState* position, int m, bool checkForTurn);
    //valid move but doesn't check if opponent is in check after the move
    //seperate function so return can exit easier
    bool validMove(BoardState* position, int m, bool checkForCheck=true, bool checkForTurn=true);
    //returns true if the move is a valid move
    //if checkForCheck is false then validMove doesn't care if the move puts the mover in check
    //if checkForTurn is false then it doesn't matter which piece you move
    
    LL* candidatePossibleMoves(BoardState* position);
    //returns a LL of moves for testing from a certain position
    //used to determine if can get out of check, stalemate, and making GT
    
    bool samePiecePositions(BoardState* position1, BoardState* position2);
    //returns true if every piece is in the same position
    bool isTie(BoardState* position);
    //returns true if it is a tie
    int checkGameOver(BoardState* position);
    //sets the winner state of board position to 0, -1, or 1 and returns that value
    //win by not having any of a piece
    bool inCheck(BoardState* position, bool player);
    //returns if the player is in check, with true being white and false black
    signed int numPiecesAttacking(BoardState* position, int piecePosition, bool attackingPiecesPlayer);
    //returns the number of attackingPiecesPlayer's pieces attacking a certain spot specified by piecePosition using 0-63
    //IF ONLY ONE PIECE ATTACKING: returns the negative value corresponding to the location of the attacking piece using 1-64
    //ONE TO SIXTY FOUR because 0 is used by having no pieces attacking
    bool hasCastled(BoardState* position, bool player);
    //checks whether the given player has castled yet
    
    string toString(BoardState* position);
    //returns the board as a string
    string toString(BoardState* position, int m);
    //returns a move in move notation
    //can't differentiate moves where two pieces can go to the same place, so also includes coordinate style
    string toLetter(signed char piece);
    //returns the letter corresponding to that piece in standard notation (black pieces lowercase)
};

class ComputerPlayer
{
public:
//private:
    GT* solvedGame;
    Chess* gamePlaying;
    GTNode* currentPosition;
    
//public:
    bool playingAsWhite;
    //is true if playing as 1
    
    ComputerPlayer(bool isWhite);
    //set whether the computer is playing as 1
    
    void createGT(GTNode* fromNode, int size);
    //size is how many moves deep to make the game tree (how many moves it can see ahead)
    //only fixes evaluations if non-recursive version is called
    //size must be greater than 2 for the computer to have enough information to make both it's and the opponent's move
    void createGT(int size);
    void updateGT(int size);
    //takes the current position and makes it the root of the tree and then makes the tree larger from the new position
    
    //could make it so that it stops once it finds a winning move (save ram but take a slow way sometimes)
    //could also make it not explore moves with a bad enough rating (only for games that use hueristics)
    //should minimize distance to winning move now
    //should maxamize distance to losing move now
    //should minimize the number of winning options for the opponent
    
    void createEvaluateDestroyGT(GTNode* fromNode, int size, int startDeletingAtSize);
    //creates a GT but whenever it finishes it evaluates and destroys what it used to evaluate
    //that way it only ever stores the evaluations and not the entire GT each time
    //startDeletingAtSize is the size where it will start recusively deleting as it goes
    void createEvaluateDestroyGT(int size);
    //does the function from the current position and chops off the rest
    
    int bestMove();
    //returns the best move from the currentPosition
    //now returns a random of the best possible moves
    //(doesn't default to the first node)
    
    int movesToWin(GTNode* someNode, int winner);
    //returns the number of moves away the win is for whatever player is evaluated as winner in the GTNode someNode
    
    void updateCurrentPosition(int m, int autoUpdateGTSize);
    //if autoUpdateGTSize is 0, GT will not update when a move is made
    //if it is >0, GT will update with that size
    //must be greater than 2 if the initial creation of the GT is not big enough, otherwise one might run into a move where
    //the computer doesn't know where it is possible to go
    
    float initialEvaluation(BoardState* position);
    //combination of material evaluation and position evaluation
    float materialEvaluation(BoardState* position);
    //uses the amount of pieces to make an evaluation between 0 and 1
    int pieceEvaluation(int piece);
    //returns the evaluation of piece (P = 1, N = 3, Q = 9)
    float positionEvaluation(BoardState* position);
    //uses the position of the pieces to make an evaluation between 0 and 1
    
    ~ComputerPlayer();
};
