//
//  Piece.cpp
//  GlTest
//
//  Created by Pietro Ribeiro Pepe on 17/05/19.
//  Copyright © 2019 Pietro Ribeiro Pepe. All rights reserved.
//

#include "Piece.hpp"

Piece::Piece(int id){
    this->id = id;
}

void Piece::flip(){
    isFlipped = true;
}

bool Piece::getFlip(){
    return isFlipped;
}

int Piece::getId(){
    return id;
}
