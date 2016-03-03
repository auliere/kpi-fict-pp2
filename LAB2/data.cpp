#include "data.h"
#include <iostream>
#include <fstream>

DWORD WINAPI F1(LPVOID lparam)
{
    cout << "Thread T1 started\n";

    srand(time(0) * 1000);
    ofstream f;
    f.open("F1.out");
    VectorXd A = inputVector(), B = inputVector(), C = inputVector();
    MatrixXd MA = inputMatrix(), MD = inputMatrix();
    double d;

    d = f1(A, B, C, MA, MD);
    f << "F1 = " << d << "\n";

    f.close();
    cout << "Thread T1 finished\n";
    return 0;
}

DWORD WINAPI F2(LPVOID lparam)
{
    cout << "Thread T2 started\n";

    ofstream f;
    f.open("F2.out");

    srand(time(0) * 1000);
    MatrixXd MK = inputMatrix(), MN = inputMatrix();
    MatrixXd MF = f2(MK, MN);

    f << "F2 = \n" << MF << endl;

    cout << "Thread T2 finished\n";
    return 0;
}

DWORD WINAPI F3(LPVOID lparam)
{
    cout << "Thread T3 started\n";

    ofstream f;
    f.open("F3.out");

    MatrixXd MS = inputMatrix(), MX = inputMatrix(), MT = inputMatrix();
    double s = f3(MS, MX, MT);

    f << "F3 = " << s << endl;

    cout << "Thread T3 finished\n";
    return 0;
}

// F1: d = (B*C) + (A*B) + (C * (B * (MA*MD)))
double f1(VectorXd A, VectorXd B, VectorXd C, MatrixXd MA, MatrixXd MD)
{
    double d = 0;
    d = B.dot(C) + A.dot(B) + C.dot(B.adjoint() * (MA * MD));
    return d;
}

// F2: MF = SORT(TRANS(MK)*MN)
MatrixXd f2(MatrixXd MK, MatrixXd MN)
{
    MatrixXd MF;
    MF = sortMatrix(MK.transpose()*MN);
    return MF;
}

// F3: s = MAX(SORT(MS) + MX*MT)
double f3(MatrixXd MS, MatrixXd MX, MatrixXd MT)
{
    double s = 0;
    s = (sortMatrix(MS) + MX * MT).maxCoeff();
    return s;
}

MatrixXd inputMatrix()
{
    MatrixXd res;
    #ifdef M_VALUE
    res = MatrixXd::Constant(M_SIZE, M_SIZE, M_VALUE);
    #endif
    #ifdef M_RANDOM
    res = MatrixXd::Random(M_SIZE, M_SIZE);
    res = (res + MatrixXd::Constant(M_SIZE, M_SIZE, 1.2)) * 50;
    #endif
    return res;
}

VectorXd inputVector()
{
    VectorXd res;
    #ifdef M_VALUE
    res = VectorXd::Constant(M_SIZE, M_VALUE);
    #endif
    #ifdef M_RANDOM
    res = VectorXd::Random(M_SIZE);
    res = (res + VectorXd::Constant(M_SIZE, 1.2)) * 50;
    #endif
    return res;
}

VectorXd sort(VectorXd v)
{
    sort(v.data(), v.data()+v.size());
    return v;
}

MatrixXd sortMatrix(MatrixXd m)
{
    MatrixXd res = MatrixXd::Zero(M_SIZE, M_SIZE);
    for(int i = 0; i <  m.rows(); i++)
    {
        res.row(i) = sort(m.row(i));
    }
    return res.rowwise().reverse();
}
