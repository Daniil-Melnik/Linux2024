#include <iostream>
#include <sys/shm.h>
#include <sys/msg.h>

#define PROG_NUM 3
#define Q_MAIN_OWNER 1

using namespace std;

int queueIDm;

int main(){
    int id;
    int queueID;
    cout << "Введите ID: ";
    cin >> queueID;

    if (PROG_NUM == Q_MAIN_OWNER){
        queueIDm = msgget(queueIDm, 0666 | IPC_CREAT | IPC_EXCL);
        while (queueID == -1){
            cout << "ID занят, введите новый: ";
            cin >> queueID;
            queueIDm = msgget(queueID, 0666 | IPC_CREAT | IPC_EXCL);
        }
        cout << "Очередь с ключом =  " << queueID << " была создана. Ее id = " << queueIDm << ".\n";
    }
    else {
        do
        {
            queueIDm = msgget(queueID, IPC_EXCL);
            if(queueIDm == -1)
            {
                cout << "Очередь с ключом = " << queueID << " еще не существует. Ожидание...\n";
            }
        }while(queueIDm == -1);
        queueIDm = msgget(queueID, IPC_CREAT);
        cout << "Было вложение в очередь с ключом = " << queueID << ". Ее id = " << queueIDm << ".\n";
    }
    return 0;
}
