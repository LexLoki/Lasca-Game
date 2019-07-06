//
//  SelectionState.cpp
//  GlTest
//
//  Created by Pietro Ribeiro Pepe on 18/05/19.
//  Copyright © 2019 Pietro Ribeiro Pepe. All rights reserved.
//

#include "SelectionState.hpp"
#include <GLUT/GLUT.h>

//TODO: SET CALLBACK ON CONSTRUCTOR
SelectionState::SelectionState(Board *board){
    this->board = board;
    std::tuple<int,int> tup = board->getSize();
    int nCol = std::get<0>(tup);
    int nRow = std::get<1>(tup);
    allowedCoordinates = new bool *[nCol];
    int i,j;
    for(i=0;i<nCol;i++){
        allowedCoordinates[i] = new bool[nRow];
        for(j=0;j<nRow;allowedCoordinates[i][j++]=true);
    }
    //_instance = this;
}

void SelectionState::setCurrentPlayer(int playerId){
    currentPlayer = playerId;
    triggerSelection(nullptr);
    forceMode = false;
    setAll(true);
    //selected = nullptr;
}

void SelectionState::setMoveRequestCallback(bool (*call)(PieceStack *, int, int)){
    onMoveRequest = call;
}

void SelectionState::setAll(bool state){
    int i,j;
    std::tuple<int,int> tup = board->getSize();
    int nCol = std::get<0>(tup);
    int nRow = std::get<1>(tup);
    for(i=0;i<nCol;i++)
        for(j=0;j<nRow;j++)
            allowedCoordinates[i][j] = state;
}

void SelectionState::allowNone(){
    setAll(false);
}

void SelectionState::allowAll(){
    setAll(true);
}

void SelectionState::allowCoordinate(int x, int y){
    allowedCoordinates[x][y] = true;
}

void SelectionState::triggerSelection(PieceStack *ps){
    selected = ps;
    board->highlightPiece(selected);
    glutPostRedisplay();
}

void SelectionState::setForceMode(PieceStack *ps){
    triggerSelection(ps);
    forceMode = true;
}

bool SelectionState::canSelect(PieceStack *ps){
    //return true;
    tuple<int,int> coord = ps->getCoordinate();
    int x = get<0>(coord), y = get<1>(coord);
    return allowedCoordinates[x][y];
}

void SelectionState::handleMouse(int button, int state, int x, int y){
    if(state == GLUT_DOWN){
        if(button == GLUT_LEFT_BUTTON){
            PieceStack *ps = board->getPieceAtPos(x, y);
            //if(ps != nullptr) std::cout << ps->getId() << std::endl;
            if(ps != nullptr && ps->getId() == currentPlayer){
                if((selected == nullptr || !forceMode) && canSelect(ps))
                    selected = ps;
            }
            else if(selected != nullptr && ps == nullptr){
                std::tuple<int,int> tup = board->getCoordAtPos(x, y);
                int col = std::get<0>(tup);
                int row = std::get<1>(tup);
                if(onMoveRequest(selected,row,col)){
                    //could move
                }
                else{
                    //could not move
                }
            }
        }
        else if(button == GLUT_RIGHT_BUTTON){
            //cancel selection
            if(!forceMode)
                selected = nullptr;
        }
        triggerSelection(selected);
    }
}
