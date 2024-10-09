#include <unistd.h>
#include <fcntl.h>
#include <sys/msg.h>

#include <iostream>

#define PROG_NUM 0
#define FILENAME "./test.txt"
#define Q_MAIN_OWNER 0
using std::cout;
using std::endl;

struct msg
{
    long whom; // тип сообщения
    int who; //кому это сообщение (whom - от кого)
    long long time_stamp;
    bool ra; //false-request, true-answer (read-allowed)
};

//длина передаваемого сообщения (структуры msg)
const unsigned MSG_LEN = sizeof(long) + sizeof(int) + sizeof(long long) + sizeof(bool);
void print_msg(const msg &msg_to_print);

int create_connect(unsigned prog_num, int queue_key);
void send_requests_to_read(unsigned prog_num, int queue_id, long long cur_time);
void read_file(int fd);
void send_allow_msg_from_current(msg &cur_msg, int queue_id);

/*
> ./main {queue_key}
*/
int main(int argc, char* argv[])
{
    srandom(time(NULL));
    if(argc != 2)
    {
        cout << "Ошибка синтаксиса. Ожидалось: \"> ./main {queue_key}\"" << endl;
        exit(EXIT_FAILURE);
    }
    const unsigned prog_num = PROG_NUM;
    const int queue_key = atoi(argv[1]);
    if(queue_key <= 0)
    {
        cout << "{queue_key} должен быть не меньше 0. " << endl;
        exit(EXIT_FAILURE);
    }

    int fd = open(FILENAME, O_RDONLY, S_IRUSR);
    if(fd < 0)
    {
        perror("Не удалось открыть файл");
        exit(EXIT_FAILURE);
    }

    //создает очередь сообщений, и все остальные программы коннектятся к ней
    const int msqid = create_connect(prog_num, queue_key);
    long long send_req_time = time(NULL);
    cout << "Текущая временная метка: " << send_req_time << endl;
    send_requests_to_read(prog_num, msqid, send_req_time);
    cout << "Сообщения запроса чтения для двух других программ были отправлены.\n";

    int queue_local = msgget(IPC_PRIVATE, 0666 | IPC_CREAT); //идентификатор очереди
    //Ответы каждая программа должна принимать в свою локальную очередь.

    int serviced = 0; //кол-во ответов запросов на чтение
    int recieved_local = 0; // кол-во сообщений в локальной очереди
    int recieved_all = 0; //всего полученных сообщений
    int allowed = 0; //кол-во разрешений
    //пока всем не ответил
    while(serviced != 2 || recieved_all != 4)
    {
        cout << "Получение msg...\n";
        msg rec_msg;
        //прием сообщения (в rec_msg)
        if(msgrcv(msqid, &rec_msg, MSG_LEN, prog_num+1, 0) != MSG_LEN) //прием здесь!!!
        {
            perror("msgrcv");
            exit(EXIT_FAILURE);
        }
        ++recieved_all;
        cout << "Msg получено: \n";
        print_msg(rec_msg);

        if(rec_msg.ra == true)
        {
            //Одна из программ разрешила читать, помечаем это
            cout << "Чтение разрешено из: " << (rec_msg.who-1) << endl;

            ++allowed;
            //когда получено оба разрешения, читаем
            if(allowed == 2)
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
                    send_allow_msg_from_current(l_msg, msqid);
                    ++serviced;
                }
                cout << "Всем разрешено прочесть тоже.\n";
            }
        }
        else //запрос на чтение
        {
            cout << "Прочитан запрос от: " << (rec_msg.who-1) << endl;
            if(allowed >= 2)
            {
                //если файл прочитан, то другим читать можно
                cout << "Я уже прочитал файл, поэтому я разрешаю им читать тоже.\n";

                send_allow_msg_from_current(rec_msg, msqid);
                ++serviced;
            }
            else //если файл не прочитан, то
            {
                // Если программа послала запрос раньше нас, то ей даём разрешение
                if(send_req_time < rec_msg.time_stamp)
                {
                    cout << "Моя временная метка ниже, чем у него. Позвольте прочитать позже.\n";

                    msgsnd(queue_local, &rec_msg, MSG_LEN, 0); //вот здесь!
                    ++recieved_local;
                }
                else //иначе откладываем запрос в долгий ящик
                {
                    cout << "Моя временная метка больше, чем у него. Поэтому я разрешаю им читать тоже.\n";
                    send_allow_msg_from_current(rec_msg, msqid);
                    ++serviced;
                }
            }
        }
        cout << "Получено: " << recieved_all << ", обработано: " << serviced << "\n\n\n";
    }

    close(fd);

    if(prog_num == Q_MAIN_OWNER)
    {
        cout << "Нажмите Enter, чтобы закрыть очередь сообщений с id = " << msqid << endl;
        getchar();

        //удаление очереди главной программой
        msgctl(msqid, IPC_RMID, NULL);
    }

    cout << "============================== КОНЕЦ! ==============================" << endl;
    return 0;
}

//создает очередь сообщений, и все остальные коннектятся к ней
int create_connect(unsigned prog_num, int queue_key)
{
    int idOfQueue;
    if(prog_num == Q_MAIN_OWNER)
    {
        idOfQueue = msgget(queue_key, 0666 | IPC_CREAT | IPC_EXCL); //проверка наличия очереди с ключом
        if(idOfQueue == -1) //если не получилось, вывести сообщение и выйти из программы
        {
            cout << "Не удается создать очередь сообщений. Очередь с ключом = " <<
                queue_key << " уже существует, выберите другой\n";
            exit(EXIT_FAILURE);
        }
        cout << "Очередь с ключом =  " << queue_key << " была создана. Ее id = " << idOfQueue << ".\n";
    }
    else
    {
        do
        {
            idOfQueue = msgget(queue_key, IPC_EXCL);
            if(idOfQueue == -1)
            {
                cout << "Очередь с ключом = " << queue_key << " еще не существует. Ожидание...\n";
            }
        }while(idOfQueue == -1);
        idOfQueue = msgget(queue_key, IPC_CREAT);
        cout << "Было вложение в очередь с ключом = " << queue_key << ". Ее id = " << idOfQueue << ".\n";
    }
    return idOfQueue;
}

//отправка первичного запроса
void send_requests_to_read(unsigned prog_num, int queue_id, long long cur_time)
{
    msg _msg;
    _msg.whom = (prog_num + 1) % 3 + 1;
    _msg.who = prog_num+1;
    _msg.time_stamp = cur_time;
    _msg.ra = false;

    cout << "Отправка msg:\n";
    print_msg(_msg);
    msgsnd(queue_id, &_msg, MSG_LEN, 0);
    cout << endl;

    //msgid – идентификатор очереди, в которую посылается сообщение
    //msgp – адрес буфера, где располагается передаваемое сообщение
    //msgsz – длина передаваемого сообщения
    //msgflg –флаг, определяющий режим выполнения операции
    _msg.whom = (prog_num + 2) % 3 + 1;
    cout << "Отправка msg:\n";
    print_msg(_msg);
    msgsnd(queue_id, &_msg, MSG_LEN, 0);
    cout << endl;
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
}

void print_msg(const msg &msg_to_print)
{
    cout << "От: " << msg_to_print.whom-1 << endl;
    cout << "К: " << msg_to_print.who-1 << endl;
    cout << "Время: " << msg_to_print.time_stamp << endl;
    cout << "ra: " << (msg_to_print.ra?"true":"false") << endl;
}

void read_file(int fd)
{
    unsigned char c;
    while(read(fd, &c, 1) != 0)
    {
        write(1, &c, 1);
    }
}
