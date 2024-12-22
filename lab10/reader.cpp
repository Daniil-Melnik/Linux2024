#include <iostream>
#include <fstream>
#include <string>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <unistd.h>

using namespace std;


int sKey = 500;
int nSemaphores = 4;
string fName = "file.txt";

// struct sembuf file_DECR = {0, -1, 0}; // т.к. они могут одновременно читать
struct sembuf file_INCR = {0, 1, 0};

struct sembuf writer_COMPARE = {1, 0, 0};
// struct sembuf writer_DECR = {1, -1, 0};
// struct sembuf writer_INCR = {1, 1, 0};

// struct sembuf reader_COMPARE = {2, 0, 0};
struct sembuf reader_DECR = {2, -1, 0};
struct sembuf reader_INCR = {2, 1, 0};

struct sembuf proc_DECR = {3, -1, 0};
struct sembuf proc_INCR = {3, 1, 0};


int main (int argc, char* argv[]) {
    int SID;
    char fBuf[80];
    ifstream file;

    SID = semget(sKey, nSemaphores, IPC_CREAT | IPC_EXCL | 0666);

    if (SID != -1) {
        cout << "CREATED semaphore id: " << SID << endl;
        semop(SID, &file_INCR, 1);
    } else {
        SID = semget(sKey, nSemaphores, IPC_CREAT);
        if (SID == -1){
            cout << "NOT EXIST semaphore with key: " << sKey << endl;
            exit(EXIT_FAILURE);
        }
        cout << "CONNECTED to semaphore id: " << SID << endl;
    }

    semop(SID, &proc_INCR, 1);
    cout << "NUMBER of running program: " << semctl(SID, 3, GETVAL, nullptr) << endl;

    cout << "WAIT for WRITERS" << endl;
    semop(SID, &writer_COMPARE, 1); // пропустит если нет писателей

    cout << "++ readers" << endl;
    semop(SID, &reader_INCR, 1);

    cout << "reading file" << endl;
    file.open(fName);
    while (file.getline(fBuf, 80)) {
        cout << fBuf << endl;
    }
    file.close();

    cout << "-- readers" << endl;
    semop(SID, &reader_DECR, 1);

    semop(SID, &proc_DECR, 1);
    if (semctl(SID, 3, GETVAL, nullptr) == 0) {
        semctl(SID, 0, IPC_RMID, nullptr);
        cout << "REMOVE semaphore" << endl;
    }

    cout << "program FINISH" << endl;
    return 0;
}
