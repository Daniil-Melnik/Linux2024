#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <sys/shm.h>

using namespace std;

int progID = 2;

struct sharedParams
{
	bool preps[3];
	int priorities[3];
	bool finished[3];
};

int main (int argc, char *argv[])
{
	int interval = atoi(argv[1]);
	int numberOfStrings = atoi(argv[2]);
	int key;
	string filename = "out.txt";

	bool sharedMemOWNER;
	bool finish1, finish2, finish3;
	int sharedMemPTR;
	int localPRIORITY = -1;
    string stringForFile = "From rpogram " + to_string(progID)  + "\n";
	sharedParams* sharedMemSEG;

	cout << "enter key of shared segment : ";
	cin >> key;

	cout << "program ID = " << progID << endl;
	cout << "interval = " << interval << endl;
	cout << "key = " << key << endl;

	sharedMemPTR = shmget(key, sizeof(sharedParams), 0666 | IPC_CREAT | IPC_EXCL); // создание сегмента

	if (sharedMemPTR != -1)
	{
		sharedMemOWNER = true;
		cout << "shared memory CREATED" << endl;
	}
	else
	{
		sharedMemPTR = shmget(key, sizeof(sharedParams), 0666 | IPC_CREAT);
		if(sharedMemPTR == -1)
		{
			cout << "shared memory ERROR" << endl;
			exit(-1);
		}
		else
		{
			cout << "shared memory OPENED" << endl;
		}
	}

	sharedMemSEG = (sharedParams*)shmat(sharedMemPTR, 0, 0); // подключение РП к АПП
    sharedMemSEG->finished[progID] = false;

	for (int i = 0; i < numberOfStrings; i++)
	{
		sleep(interval); // сон
		sharedMemSEG->preps[progID] = true;
		localPRIORITY = -1;

		for (int k = 0; k < 3; k++)
		{
			if (sharedMemSEG->priorities[k] > localPRIORITY)
			{
				localPRIORITY = sharedMemSEG->priorities[k];
			}
		}

		sharedMemSEG->priorities[progID] = localPRIORITY + 1;
		sharedMemSEG->preps[progID] = false;

		for(int j = 0; j < 3; j++)
		{
			while(sharedMemSEG->preps[j] == true) // контроль входа в КС
			{}

			while((sharedMemSEG->priorities[j] != 0) && ((sharedMemSEG->priorities[j] < sharedMemSEG->priorities[progID]) // есть приоритет и он болше чем у другой - ждём
			|| ((sharedMemSEG->priorities[j] == sharedMemSEG->priorities[progID]) && (j < progID)))) // приоритеты одинаковы => если номер процесса больше то ждём
			{}
		}

		cout << "open file " << filename << endl;
		ofstream local_file(filename, ios_base::app);
		cout << "write # " << i << endl;
		local_file << stringForFile;
		cout << "close file " << filename << endl;
		local_file.close();

		sharedMemSEG->priorities[progID] = 0;
	}

	sharedMemSEG->finished[progID] = true; // открепление РП от АПП

	finish1 = sharedMemSEG->finished[0];
	finish2 = sharedMemSEG->finished[1];
	finish3 = sharedMemSEG->finished[2];

	// cout << finish1 << " " << finish2 << " " << finish3 << endl;

	shmdt((void*)sharedMemSEG);

	if(finish1 && finish2 &&  finish3)
	{
		shmctl(sharedMemPTR, IPC_RMID, NULL); // удаление РП
		cout << "shared memory CLOSED" << endl;
	}

	return 0;
}

