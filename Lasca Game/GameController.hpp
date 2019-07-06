//
//  GameController.hpp
//  GlTest
//
//  Created by Pietro Ribeiro Pepe on 20/05/19.
//  Copyright © 2019 Pietro Ribeiro Pepe. All rights reserved.
//

#ifndef GameController_hpp
#define GameController_hpp

//#include <stdio.h>
#include <list>

#include "Board.hpp"
#include "SelectionState.hpp"
#include "AIState.hpp"

class GameController{
    static GameController *instance;
    Board *board;
    AIState *aiControl;
    SelectionState *selection;
    GameController();
    void SetPlayerTurn(int playerId);
    bool busy = false;
    
    friend bool onMoveCall( PieceStack *piece, int targetRow, int targetCol);
    friend list<tuple<int,int>>* possibleMovements( PieceStack *piece, int row, int col, int pId );
    friend bool hasAdditionalMovements( PieceStack *piece, int row, int col, int pId );
    friend bool playerHasMovement( int pId );
    
public:
    static GameController *getInstance();
    void init(int width, int height);
    void display();
    void mouseClick(int button, int state, int x, int y);
    void windowReshape(int width, int height);
    void performAIsolution();
    void setAI(int delay);
    void setPlayerTurn(int playerId);
    void interactionOn();
};

#endif /* GameController_hpp */
