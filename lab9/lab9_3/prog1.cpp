#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <sys/shm.h>

using namespace std;

int sharedMemPTR; //указатель на РП
bool sharedMemOWNER;
string filename = "out.txt";
int progID = 0;
int key = 420; // ключ РП

struct sharedParams
{
	bool preps[3]; // статусы подготовки
	int priorities[3]; // приоритеты в очереди
};

void controlInput(int argc, char *argv[]){
    if (argc != 3) // проверка кол-ва аргументов (М)
	{
		cout << "ERROR: must be two arguments" << endl;
		exit(-1);
	}
}

void checkStart(){
    cout << "for start press ENTER" << endl;
    getchar();
}

void getSharedMem(int key, bool sharedMemOWNER){
    sharedMemPTR = shmget(key, sizeof(sharedParams), 0666 | IPC_CREAT | IPC_EXCL);

	if (sharedMemPTR != -1)
	{
		sharedMemOWNER = true;
		cout << "shared mem struct CREATED" << endl;
	}
	else
	{
		sharedMemPTR = shmget(key, sizeof(sharedParams), 0666 | IPC_CREAT);
		if(sharedMemPTR == -1)
		{
			cout << "shared mem struct NOT OPENED" << endl;
			exit(-1);
		}
		else
		{
			cout << "shared mem struct OPENED" << endl;
		}
	}
}

void printOut(string sringForPrint, int i){
    cout << "OPEN file " << filename << endl;
    ofstream outFILE(filename, ios_base::app);
    cout << "String => " << i << endl;
    outFILE << sringForPrint;
    outFILE.close();
    cout << "CLOSE file " << filename << endl;
}

int main (int argc, char *argv[])
{
    controlInput(argc, argv);
    cout << "Program ID = " << progID << endl;
    cout << "key = " << key << endl;
    checkStart();

	int interval = atoi(argv[1]);
	int numberOfStrings = atoi(argv[2]);

	int localPriority = -1; //номер текущего приоритета
	string sringForPrint = "Written by program priorities " + to_string(progID) + "\n";
	sharedParams* sharedParamsOBJ; // необходимые для алгоритма данные

	getSharedMem(key, sharedMemOWNER); // создание/открытие разделяемой памяти

	sharedParamsOBJ = (sharedParams*)shmat(sharedMemPTR, 0, 0); // подключение РП к АПП

	for (int i = 0; i < numberOfStrings; i++) // печать всех строк
	{
		sleep(interval); // сон
		sharedParamsOBJ->preps[progID] = true; // обозначение подготовки текущего процесса
		localPriority = -1;

		for (int k = 0; k < 3; k++) // выбор большего приоритета
		{
			if (sharedParamsOBJ->priorities[k] > localPriority)
			{
				localPriority = sharedParamsOBJ->priorities[k];
			}
		}

		sharedParamsOBJ->priorities[progID] = localPriority + 1; // увеличение приоритета
		sharedParamsOBJ->preps[progID] = false; // подготовка окончена

		for(int j = 0; j < 3; j++) // ожидание всех 3-х проессов
		{

			while(sharedParamsOBJ->preps[j] == true) // ожидание когда все выбирут приоритет
			{}

			while((sharedParamsOBJ->priorities[j] != 0)
			&& ((sharedParamsOBJ->priorities[j] < sharedParamsOBJ->priorities[progID]) // ненулевой приоритет, больший приоритета другого
			|| ((sharedParamsOBJ->priorities[j] == sharedParamsOBJ->priorities[progID])
			&& (j < progID)))) // приоритеты одинаковые но номер номер больше
			{}
		}

		sharedParamsOBJ->priorities[progID] = 0; // сброс приоритета
	}

	shmdt((void*)sharedParamsOBJ); // открепление РП от АПП

	if(sharedMemOWNER == true)
	{
		shmctl(sharedMemPTR, IPC_RMID, NULL); // удаление РП
		cout << endl << "shared memory CLOSED" << endl;
	}

	return 0;
}

