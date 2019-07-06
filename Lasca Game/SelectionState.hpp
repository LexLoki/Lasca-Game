//
//  SelectionState.hpp
//  GlTest
//
//  Created by Pietro Ribeiro Pepe on 18/05/19.
//  Copyright © 2019 Pietro Ribeiro Pepe. All rights reserved.
//

#ifndef SelectionState_hpp
#define SelectionState_hpp

#include <stdio.h>
#include "Board.hpp"

class SelectionState{
private:
    Board *board;
    PieceStack *selected = nullptr;
    bool **allowedCoordinates;
    //std::list<std::tuple<int,int>> allowedCoordinates;
    bool forceMode = false;
    int currentPlayer = 0;
    bool canSelect(PieceStack *ps);
    bool (*onMoveRequest)(PieceStack *piece, int targetRow, int targetCol);
    void triggerSelection(PieceStack *ps);
    void setAll(bool state);
    //static SelectionState* _instance;
public:
    //static SelectionState *getInstance();
    SelectionState(Board *board);
    void setForceMode(PieceStack *ps);
    void handleMouse(int button, int state, int x, int y);
    void setCurrentPlayer(int playerId);
    
    void allowCoordinate(int x, int y);
    void allowNone();
    void allowAll();
    
    void setMoveRequestCallback(bool (*call)(PieceStack *piece, int targetRow, int targetCol));
};

#endif /* SelectionState_hpp */
