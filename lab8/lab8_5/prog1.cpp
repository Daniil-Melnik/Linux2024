#include <iostream>
#include <sys/shm.h>
#include <sys/msg.h>
#include <fcntl.h>
#include <unistd.h>
#include <unistd.h>

#define PROG_NUM 0
#define Q_MAIN_OWNER 0
#define FILENAME "./test.txt"

using namespace std;

struct msg
{
    long whom; // кому
    int who; //кто
    long time_stamp;
    int ra_key;
};

int connectQueue();
void sendRequest(int progID, int queueIDr, long currTime);
void print_msg(const msg &msg_to_print);
void send_allow_msg_from_current(msg &cur_msg, int queue_id);
void read_file(int fd);

msg tmp_msgs[2];
int queue_local;

int queueIDm;

const unsigned MSG_LEN = sizeof(long) + sizeof(int) + sizeof(long long) + sizeof(int);

int main(){
    struct msqid_ds mq_stat, mq_stat_local;
    int fd = open(FILENAME, O_RDONLY, S_IRUSR);
    if(fd < 0)
    {
        perror("Не удалось открыть файл");
        exit(EXIT_FAILURE);
    }

    int n = 0;
    srandom(time(NULL));
    long currTime = time(NULL);

    int queueKey;
    queueKey = connectQueue();
    queue_local = msgget(IPC_PRIVATE, 0666 | IPC_CREAT | IPC_EXCL); //идентификатор очереди
    sendRequest(0, queueKey, currTime);



    int serviced = 0; //кол-во ответов запросов на чтение
    int recieved_local = 0; // кол-во сообщений в локальной очереди
    int recieved_all = 0; //всего полученных сообщений
    int allowed = 0; //кол-во разрешений

    int n_request = 0;

    while (n_request < 2){
        msg rec_msg;
        int rc;
        rc = msgrcv(queueKey, &rec_msg, MSG_LEN, 1, 0);
        if (rc == MSG_LEN) {
            n_request = n_request + 1;
            cout << "Получен запрос от: " << (rec_msg.who) << endl;

            if(allowed >= 2) // файл прочитан - разрешить
            {
                // cout << "Я уже прочитал файл, поэтому я разрешаю им читать тоже.\n";

                send_allow_msg_from_current(rec_msg, queueKey);
                serviced = serviced + 1;
            }
            else //если файл не прочитан, то
            {
                // запрос от молодого
                if(currTime < rec_msg.time_stamp)
                {
                    cout << "Временная метка ниже. Ответить позже.\n";
                    tmp_msgs[recieved_local] = rec_msg;
                    ++recieved_local;
                }
                else // от старого - разрешить чтение
                {
                    cout << "Временная метка больше. Разрешить.\n";
                    send_allow_msg_from_current(rec_msg, queueKey);
                    ++serviced;
                }
            }

        }
    }
    cout << "все запросы получены\n";
    while (allowed != 2) {
        msg rec_msg_local;
        int local_rc;
        local_rc = msgrcv(queue_local, &rec_msg_local, MSG_LEN, 1, 0);
        if (local_rc == MSG_LEN){
            allowed = allowed + 1;
            cout << "Чтение разрешено из: " << (rec_msg_local.who) << endl;
            if(allowed == 2) //когда получено оба разрешения, читаем
            {
                cout << "Время = " << time(NULL) << ". Начать чтение файла: \n";
                cout << "\"\"\"" << endl;
                read_file(fd);
                cout << endl << "\"\"\"" << endl;
                cout << "Прочитано!\n";
            }
        }
    }

    while(recieved_local > 0) // ответить после себя всем молодым
    {
        msg l_msg;
        l_msg = tmp_msgs[recieved_local - 1];
        --recieved_local;
        send_allow_msg_from_current(l_msg, queueKey);
        ++serviced;
    }
    cout << "Всем разрешено прочесть тоже.\n";

    cout << "Нажмите Enter, чтобы закрыть очередь сообщений с id = " << queueKey << endl;
    getchar();
    getchar();

    //удаление очереди
    msgctl(queueKey, IPC_RMID, NULL);
    return 0;
}

//отправка разрешения на чтение
void send_allow_msg_from_current(msg &cur_msg, int queue_id)
{
    unsigned buff;
    buff = cur_msg.who;
    cur_msg.who = cur_msg.whom;
    cur_msg.whom = buff;
    cur_msg.time_stamp = 0;
    cur_msg.ra_key = cur_msg.ra_key;
    msgsnd(cur_msg.ra_key, &cur_msg, MSG_LEN, 0);
    //cout << "\nОтправлено разрешение:" << endl;
    cout << "\nОтправлено разрешение:" << cur_msg.whom << endl;
    //print_msg(cur_msg);
}

//отправка запроса
void sendRequest(int progID, int queueIDr, long currTime)
{
    int prog_num = 0;
    msg _msg;
    _msg.whom = (prog_num + 1) % 3 + 1;
    _msg.who = prog_num + 1;
    _msg.time_stamp = currTime;
    _msg.ra_key = queue_local;

    cout << "Отправка запроса:\n";
    print_msg(_msg);
    msgsnd(queueIDr, &_msg, MSG_LEN, 0);
    cout << endl;

    _msg.whom = (prog_num + 2) % 3 + 1;
    cout << "Отправка запроса:\n";
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
    cout << "Кому: " << msg_to_print.whom << endl;
    cout << "Кто: " << msg_to_print.who << endl;
    cout << "Время: " << msg_to_print.time_stamp << endl;
    cout << "Очередь для ответа " << (msg_to_print.ra_key) << endl;
}

void read_file(int fd){
    unsigned char c;
    while(read(fd, &c, 1) != 0)
    {
        write(1, &c, 1);
    }
}
