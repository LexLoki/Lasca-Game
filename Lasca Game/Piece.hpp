//
//  Piece.hpp
//  GlTest
//
//  Created by Pietro Ribeiro Pepe on 17/05/19.
//  Copyright © 2019 Pietro Ribeiro Pepe. All rights reserved.
//

#ifndef Piece_hpp
#define Piece_hpp

class Piece{
    int id;
    bool isFlipped;
public:
    Piece(int id);
    void flip();
    bool getFlip();
    int getId();
};

#endif /* Piece_hpp */
