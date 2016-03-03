/**
---------------------------------------------------------
---------------- PARALLEL PROGRAMMING -------------------
---------------------- LAB #4 ---------------------------
----       ------- THREADS IN Win32 ----------      -----
--- F1: d = (B*C) + (A*B) + (C * (B * (MA*MD)))     -----
--- F2: MF = SORT(TRANS(MK)*MN)                     -----
--- F3: s = MAX(SORT(MS) + MX*MT)                   -----
-----------------CREATED ON 20.10.2015-------------------
----------------BY OLEG PEDORENKO, IP-31-----------------
---------------------------------------------------------
*/

#include <iostream>
#include <windows.h>
#include "data.h"

using namespace std;

int main(int argc, char** argv)
{
    cout << "Matrix and vector size:\n";
    cout << M_SIZE << endl;

    HANDLE T1 = CreateThread(
        NULL,
        20000000,
        F1,
        NULL,
        0,
        NULL);

    HANDLE T2 = CreateThread(
        NULL,
        20000000,
        F2,
        NULL,
        0,
        NULL);

    HANDLE T3 = CreateThread(
        NULL,
        20000000,
        F3,
        NULL,
        0,
        NULL);

    HANDLE T[3] = {T1, T2, T3};

    WaitForMultipleObjects(3, T, true, INFINITE);
    return 0;
}
