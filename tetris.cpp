/*
Ursa Ursic, 2022

This is probably a very messy code for a Tetris game. I coded this as a tool to 
get more familiar with C++. This is in no way ideal, but I had a lot of fun 
coding this. 

TODO:
[] detete full lines
[] make this more organized 
[] make the code simpler

*/


// standard libraries
#include <iostream>
#include <list>
#include <random>
#include <map>
#include <vector>
#include <iterator>
#include <Eigen/Dense>
#include <thread>
#include <chrono>
#include <SFML/Window/Keyboard.hpp>
#include <stdlib.h>

// hpp files
#include "shapes.hpp"

// namespaces 
using namespace std;
using namespace Eigen;



// display size is (20, 10):
typedef Matrix <int, 20, 10> DisplayMatrix;

// all shapes I want to use, global vector
vector<char> allShapeNames = {'o', 'i', 's', 'z', 't', 'l', 'j'};

//map shape names and shape definitions
map<char, MatrixXd> mapofShapes;

enum key {
  Q, Z, Left, Right, Down
};

// ---------------------------------------my_functions----------------------------------------

int getRandomNumber(int start, int stop) {
    // Seed with a real random value, if available
    std::random_device r;
 
    // Choose a number mean between 0 and 9
    std::default_random_engine e1(r());
    std::uniform_int_distribution<int> uniform_dist(start, stop);
    int rnd = uniform_dist(e1);
    return rnd;
}

void makeShapeMap() {
     //connecting shape names with shape definitions and inserting them into a map
    mapofShapes.insert(make_pair('o', shapeO));
    mapofShapes.insert(make_pair('i', shapeI));
    mapofShapes.insert(make_pair('s', shapeS));
    mapofShapes.insert(make_pair('z', shapeZ));
    mapofShapes.insert(make_pair('t', shapeT));
    mapofShapes.insert(make_pair('l', shapeL));
    mapofShapes.insert(make_pair('j', shapeJ));
}

void clear() {
    // CSI[2J clears screen, CSI[H moves the cursor to top-left corner
    cout << "\x1B[2J\x1B[H";
}

void printMapofShapes(map<char, MatrixXd> maptoPrint) {
    //Prints the whole map. Useful for testing.
    map<char, MatrixXd>::iterator it_map;
    cout << "\nThe map mapofShapes is : \n";
    cout << "\tKEY\tELEMENT\n";
    for (it_map = maptoPrint.begin(); it_map != maptoPrint.end(); ++it_map) {
        cout << '\t' << it_map->first << "\t";
        for (int i=0; i < it_map->second.rows(); ++i) {
            for (int j=0; j < it_map->second.cols(); ++j) {
                cout << it_map->second(i, j);
            }
            cout << endl;
            cout << "\t\t";
        }
        cout << "\n";
    }
    cout << endl;
}

// ---------------------------------------my_classes-----------------------------------------

class Game {
    private:
        MatrixXd activeShape; 
        int randomStart; 
        vector<int> activeShapePosition;
        bool activeHit = false;
        DisplayMatrix occupiedDisplay;
    public:
        bool gameOver = false;
        DisplayMatrix display;

    Game(){
        for (int i =0; i<display.rows(); ++i) {
            for (int j=0; j<display.cols(); ++j) {
                occupiedDisplay(i, j) = 0;
                // display(i, j) = 0; // should not be necessary
            }
        }
        updateActiveShape();
        activeShapeOnDisplay();
    }

    void updateActiveShape() {
        int pickShapeIdx = getRandomNumber(0, allShapeNames.size());
        activeShape = mapofShapes[allShapeNames[pickShapeIdx]];
        randomStart = getRandomNumber(0, 9-activeShape.cols());
        activeShapePosition = {0, randomStart};
    }

    void activeShapeOnDisplay() {
        // this function combines the active shape and the current "occupied display"
        display = occupiedDisplay;
        for (int i=0; i<activeShape.rows(); ++i) {
            for (int j=0; j<activeShape.cols(); ++j) {
                display(i + activeShapePosition[0], j + activeShapePosition[1]) += activeShape(i, j);
                // check if we have overlaps
                if (display(i + activeShapePosition[0], j + activeShapePosition[1]) > 1) {
                    clear();
                    cout << "Something went wrong" << endl; 
                    exit(2);
                }
            }
        }
    }

    bool doesShapeFit(DisplayMatrix occupiedDisplay, MatrixXd activeShape, vector<int> activeShapePosition) {
        if (activeShapePosition[0] <= display.rows()-activeShape.rows() && activeShapePosition[1] >=0 && activeShapePosition[1] <= display.cols()-activeShape.cols()) { 
            for (int i=0; i<activeShape.rows(); ++i) {
                for (int j=0; j<activeShape.cols(); ++j) {
                    if (occupiedDisplay(activeShapePosition[0]+i, activeShapePosition[1]+j) == 1 && activeShape(i, j) == 1) return false;
                }
            }
            return true;
        }
        return false;
    }

    void rotate() {
        // add the corrections to activeShapePosition
        int NofRows = int(activeShape.rows());
        int NofCols = int(activeShape.cols());
        MatrixXd rotatedMatrix(NofCols, NofRows);
        for (int i = 0; i < NofRows; i++) {
            for (int j = 0; j < NofCols; j++) {
                rotatedMatrix(j, i) = activeShape(NofRows - i - 1, j);
            }
        }
        // add a different activeShapePosition when rotating
        if (doesShapeFit(occupiedDisplay, rotatedMatrix, activeShapePosition)) activeShape = rotatedMatrix;
    }

    void moveL() {
        vector<int> tryPosition = {activeShapePosition[0], activeShapePosition[1]-1};
        if (activeShapePosition[1]<=display.cols()-activeShape.cols() && activeShapePosition[1]>0 && doesShapeFit(occupiedDisplay, activeShape, tryPosition)) activeShapePosition = tryPosition;
    }

    void moveR() {
        vector<int> tryPosition = {activeShapePosition[0], activeShapePosition[1]+1};
        if (activeShapePosition[1]<display.cols()-activeShape.cols() && activeShapePosition[1]>=0 && doesShapeFit(occupiedDisplay, activeShape, tryPosition)) activeShapePosition = tryPosition;
    }

    void moveDown() {
        vector<int> tryPosition = {activeShapePosition[0]+1, activeShapePosition[1]};
        
        if (activeShapePosition[0] < display.rows()-activeShape.rows() && doesShapeFit(occupiedDisplay, activeShape, tryPosition)) {
            activeShapePosition = tryPosition;
        }
        else {
            activeHit = true;
            for (int i=0; i<activeShape.rows(); ++i) {
                for (int j=0; j<activeShape.cols(); ++j) {
                    occupiedDisplay(activeShapePosition[0] +i, activeShapePosition[1] + j) += activeShape(i, j);
                }
            } 
        } 
    }
    
    
    void update(int loop_time) {
        /*
        TODO:
        [] check if a line is full and move everything down
        */
        if (loop_time % 5 == 0) {
            moveDown();
            loop_time = 0;
        }
        if (activeHit) {
            updateActiveShape();
            if (!doesShapeFit(occupiedDisplay, activeShape, activeShapePosition)) {
                gameOver = true;
            }
            activeHit = false;
        }
    }

    void show() {
        if (!gameOver) {
            clear();
            activeShapeOnDisplay();
        }
        cout << "###TETRIS###" << endl;
        for (int i =0; i<display.rows(); ++i) {
            cout << "#";
            for (int j=0; j<display.cols(); ++j) {
                if (display(i, j) == 1) {
                    cout << 'X'; 
                }
                else cout << ' '; 
            }
            cout << "#\n";
        }
        cout << "############" << endl;
    }
};


// ---------------------------------------main------------------------------------------------
int main() { 
    this_thread::sleep_for(5s);  
    system("clear");
    makeShapeMap(); 

    Game tetris = Game();
    tetris.show();

    int loop_time = 0;
    while (!tetris.gameOver) {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q)) break;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z)) tetris.rotate();
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) tetris.moveL();
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) tetris.moveR();
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) tetris.moveDown();
        
        tetris.update(loop_time);
        if (tetris.gameOver) break;
        tetris.show();
        this_thread::sleep_for(75ms);
        ++loop_time;   
    }
    clear();

    tetris.show();
    cout << "GAME OVER!" << endl;
}