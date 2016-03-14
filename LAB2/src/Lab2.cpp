/**
---------------------------------------------------------
---------------- PARALLEL PROGRAMMING -------------------
---------------------- LAB #2 ---------------------------
- Win32. Semaphores, events, mutex and critical section -
------------ A = sort(a*B + b*C*(MO*MK)) ----------------
-----------------CREATED ON 12.03.2016-------------------
----------------BY OLEG PEDORENKO, IP-31-----------------
---------------------------------------------------------
*/

#include <iostream>
#include <windows.h>
#include "data.h"

using namespace std;

int main(int argc, char** argv)
{
	int size = 8;
	if(argc > 1)
	{
		size = atoi(argv[1]);
	}

    HANDLE* T = initThreads(size);
	WaitForMultipleObjects(4, T, true, INFINITE);
	cout << "All threads finished successfully" << endl;
	return 0;
}
