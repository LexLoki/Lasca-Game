//
//  AIState.hpp
//  Lasca Game
//
//  Created by Pietro Ribeiro Pepe on 20/05/19.
//  Copyright © 2019 Pietro Ribeiro Pepe. All rights reserved.
//

#ifndef AIState_hpp
#define AIState_hpp

#include <stdio.h>

#include "Board.hpp"

class AIState{
    
    //char board[7][7];
    Board *board;
    
    //int point;
    
public:
    
    AIState(Board *board);
    //AIState *getPossibleMoves();
    int solution(int depth, int *move);
    
};

#endif /* AIState_hpp */
