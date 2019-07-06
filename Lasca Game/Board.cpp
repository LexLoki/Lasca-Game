//
//  Board.cpp
//  GlTest
//
//  Created by Pietro Ribeiro Pepe on 13/05/19.
//  Copyright © 2019 Pietro Ribeiro Pepe. All rights reserved.
//


#include "Board.hpp"

#include <GLUT/GLUT.h>
#include "math.h"

#include <iostream>

using namespace std;

Board::Board(int nRow, int nCol){
    this->nRow = nRow;
    this->nCol = nCol;
    
    int i,j;
    int idx = 0;
    pieces = new PieceStack **[nRow];
    for(i=0;i<nRow;i++){
        pieces[i] = new PieceStack *[nCol];
        for(j=0;j<nCol;j++){
            idx = (idx+1)%2;
            if(i!=nRow/2 && idx) pieces[i][j] = new PieceStack(i<nRow/2 ? 0 : 1, j, i);
            else pieces[i][j] = nullptr;
        }
    }
}

void Board::setPiecePos(PieceStack *ps, int row, int col){
    pieces[row][col] = ps;
    if(ps != nullptr){
        ps->x = col; ps->y = row;
    }
}

void Board::movePiece(PieceStack *ps, int toRow, int toCol){
    int col = ps->x, row = ps->y;
    PieceStack *aux = getPieceAtCoord(toRow, toCol);
    setPiecePos(ps, toRow, toCol);
    setPiecePos(aux, row, col);
}

Piece *Board::removePiece(int row, int col){
    PieceStack *ps = getPieceAtCoord(row, col);
    if(ps == nullptr) return nullptr;
    Piece *p = ps->removePiece();
    if(!ps->hasPiece())
        pieces[row][col] = nullptr;
    return p;
}

tuple<int,int> Board::getSize(){
    return make_tuple(nCol,nRow);
}

void Board::setSize(int w, int h){
    width = w;
    height = h;
}

void Board::init(){
    
}

PieceStack *Board::getPieceAtPos(int x, int y){
    tuple<int,int> coord = getCoordAtPos(x, y);
    int col = get<0>(coord);
    int row = get<1>(coord);
    //cout << row << ", " << col << endl;
    return getPieceAtCoord(row, col);
}

tuple<int,int> Board::getCoordAtPos(int x, int y){
    return make_tuple(x*nCol/width,nRow-1-y*nRow/height);
}

PieceStack *Board::getPieceAtCoord(int row, int col){
    return pieces[row][col];
}

void Board::highlightPiece(PieceStack *ps){
    highlighted = ps;
}

void DrawStar(float cx, float cy, float rx, float ry){
    glBegin(GL_LINE_LOOP);
    float dist = 4*M_PI/5;
    float theta = dist/4;
    for(int idx = 0; idx < 5; idx++){
        glVertex2f(cx + rx*cosf(theta), cy + ry*sinf(theta));
        theta += dist;
        if(theta > 2*M_PI)
            theta -= 2*M_PI;
        //output vertex
    }
    glEnd();
}

void DrawCircle(float cx, float cy, float r, int num_segments) {
    //glBegin(GL_LINE_LOOP);
    glBegin(GL_TRIANGLE_FAN);
    for (int ii = 0; ii < num_segments; ii++)   {
        float theta = 2.0f * 3.1415926f * float(ii) / float(num_segments);//get the current angle
        float x = r * cosf(theta);//calculate the x component
        float y = r * sinf(theta);//calculate the y component
        glVertex2f(x + cx, y + cy);//output vertex
    }
    glEnd();
}
void DrawEllipse(float cx, float cy, float rx, float ry, int num_segments, bool fill) {
    //glBegin(GL_LINE_LOOP);
    glBegin(fill ? GL_TRIANGLE_FAN : GL_LINE_LOOP);
    for (int ii = 0; ii < num_segments; ii++)   {
        float theta = 2.0f * 3.1415926f * float(ii) / float(num_segments);//get the current angle
        float x = rx * cosf(theta);//calculate the x component
        float y = ry * sinf(theta);//calculate the y component
        glVertex2f(x + cx, y + cy);//output vertex
    }
    glEnd();
}

void Board::display(){
    
    glClearColor(1,1,1,0);
    //glClear(GL_COLOR_BUFFER_BIT);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glColor4f(0, 0, 0, 1);
    //glBegin(GL_POINTS);
    glBegin(GL_2D);
    GLfloat x;
    GLfloat minX = -1.0;
    GLfloat maxX = 1.0;
    
    int idx = 0;
    
    int i,j,id,count;
    GLfloat size = (maxX-minX)/nRow;
    PieceStack *ps;
    float cx,cy;
    float pSizeW = 0.3*size, pSizeH = 0.2*size;
    int nSeg = 100;
    for(j=0;j<nRow;j++)
        for(i=0;i<nCol;i++){
            idx = (idx+1)%2;
            glColor4f(idx, idx, idx, 1);
            glRectd(minX+size*i, minX+size*j, minX+size*(i+1), minX+size*(j+1));
                /*
                id = pieces[j][i]->getId();
                glColor3f(id, 0, 1-id);
                glRectd(minX+size*i, minX+size*j, minX+size*(i+0.5), minX+size*(j+0.5));
                */
        }
    
    for(j=nRow-1;j>=0;j--)
        for(i=0;i<nCol;i++){
            if(pieces[j][i] != nullptr){
                ps = pieces[j][i];
                count = 0;
                for(auto it = ps->pieces.rbegin(); it != ps->pieces.rend(); ++it){
                    id = (*it)->getId();
                    glColor3f(id, 0, 1-id);
                    cx = minX+size*(i+0.5);
                    cy = minX+size*(j+0.34+count*0.1);
                    DrawEllipse(cx, cy - size*0.02, pSizeW, pSizeH, nSeg, true);
                    glColor3f(0,0,0);
                    DrawEllipse(cx, cy - size*0.02, pSizeW, pSizeH, nSeg, false);
                    glColor3f(id, 0, 1-id);
                    DrawEllipse(cx, cy + size*0.02, pSizeW, pSizeH, nSeg, true);
                    glRectd(cx-pSizeW, cy - size*0.02, cx+pSizeW, cy + size*0.02);
                    glColor3f(0,0,0);
                    DrawEllipse(cx, cy + size*0.02, pSizeW, pSizeH, nSeg, false);
                    //glRectd(minX+size*(i+count*0.1), minX+size*(j+count*0.1), minX+size*(i+0.5+count*0.1), minX+size*(j+0.5+count*0.1));
                    if((*it)->getFlip())
                        DrawStar(cx,cy + size*0.02,pSizeW/2,pSizeH/2);
                    count++;
                }
            }
        }
    
    if(highlighted != nullptr){
        glColor4f(0, 1, 0, 0.4);
        tuple<int,int> coord = highlighted->getCoordinate();
        int x = get<0>(coord), y = get<1>(coord);
        glRectd(minX+size*x, minX+size*y, minX+size*(x+1), minX+size*(y+1));
    }
    
    glEnd();
    
    
    /*
    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0);
    glVertex3f(-1.0, -1.0, 0.0);
    glTexCoord2f(0.0, 1.0);
    glVertex3f(-1.0, 0.0, 0.0);
    glTexCoord2f(1.0, 1.0);
    glVertex3f(0.0, 0.0, 0.0);
    glTexCoord2f(1.0, 0.0);
    glVertex3f(0.0, -1.0, 0.0);
    glEnd();
     */
    
    
    glFlush();
    glutSwapBuffers();
}
