//
//  GameController.cpp
//  GlTest
//
//  Created by Pietro Ribeiro Pepe on 20/05/19.
//  Copyright © 2019 Pietro Ribeiro Pepe. All rights reserved.
//

#include "GameController.hpp"
#include <GLUT/GLUT.h>

#include <iostream>

using namespace std;

/***********************************
 * Internal callbacks and routines *
 ***********************************/

//Move routine
bool onMoveCall( PieceStack *piece, int targetRow, int targetCol);

//Obtain scoring moves
list<tuple<int,int>>* possibleMovements( PieceStack *piece, int row, int col, int pId );
bool hasAdditionalMovements( PieceStack * piece, int row, int col, int pId );

//Check move constraints
bool isMoveAllowed(PieceStack * piece, int row, int col, int tRow, int tCol, int pId);

//Flip piece when appliable
void verifyFlip(PieceStack *piece, int tRow, int nRow);

//Glut callback for AI movement after delay
void timerMove(int par);

/*********************************
 * GameController implementation *
 *********************************/

GameController *GameController::instance = nullptr;

int currentPlayer = 0;

GameController::GameController(){
    board = new Board(7,7);
    aiControl = new AIState(board);
    selection = new SelectionState(board);
    selection->setMoveRequestCallback(onMoveCall);
}

GameController *GameController::getInstance(){
    if(instance == nullptr) instance = new GameController();
    return instance;
}

void GameController::init(int width, int height){
    board->setSize(width, height);
    
    glutInitDisplayMode(GL_DOUBLE | GLUT_RGBA | GLUT_ALPHA);
    //glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(width, height);
    glutCreateWindow("Lasca Game");
    
    board->init();
}

void GameController::windowReshape(int width, int height){
    board->setSize(width, height);
}

void GameController::mouseClick(int button, int state, int x, int y){
    if(button == GLUT_LEFT_BUTTON){
        if(state == GLUT_DOWN){
            cout << x << ", " << y << endl;
            board->getPieceAtPos(x, y);
        }
    }
    if(!busy)
        selection->handleMouse(button, state, x, y);
}

void GameController::display(){
    board->display();
}

void GameController::interactionOn(){
    busy = false;
}

void GameController::performAIsolution(){
    int move[4];
    move[0] = -1;
    aiControl->solution(8, move);
    if(move[0] != -1)
        onMoveCall(board->getPieceAtCoord(move[0], move[1]), move[2], move[3]);
}

void GameController::setAI(int delay){
    busy = true;
    glutTimerFunc(delay, timerMove, 0);
}

void GameController::SetPlayerTurn(int playerId){
    currentPlayer = playerId;
    selection->setCurrentPlayer(playerId);
    if(playerId == 1){
        cout << "AI TURN" << endl;
        busy = true;
        setAI(1000);
    }
    else{
        cout << "PL TURN" << endl;
        //check mandatory movement
        tuple<int,int> coord = board->getSize();
        list<tuple<int,int>> *movs;
        int nCol = get<0>(coord), nRow = get<1>(coord), i, j;
        PieceStack *ps;
        selection->allowNone();
        bool found = false;
        for(i=0;i<nRow;i++)
            for(j=0;j<nCol;j++){
                ps = board->getPieceAtCoord(i, j);
                if(ps != nullptr && ps->getId() == playerId){
                    movs = possibleMovements(ps, i, j, playerId);
                    if(!movs->empty()){
                        found = true;
                        selection->allowCoordinate(j, i);
                    }
                    delete movs;
                }
                
            }
        cout << found << endl;
        if(!found)
            selection->allowAll();
    }
}



/***************************
 * Internal implementation *
 ***************************/


void timerMove(int par){
    GameController *control = GameController::getInstance();
    control->performAIsolution();
    control->interactionOn();
}

void verifyFlip(PieceStack *piece, int tRow, int nRow){
    if(piece->getId() == 0){
        if(tRow == nRow-1)
            piece->flip();
    }
    else if(tRow == 0)
        piece->flip();
}

bool hasAdditionalMovements( PieceStack * piece, int row, int col, int pId ){
    list<tuple<int,int>>* movs = possibleMovements(piece, row, col, pId);
    bool hasMove = !(movs->empty());
    delete movs;
    return hasMove;
}

//check if move is a restricted mandatory move
bool isMoveAllowed(PieceStack * piece, int row, int col, int tRow, int tCol, int pId){
    list<tuple<int,int>>* movs = possibleMovements(piece, row, col, pId);
    bool canMove = false;
    if(movs->empty()){
        canMove = true;
    }else{
        tuple<int,int> tup;
        for(auto it = movs->begin(); it != movs->end(); it++){
            tup = (*it);
            if(get<0>(tup) == tCol && get<1>(tup) == tRow){
                canMove = true;
                break;
            }
        }
    }
    delete movs;
    return canMove;
}

//get possible scoring positions for a piece
list<tuple<int,int>>* possibleMovements( PieceStack *piece, int row, int col, int pId ){
    list<tuple<int,int>> *l = new list<tuple<int,int>>();
    Board *board = GameController::getInstance()->board;
    tuple<int,int> dim = board->getSize();
    int nCol = get<0>(dim), nRow = get<1>(dim);
    list<int> dirs;
    if(piece->isFlipped()){
        dirs.push_back(1);
        dirs.push_back(-1);
    }
    else{
        dirs.push_back(1 - 2*pId); //verify
    }
    int r,c,i,tR,tC;
    int dirH[2] = {-1,1};
    PieceStack *midPs;
    //cout << "Checking sides, center: " << row << ", " << col << endl;
    for(auto it = dirs.begin(); it!=dirs.end(); ++it){
        int dir = (*it);
        r = row + 2*dir;
        if(r >= 0 && r < nRow){
            for(int i=0;i<2;i++){
                c = col + 2*dirH[i];
                if(c<nCol && c>=0){
                    tR = (row+r)/2;
                    tC = (col+c)/2;
                    //cout << "Coords" << tR << ", " << tC << endl;
                    midPs = board->getPieceAtCoord(tR, tC);
                    //if(midPs == nullptr) cout << " -> no piece" <<endl;
                    //else cout << "id: " << midPs->getId() << endl;
                    if(board->getPieceAtCoord(r,c) == nullptr && midPs != nullptr && midPs->getId() != pId){
                        cout << row << col << r << c << endl;
                        l->push_back(make_tuple(c,r));
                    }
                }
            }
        }
    }
    return l;
}

bool onMoveCall( PieceStack *piece, int targetRow, int targetCol){
    GameController * control = GameController::getInstance();
    tuple<int,int> coord = piece->getCoordinate();
    int col = get<0>(coord), row = get<1>(coord);
    int nRow = get<1>(control->board->getSize());
    
    int rowDist = targetRow-row;
    int colDist = targetCol-col;
    Board *board = control->board;
    
    //Check move restriction
    if(!isMoveAllowed(piece, row, col, targetRow, targetCol, currentPlayer))
        return false;
    
    //Move direction restriction
    bool canMoveDir = piece->isFlipped() || (rowDist/abs(rowDist) == 1 - 2*currentPlayer);
    
    //Single movement
    if(abs(rowDist) == 1 && abs(colDist) == 1 && canMoveDir){
        cout << "possible movement" << endl;
        //perform movement
        board->movePiece(piece, targetRow, targetCol);
        verifyFlip(piece, targetRow, nRow);
        control->SetPlayerTurn((currentPlayer+1)%2);
        //disable selection stage
        return true;
    }
    //Scoring movement
    else if(abs(rowDist) == 2 && abs(colDist) == 2 && canMoveDir){
        rowDist = (row+targetRow)/2;
        colDist = (col+targetCol)/2;
        PieceStack *midPs = board->getPieceAtCoord(rowDist, colDist);
        if(midPs != nullptr && midPs->getId() != currentPlayer){
            //eat piece
            piece->includePiece(board->removePiece(rowDist, colDist));
            //move
            board->movePiece(piece, targetRow, targetCol);
            //flip
            verifyFlip(piece, targetRow, nRow);
            if(hasAdditionalMovements(piece, targetRow, targetCol, currentPlayer)){
                cout << "can go on" << endl;
                if(currentPlayer == 1){
                    glutPostRedisplay(); //to guarantee refresh between movements
                    control->setAI(500);
                }
                else{
                    //force player to keep moving with piece
                    control->selection->setForceMode(piece);
                }
            }
            else{
                //no more moves, next player
                control->SetPlayerTurn((currentPlayer+1)%2);
            }
            return true;
        }
    }
    cout << "forbidden movement" << endl;
    return false;
}
