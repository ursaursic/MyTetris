#include <Eigen/Dense>

using namespace Eigen;

// shapes in Tetris
MatrixXd shapeO {{1, 1}, 
                {1, 1}};

MatrixXd shapeI {{1, 1, 1, 1}};

MatrixXd shapeS {{0, 1, 1}, 
                {1, 1, 0}};

MatrixXd shapeZ {{1, 1, 0}, 
                {0, 1, 1}};

MatrixXd shapeT {{0, 1, 0}, 
                {1, 1, 1}};

MatrixXd shapeL {{1, 0}, 
                {1, 0}, 
                {1, 1}};

MatrixXd shapeJ {{0, 1}, 
                {0, 1}, 
                {1, 1}};