//
//  AIState.cpp
//  Lasca Game
//
//  Created by Pietro Ribeiro Pepe on 20/05/19.
//  Copyright © 2019 Pietro Ribeiro Pepe. All rights reserved.
//

/*
 MIN/MAX AI, depth Tree with score = number of player pieces
    - Player tries to maximize
    - Opponent tries to minimize
    - Board state saved on 19bits bitset: id, flip, pieces
 */

#include "AIState.hpp"
//#include <list>
//#include <bitset>

#define SIZE 7
#define NBITS 19

#define ID_NONE -1
#define ID_PLAYER 0
#define ID_AI 1

using namespace std;

int computeBest(Board *board, char score, char depth, char opId, int *answer = nullptr);

//char board[7][7];
//Board *board;

//for ai access
Piece *removeLastPiece(PieceStack *ps){
    Piece *p = ps->pieces.back();
    ps->pieces.pop_back();
    return p;
}
void addLastPiece(PieceStack *ps, Piece *p){
    ps->pieces.push_front(p);
}


int playerId(PieceStack *ps){
    return (ps == nullptr || !ps->hasPiece()) ? ID_NONE : ps->getId();
}
int nPieces(PieceStack *ps){
    return ps->pieceCount(); //care for null
}
int getPiece(PieceStack *ps, int pos=0){
    return ps->getId();
    //return v.test(7+pos) ? 1 : 2; //verify
}

int getDir(int id){
    return 1-2*id;
}
int nextPlayer(int id){
    return (id+1)%2;
}
bool betterScore(int currScore, int newScore, int id){
    return (id == ID_PLAYER) == (newScore > currScore);
}
int nextScore(int currScore, int id){
    return currScore + (id == ID_PLAYER ? 1 : -1);
}

void doMove(Board *board, int i, int j, int targetI, int targetJ, bool scoring = false){
    PieceStack *ps = board->getPieceAtCoord(i, j);
    board->movePiece(ps, targetI, targetJ);
    if(scoring)
        ps->includePiece(board->removePiece((i+targetI)/2, (j+targetJ)/2));
}
void undoMove(Board *board, int i, int j, int targetI, int targetJ, bool scoring = false){
    PieceStack *ps = board->getPieceAtCoord(targetI, targetJ);
    board->movePiece(ps, i, j);
    if(scoring){
        int midI = (i+targetI)/2, midJ = (j+targetJ)/2;
        PieceStack *midPs = board->getPieceAtCoord(midI, midJ);
        Piece *midP = removeLastPiece(ps);
        if(midPs == nullptr){
            board->pieces[midI][midJ] = new PieceStack(midP->getId(), midJ, midI);
            if(midP->getFlip())
                board->pieces[midI][midJ]->flip();
        }
        else{
            addLastPiece(midPs, midP);
        }
    }
}


//i row, j column

/* SCORING MOVES CHECK */

//Check if piece (i,j) is movable to (targetI,targetJ)
//If yes, return true, and:
//  * if addTarget = false (default): add (i,j) to list l
//  * if addTarget = true: add INSTEAD (targetI, targetJ) to list l
bool addIfExistC(list<int*>* l, Board *board, char id, int i, int j, int targetI, int targetJ, bool addTarget = false, bool twoStep = false){
    
    if(targetJ < SIZE && targetJ >= 0 && playerId(board->getPieceAtCoord(targetI, targetJ))==ID_NONE){
        int targetId = playerId(board->getPieceAtCoord((targetI+i)/2, (targetJ+j)/2));
        if(!twoStep || (targetId != ID_NONE && targetId != id)){
            int *coord = new int[2];
            if(addTarget){ coord[0] = targetI; coord[1] = targetJ; }
            else{ coord[0] = i; coord[1] = j; }
            l->push_back(coord);
            return true;
        }
    }
    return false;
}

//Check if piece (i,j) is movable to (targetI,???)
//If yes, return true, and:
//  * if addTarget = false (default): add (i,j) to list l
//  * if addTarget = true: add INSTEAD EVERY (targetI, ???) to list l
bool addIfExistR(list<int*>* l, Board *board, char id, int i, int j, int targetI, bool addTarget = false, bool twoStep = false){
    int dist = twoStep ? 2 : 1;
    if(targetI < SIZE && targetI >= 0){ //if can move forward
        if(addTarget){
            bool s1 = addIfExistC(l, board, id, i, j, targetI, j+dist, true, twoStep);
            bool s2 = addIfExistC(l, board, id, i, j, targetI, j-dist, true, twoStep);
            return s1 || s2;
        }
        return addIfExistC(l, board, id, i, j, targetI, j+dist, false, twoStep) || addIfExistC(l, board, id, i, j, targetI, j-dist, false, twoStep);
    }
    return false;
}

//Build a list of possible normal target positions for piece at (i,j)
list<int *> *getListOfNormalMoves(Board *board, int i, int j, char opId){
    list<int *> *l = new list<int *>;
    int dir = getDir(opId);
    addIfExistR(l, board, opId, i, j, i+dir, true, false);
    if(board->getPieceAtCoord(i, j)->isFlipped())
        addIfExistR(l, board, opId, i, j, i-dir, true, false);
    return l;
}

//Build a list of possible scoring target positions for piece at (i,j)
list<int *> *getListOfScoringMoves(Board *board, int i, int j, char opId){
    list<int *> *l = new list<int *>;
    int dir = getDir(opId);
    addIfExistR(l, board, opId, i, j, i+2*dir, true, true);
    if(board->getPieceAtCoord(i, j)->isFlipped())
        addIfExistR(l, board, opId, i, j, i-2*dir, true, true);
    return l;
}

void listOfScoringMovables(list<int*>* l, Board *board, char opId){
    int i,j,id,dir;
    //for evaluation
    PieceStack *ps;
    for(i=0;i<SIZE;i++){
        for(j=0;j<SIZE;j++){
            ps = board->getPieceAtCoord(i, j);
            id = playerId(ps);
            if(id == opId){
                dir = getDir(opId);
                addIfExistR(l, board, id, i, j, i+2*dir, false, true) || ( ps->isFlipped() && addIfExistR(l, board, id, i, j, i-2*dir, false, true));
            }
        }
    }
}

int *copyAnswer(int *pointer, int n){
    int *p = new int[n];
    while(n--){
        p[n] = pointer[n];
    }
    return p;
}

void copyAnswer(int *from, int *to, int n){
    while(n--){
        to[n] = from[n];
    }
}

void copyAnswer(int *from, int *to, int *answer){
    answer[0] = from[0];
    answer[1] = from[1];
    answer[2] = to[0];
    answer[3] = to[1];
}

int bestScore(Board *board, char score, char depth, char opId, int i, int j, int *answer = nullptr){
    list<int *> *moves = getListOfScoringMoves(board, i, j, opId);
    int ti,tj;
    int *move;
    int localScore,bScore = -1;
    
    
    if(moves->size() == 0){
        return computeBest(board, score, depth-1, nextPlayer(opId)); //next player
    }
    int best;
    for(auto it = moves->begin();it!=moves->end();++it){
        move = *it;
        ti = move[0]; tj = move[1];
        
        doMove(board, i, j, ti, tj, true);
        localScore = bestScore(board, nextScore(score, opId), depth, opId, ti, tj);
        
        if(bScore == -1 || betterScore(bScore, localScore, opId)){
            bScore = localScore;
            if(answer != nullptr){
                answer[0] = ti; answer[1] = tj;
            }
        }
        undoMove(board, i, j, ti, tj, true);
    }
    return bScore;
}

void freeList(list<int *> *l){
    for(auto it = l->begin();it!=l->end();++it){
        delete[] *it;
    }
    delete l;
}

int computeBest(Board *board, char score, char depth, char opId, int *answer){
    //check depth
    if(depth == 0){
        //then return solution
        return score;
    }
    
    int i,j;
    int *coord;
    int localScore, bestSc = -1;
    
    list<int *> *mandatory = new list<int *>;
    listOfScoringMovables(mandatory, board, opId);
    //build list of movable scoring pieces
    
    int subAnswer[2];
    
    //if we have any, we NEED to choose one of those
    if(mandatory->size()>0){
        //BEST MANDATORY SCORING MOVE
        for(auto it = mandatory->begin();it!=mandatory->end();++it){
            coord = *it;
            i = coord[0]; j = coord[1];
            
            localScore = bestScore(board, score, depth, opId, i, j, subAnswer);
            
            if(bestSc == -1 || betterScore(bestSc, localScore, opId)){
                bestSc = localScore;
                if(answer != nullptr){
                    answer[0] = i; answer[1] = j;
                    answer[2] = subAnswer[0]; answer[3] = subAnswer[1];
                }
            }
        }
        freeList(mandatory);
    }
    else{ //no scoring move!
        //BEST FREE MOVEMENT (non-scoring)
        freeList(mandatory);
        char id;
        int ti,tj;
        for(i=0;i<SIZE;i++){
            for(j=0;j<SIZE;j++){
                id = playerId(board->getPieceAtCoord(i, j));
                if(id == opId){
                    //Get normal moves for each piece
                    mandatory = getListOfNormalMoves(board, i, j, id);
                    for(auto it = mandatory->begin();it!=mandatory->end();++it){
                        coord = *it;
                        ti = coord[0]; tj = coord[1];
                        
                        doMove(board, i, j, ti, tj);
                        localScore = computeBest(board, score, depth-1, nextPlayer(opId));
                        undoMove(board, i, j, ti, tj);
                        
                        if(bestSc == -1 || betterScore(bestSc, localScore, opId)){
                            bestSc = localScore;
                            if(answer != nullptr){
                                answer[0] = i; answer[1] = j;
                                answer[2] = ti; answer[3] = tj;
                            }
                        }
                    }
                    freeList(mandatory);
                }
            }
        }
    }
    return bestSc;
}

AIState::AIState(Board *board){
    this->board = board;
}

int AIState::solution(int depth, int *move){
    return computeBest(this->board, 100, depth, ID_AI, move);
}

/*Another naming convention to account for piece stacks
 Interpret bits:
    a1 a2 ... a12 b1 b2 b3 b4 flip op has
 a: bits for piecestack
 b: compose a number b1<<3 + b2<<2 + b3<<1 + b4<<0 = number of pieces in stack
 */
