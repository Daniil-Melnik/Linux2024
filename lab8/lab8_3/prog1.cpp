#include <iostream>
#include <sys/shm.h>
#include <sys/msg.h>

#define PROG_NUM 0
#define Q_MAIN_OWNER 0

using namespace std;

struct msg
{
    long whom; // тип сообщения
    int who; //кому это сообщение (whom - от кого)
    long long time_stamp;
    bool ra; //false-request, true-answer (read-allowed)
};

int connectQueue();
void sendRequest(int progID, int queueIDr, long currTime);
void print_msg(const msg &msg_to_print);

int queueIDm;

const unsigned MSG_LEN = sizeof(long) + sizeof(int) + sizeof(long long) + sizeof(bool);

int main(){
    srandom(time(NULL));
    long currTime = time(NULL);

    int queueKey;
    queueKey = connectQueue();

    sendRequest(0, queueKey, currTime);

    int queue_local = msgget(IPC_PRIVATE, 0666 | IPC_CREAT); //идентификатор очереди
    //Ответы каждая программа должна принимать в свою локальную очередь.

    int serviced = 0; //кол-во ответов запросов на чтение
    int recieved_local = 0; // кол-во сообщений в локальной очереди
    int recieved_all = 0; //всего полученных сообщений
    int allowed = 0; //кол-во разрешений
    //пока всем не ответил

    while (serviced != 2 || recieved_all != 4){
        cout << "Ожидание..." << endl;
        msg rec_msg;
        if (msgrcv(queueKey, &rec_msg, MSG_LEN, 1, 0) != MSG_LEN){ // 1 - msgtyp, от кого (в структуре msg.whom)
            perror("msgrcv");
            exit(EXIT_FAILURE);
        }
        ++recieved_all;
        cout << "MSG получено" << endl;
    }
    msgctl(queueKey, IPC_RMID, NULL);
    return 0;
}

//отправка первичного запроса
void sendRequest(int progID, int queueIDr, long currTime)
{
    int prog_num = 0;
    msg _msg;
    _msg.whom = (prog_num + 1) % 3 + 1;
    _msg.who = prog_num+1;
    _msg.time_stamp = currTime;
    _msg.ra = false;

    cout << "Отправка msg:\n";
    print_msg(_msg);
    msgsnd(queueIDr, &_msg, MSG_LEN, 0);
    cout << endl;

    //msgid – идентификатор очереди, в которую посылается сообщение
    //msgp – адрес буфера, где располагается передаваемое сообщение
    //msgsz – длина передаваемого сообщения
    //msgflg –флаг, определяющий режим выполнения операции
    _msg.whom = (prog_num + 2) % 3 + 1;
    cout << "Отправка msg:\n";
    print_msg(_msg);
    msgsnd(queueIDr, &_msg, MSG_LEN, 0);
    cout << endl;
}


int connectQueue(){
    int queueID;
    cout << "Введите ID: ";
    cin >> queueID;

    if (PROG_NUM == Q_MAIN_OWNER){
        queueIDm = msgget(queueID, 0666 | IPC_CREAT | IPC_EXCL);
        while (queueIDm == -1){
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
    return queueIDm;
}

void print_msg(const msg &msg_to_print)
{
    cout << "От: " << msg_to_print.whom-1 << endl;
    cout << "К: " << msg_to_print.who-1 << endl;
    cout << "Время: " << msg_to_print.time_stamp << endl;
    cout << "ra: " << (msg_to_print.ra?"true":"false") << endl;
}
