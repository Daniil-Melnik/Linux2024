#include <iostream>
#include <sys/shm.h>
#include <sys/msg.h>
#include <fcntl.h>
#include <unistd.h>

#define PROG_NUM 0
#define Q_MAIN_OWNER 0
#define FILENAME "./test.txt"

using namespace std;

struct msg
{
    long whom; // тип сообщения
    int who; //кому это сообщение (whom - от кого)
    long time_stamp;
    bool ra; //false-request, true-answer (read-allowed)
};

int connectQueue();
void sendRequest(int progID, int queueIDr, long currTime);
void print_msg(const msg &msg_to_print);
void send_allow_msg_from_current(msg &cur_msg, int queue_id);
void read_file(int fd);

int queueIDm;

const unsigned MSG_LEN = sizeof(long) + sizeof(int) + sizeof(long long) + sizeof(bool);

int main(){
    int fd = open(FILENAME, O_RDONLY, S_IRUSR);
    if(fd < 0)
    {
        perror("Не удалось открыть файл");
        exit(EXIT_FAILURE);
    }

    srandom(time(NULL));
    long currTime = time(NULL);

    int queueKey;
    queueKey = connectQueue();
    sendRequest(0, queueKey, currTime);

    int queue_local = msgget(IPC_PRIVATE, 0666 | IPC_CREAT); //идентификатор очереди

    int serviced = 0; //кол-во ответов запросов на чтение
    int recieved_local = 0; // кол-во сообщений в локальной очереди
    int recieved_all = 0; //всего полученных сообщений
    int allowed = 0; //кол-во разрешений

    while (serviced != 2 || recieved_all != 4){
        cout << "Ожидание..." << endl;
        msg rec_msg;
        if (msgrcv(queueKey, &rec_msg, MSG_LEN, 1, 0) != MSG_LEN){ // 1 - msgtyp, от кого (в структуре msg.whom)
            perror("msgrcv");
            exit(EXIT_FAILURE);
        }
        ++recieved_all;
        cout << "\nMSG получено" << endl;
        print_msg(rec_msg);

        if(rec_msg.ra == true){
            cout << "Чтение разрешено из: " << (rec_msg.who-1) << endl;
            ++allowed;
            if(allowed == 2) //когда получено оба разрешения, читаем
            {
                cout << "Время = " << time(NULL) << ". Начать чтение файла: \n";
                cout << "\"\"\"" << endl;
                read_file(fd);
                cout << endl << "\"\"\"" << endl;
                cout << "Прочитано!\n";

                while(recieved_local > 0)
                {
                    msg l_msg;
                    msgrcv(queue_local, &l_msg, MSG_LEN, 0, 0);
                    --recieved_local;
                    send_allow_msg_from_current(l_msg, queueKey);
                    ++serviced;
                }
                cout << "Всем разрешено прочесть тоже.\n";
            }
        }
        else{
            cout << "Получен запрос от: " << (rec_msg.who) << endl;

            if(allowed >= 2)
            {
                //если файл прочитан, то другим читать можно
                cout << "Я уже прочитал файл, поэтому я разрешаю им читать тоже.\n";

                send_allow_msg_from_current(rec_msg, queueKey);
                ++serviced;
            }
            else //если файл не прочитан, то
            {
                // Если программа послала запрос раньше нас, то ей даём разрешение
                if(currTime < rec_msg.time_stamp)
                {
                    cout << "Моя временная метка ниже, чем у него. Позвольте прочитать позже.\n";

                    msgsnd(queue_local, &rec_msg, MSG_LEN, 0); //вот здесь!
                    ++recieved_local;
                }
                else //иначе откладываем запрос в долгий ящик
                {
                    cout << "Моя временная метка больше, чем у него. Поэтому я разрешаю им читать тоже.\n";
                    send_allow_msg_from_current(rec_msg, queueKey);
                    ++serviced;
                }
            }
        }
    }
    cout << "Нажмите Enter, чтобы закрыть очередь сообщений с id = " << queueKey << endl;
    getchar();
    getchar();

    //удаление очереди главной программой
    msgctl(queueKey, IPC_RMID, NULL);
    return 0;
}

//отправка сообщения с разрешением на чтение
void send_allow_msg_from_current(msg &cur_msg, int queue_id)
{
    unsigned buff;
    buff = cur_msg.who;
    cur_msg.who = cur_msg.whom;
    cur_msg.whom = buff;
    cur_msg.time_stamp = 0;
    cur_msg.ra = true;
    msgsnd(queue_id, &cur_msg, MSG_LEN, 0);
    cout << "\nОтправлено разрешение:" << endl;
    print_msg(cur_msg);
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
    cout << "Кому: " << msg_to_print.whom-1 << endl;
    cout << "Кто: " << msg_to_print.who-1 << endl;
    cout << "Время: " << msg_to_print.time_stamp << endl;
    cout << "Запрос(0)/Ответ(1): " << (msg_to_print.ra?"true":"false") << endl;
}

void read_file(int fd){
    unsigned char c;
    while(read(fd, &c, 1) != 0)
    {
        write(1, &c, 1);
    }
}
