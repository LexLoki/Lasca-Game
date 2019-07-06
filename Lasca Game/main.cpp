//
//  main.cpp
//  GlTest
//
//  Created by Pietro Ribeiro Pepe on 08/04/19.
//  Copyright © 2019 Pietro Ribeiro Pepe. All rights reserved.
//

#include <GLUT/GLUT.h>
#include "GameController.hpp"

void display(){
    GameController::getInstance()->display();
}

void mouseClick(int button, int state, int x, int y){
    GameController::getInstance()->mouseClick(button, state, x, y);
}

void reshapeFunc(int width, int height){
    GameController::getInstance()->windowReshape(width, height);
}

int main(int argc, char * argv[]) {
    // insert code here...
    glutInit(&argc, argv);
    
    GameController *control = GameController::getInstance();
    control->init(600, 600);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glutMouseFunc(mouseClick);
    glutDisplayFunc(display);
    glutReshapeFunc(reshapeFunc);
    
    glutMainLoop();
    return 0;
}
