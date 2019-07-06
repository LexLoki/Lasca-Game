//
//  PieceStack.cpp
//  GlTest
//
//  Created by Pietro Ribeiro Pepe on 17/05/19.
//  Copyright © 2019 Pietro Ribeiro Pepe. All rights reserved.
//

#include "PieceStack.hpp"

PieceStack::PieceStack(int id, int x, int y){
    pieces.push_back(new Piece(id));
    //pieces.push(new Piece(id));
    this->x = x;
    this->y = y;
}

int PieceStack::getId(){
    return pieces.front()->getId();
    //return pieces.top()->getId();
}

tuple<int,int> PieceStack::getCoordinate(){
    return std::make_tuple(x, y);
}

void PieceStack::includePiece(Piece *piece){
    pieces.push_back(piece);
    //pieces.push(piece);
}

int PieceStack::pieceCount(){
    return (int)pieces.size();
}

bool PieceStack::hasPiece(){
    return pieceCount()>0;
}

void PieceStack::flip(){
    pieces.front()->flip();
}

bool PieceStack::isFlipped(){
    return pieces.front()->getFlip();
}

Piece *PieceStack::removePiece(){
    Piece *piece = pieces.front();
    pieces.pop_front();
    return piece;
}
