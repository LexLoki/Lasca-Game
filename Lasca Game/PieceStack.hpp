//
//  PieceStack.hpp
//  GlTest
//
//  Created by Pietro Ribeiro Pepe on 17/05/19.
//  Copyright © 2019 Pietro Ribeiro Pepe. All rights reserved.
//

#ifndef PieceStack_hpp
#define PieceStack_hpp

//#include <stdio.h>
//#include <iostream>
#include <list>
//#include <stack>

#include "Piece.hpp"

using namespace std;

class PieceStack{
private:
    //stack <Piece> pieces;
    list<Piece*> pieces;
    //stack<Piece*> pieces;
public:
    int x,y;
    PieceStack(int id, int x, int y);
    int getId();
    tuple<int,int> getCoordinate();
    
    int pieceCount();
    void includePiece(Piece *piece);
    bool hasPiece();
    void flip();
    bool isFlipped();
    Piece *removePiece();
    
    friend class Board;
    
    //For AI access
    friend Piece *removeLastPiece(PieceStack *ps);
    friend void addLastPiece(PieceStack *ps, Piece *p);
};

#endif /* PieceStack_hpp */
