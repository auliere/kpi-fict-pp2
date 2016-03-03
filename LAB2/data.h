#ifndef DATA_H
#define DATA_H
#define M_VALUE 1
#define M_SIZE 3000
#include <windows.h>
#include <ctime>
#include "Eigen/Dense"

using namespace std;
using namespace Eigen;

DWORD WINAPI F1(LPVOID lparam);
DWORD WINAPI F2(LPVOID lparam);
DWORD WINAPI F3(LPVOID lparam);

// F1: d = (B*C) + (A*B) + (C * (B * (MA*MD)))
double f1(VectorXd A, VectorXd B, VectorXd C, MatrixXd MA, MatrixXd MD);

// F2: MF = SORT(TRANS(MK)*MN)
MatrixXd f2(MatrixXd MK, MatrixXd MN);

// F3: s = MAX(SORT(MS) + MX*MT)
double f3(MatrixXd MS, MatrixXd MX, MatrixXd MT);

MatrixXd inputMatrix();
VectorXd inputVector();
void sort(VectorXd* v);
MatrixXd sortMatrix(MatrixXd m);


#endif
