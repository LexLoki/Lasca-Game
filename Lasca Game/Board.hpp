//
//  Board.hpp
//  GlTest
//
//  Created by Pietro Ribeiro Pepe on 13/05/19.
//  Copyright © 2019 Pietro Ribeiro Pepe. All rights reserved.
//

#ifndef Board_hpp
#define Board_hpp

#include "PieceStack.hpp"

class Board{
private:
    int nRow, nCol;
    int width, height;
    PieceStack ***pieces;
    PieceStack *highlighted = nullptr;
    void setPiecePos(PieceStack *ps, int row, int col);
    
public:
    Board(int nRow, int nCol);
    void init();
    std::tuple<int,int> getSize();
    void setSize(int w, int h);
    PieceStack *getPieceAtCoord(int row, int col);
    PieceStack *getPieceAtPos(int x, int y);
    void movePiece(PieceStack *ps, int toRow, int toCol);
    Piece *removePiece(int row, int col);
    std::tuple<int,int> getCoordAtPos(int x, int y);
    
    //Graphics
    void display();
    void highlightPiece(PieceStack *ps);
    
    
    //AI
    friend void undoMove(Board *board, int i, int j, int targetI, int targetJ, bool scoring);
};

#endif /* Board_hpp */
