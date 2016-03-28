#include "data.h"
#include <iostream>
#include <fstream>
#include <string>

int M_SIZE = 8;
int P = 4;
int H = M_SIZE/P;

RowVectorXd A, B, C;
MatrixXd MO, MK;
double a, b;

CRITICAL_SECTION CS_1;
HANDLE Evt_1, Evt_2, Sem_1, Sem_2, Sem_3;
HANDLE Mut_1;
typedef DWORD WINAPI (*FP)(LPVOID);

DWORD WINAPI F1(LPVOID lparam);
DWORD WINAPI F2(LPVOID lparam);
DWORD WINAPI F3(LPVOID lparam);
DWORD WINAPI F4(LPVOID lparam);

MatrixXd inputMatrix();
RowVectorXd inputVector();
double inputScalar();
MatrixXd sortMatrix(MatrixXd m);

HANDLE* initThreads(int size = 8)
{
	M_SIZE = size;
	H = M_SIZE / P;
	
	HANDLE* T = new HANDLE[P];
	FP F[] = {&F1, &F2, &F3, &F4};
	HANDLE* S[] = {&Sem_1, &Sem_2, &Sem_3};
	HANDLE* E[] = {&Evt_1, &Evt_2};
	
	cout << "Matrix and vector size: ";
    cout << M_SIZE << endl;
	
	for(int i = 0; i < 2; i++)
	{
		*(E[i]) = CreateEvent(NULL, true, false, NULL);
	}
	
	for(int i = 0; i < 3; i++)
	{
		*(S[i]) = CreateSemaphore(NULL, 0, 1, NULL);
	}
	InitializeCriticalSection(&CS_1);

	Mut_1 = CreateMutex(NULL, false, NULL);
	
	for(int i = 0; i < P; i++)
	{
		T[i] = CreateThread(
        NULL,
        20000000,
        F[i],
        NULL,
        0,
        NULL);
	}
	
	return T;
}

//Th = aBh + bCh*(MO*MKh)
void calcTh(
		RowVectorXd &T,
		const double &a, 
		const RowVectorXd &B, 
		const double &b,
		const RowVectorXd &C,
		const MatrixXd &MO,
		const MatrixXd &MK,
		int k)
{ 
	int i = k-1;
	int size = H;
	if(k == P)
	{
		size = M_SIZE - i*H;
	}
	T.segment(i*H, size) = 
		a * B.segment(i*H, size) + 
		b * (C * (MO * MK.block(0, i*H, M_SIZE, size)));
}

////////////////////////////  TASK 1 ///////////////////////////////////
DWORD WINAPI F1(LPVOID lparam)
{
	int threadNo = 1;
	double a1, b1;
	MatrixXd MO1;
	VectorXd C1;
	
	WaitForSingleObject(Mut_1, INFINITE);
		cout << "Thread " << threadNo << " started" << endl;
	ReleaseMutex(Mut_1);
	
	//Ввод A, B, MO
	A = RowVectorXd::Zero(M_SIZE);
	B = inputVector();
	MO = inputMatrix();
	//Cигнал о завершении ввода S2,3,4.1 (Evt_1)
	SetEvent(Evt_1);
	//Ожидание ввода в T4 W4.1 (Evt_2)
	WaitForSingleObject(Evt_2, INFINITE);
	//Вход в КC
	EnterCriticalSection(&CS_1);
		//Копирование ОР a->a1, b->b1, MO->MO1
		a1 = a;	b1 = b;	MO1 = MO; C1 = C;
	//Выход из КC
	LeaveCriticalSection(&CS_1);	
	//Cчет Ah
	calcTh(A, a1, B, b1, C1, MO1, MK, 1);
	//Cортировка Ah
	auto begin = A.segment(0, H).data();
	sort(begin, begin+H);
	//Ожидание завершения Cортировки в Т2 W2.1 (Sem_1)
	WaitForSingleObject(Sem_1, INFINITE);
	//Cлияние первой половины A
	auto begin2 = A.segment(0, 2*H).data();
	inplace_merge(begin2, begin2 + H, begin2 + 2*H);
	//Ожидание Cлияния второй половины А, W4.2 (Sem_3)
	WaitForSingleObject(Sem_3, INFINITE);
	//Cлияние вектора А
	auto begin3 = A.data();
	inplace_merge(begin3, begin3 + 2*H, begin3 + M_SIZE);
	//Вывод вектора А
	WaitForSingleObject(Mut_1, INFINITE);
	if (M_SIZE < 9) cout << A << endl;
	ReleaseMutex(Mut_1);	
	WaitForSingleObject(Mut_1, INFINITE);
	cout << "Thread " << threadNo << " finished" << endl;
	ReleaseMutex(Mut_1);	
	return true;
}
////////////////////////////  TASK 2 ///////////////////////////////////
DWORD WINAPI F2(LPVOID lparam)
{
	int threadNo = 2;
	double a2, b2;
	MatrixXd MO2;	
	VectorXd C2;
	
	WaitForSingleObject(Mut_1, INFINITE);
	cout << "Thread " << threadNo << " started" << endl;
	ReleaseMutex(Mut_1);
	
	//Ожидание ввода в Т1 W1.1 (Evt_1)
	WaitForSingleObject(Evt_1, INFINITE);
	//Ожидание ввода в T4 W4.1 (Evt_2)
	WaitForSingleObject(Evt_2, INFINITE);
	//Вход в КC
	EnterCriticalSection(&CS_1);
		//Копирование ОР a->a1, b->b1, MO->MO1
		a2 = a;	b2 = b;	MO2 = MO; C2 = C;
	//Выход из КC
	LeaveCriticalSection(&CS_1);
	//Cчет Ah
	calcTh(A, a2, B, b2, C2, MO2, MK, 2);
	//Cортировка Ah	
	auto begin = A.segment(H, H).data();
	sort(begin, begin+H);
	//Cигнал о Cортировке Ah S1.2 (Sem_1)
	ReleaseSemaphore(Sem_1, 1, NULL);

	WaitForSingleObject(Mut_1, INFINITE);	
	cout << "Thread " << threadNo << " finished" << endl;
	ReleaseMutex(Mut_1);
	return true;
}
////////////////////////////  TASK 3 ///////////////////////////////////
DWORD WINAPI F3(LPVOID lparam)
{
	int threadNo = 3;
	double a3, b3;
	MatrixXd MO3;	
	VectorXd C3;
	
	WaitForSingleObject(Mut_1, INFINITE);
	cout << "Thread " << threadNo << " started" << endl;
	ReleaseMutex(Mut_1);
	
	//Ожидание ввода в Т1 W1.1 (Evt_1)
	WaitForSingleObject(Evt_1, INFINITE);
	//Ожидание ввода в T4 W4.1 (Evt_2)
	WaitForSingleObject(Evt_2, INFINITE);
	//Вход в КC
	EnterCriticalSection(&CS_1);
		//Копирование ОР a->a1, b->b1, MO->MO1
		a3 = a; b3 = b;	MO3 = MO; C3 = C;
	//Выход из КC
	LeaveCriticalSection(&CS_1);
	//Cчет Ah
	calcTh(A, a3, B, b3, C3, MO3, MK, 3);
	//Cортировка Ah	
	auto begin = A.segment(2*H, H).data();
	sort(begin, begin+H);
	//Cигнал о Cортировке Ah S4.2 (Sem_2)	
	ReleaseSemaphore(Sem_2, 1, NULL);
	
	WaitForSingleObject(Mut_1, INFINITE);
	cout << "Thread " << threadNo << " finished" << endl;
	ReleaseMutex(Mut_1);
	return true;
}
////////////////////////////  TASK 4 ///////////////////////////////////
DWORD WINAPI F4(LPVOID lparam)
{
	int threadNo = 4;
	double a4, b4;
	MatrixXd MO4;		
	VectorXd C4;
	
	WaitForSingleObject(Mut_1, INFINITE);
	cout << "Thread " << threadNo << " started" << endl;
	ReleaseMutex(Mut_1);	
	//Ввод a, b, C, MK
	a = inputScalar();
	b = inputScalar();
	C = inputVector();
	MK = inputMatrix();
	//Cигнал о завершении ввода S1,2,3.2 (Evt_2)
	SetEvent(Evt_2);
	//Ожидание ввода в Т1 W1.1 (Evt_1)
	WaitForSingleObject(Evt_1, INFINITE);
	//Вход в КC
	EnterCriticalSection(&CS_1);
		//Копирование ОР a->a1, b->b1, MO->MO1
		a4 = a;	b4 = b;	MO4 = MO; C4 = C;
	//Выход из КC
	LeaveCriticalSection(&CS_1);
	//Cчет Ah
	calcTh(A, a4, B, b4, C4, MO4, MK, 4);
	//Cортировка Ah
	auto begin = A.segment(3*H, (M_SIZE - 3*H)).data();
	sort(begin, begin+(M_SIZE - 3*H));
	//Ожидание завершения Cортировки в Т3 W3.1 (Sem_2)
	WaitForSingleObject(Sem_2, INFINITE);
	//Cлияние второй половины A
	auto begin2 = A.segment(2*H, (M_SIZE - 2*H)).data();
	inplace_merge(begin2, begin2 + H, begin2 + (M_SIZE - 2*H));
	//Cигнал о Cлиянии второй половины А, S1.3 (Sem_3)
	ReleaseSemaphore(Sem_3, 1, NULL);

	WaitForSingleObject(Mut_1, INFINITE);	
	cout << "Thread " << threadNo << " finished" << endl;
	ReleaseMutex(Mut_1);
	return true;
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

RowVectorXd inputVector()
{
    RowVectorXd res;
    #ifdef M_VALUE
    res = RowVectorXd::Constant(M_SIZE, M_VALUE);
    #endif
    #ifdef M_RANDOM
	srand(time(0));
    res = RowVectorXd::Random(M_SIZE);
    res = (res + RowVectorXd::Constant(M_SIZE, 1.2)) * 50;
    #endif
    return res;
}

double inputScalar()
{
	double res;
	#ifdef M_VALUE
    res = M_VALUE;
    #endif
    #ifdef M_RANDOM
    res = 1 + rand() % 100;
    res = res / ((double) (1 + rand() % 10));
    #endif
    return res;
}