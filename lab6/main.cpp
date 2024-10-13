#include <iostream>
#include <signal.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/wait.h>
#include <cstring>

using namespace std;

int lCnt = 0;
int lNum;
float parentStart; // начало отсчета процесса-родителя

float finalTime (float childStart)
{
    return (float)clock() - childStart;
}

char * makeDate(tm* childTime){
    char *date = asctime(childTime);
    if (date[strlen(date) - 1] == '\n'){
        date[strlen(date) - 1] = '\0';
    }
    return date;
}

void processFunction(int local_int) // повторяющийся процесс
{
    parentStart = (float)clock();

    int childStatus;
    float childStart;
    time_t childSeconds = time (NULL);
    struct tm* childTime = localtime (&childSeconds); // время начала работы родителя



    pid_t childPID = fork(); // потомок

    if (childPID == 0) // если процесс создан
    {
        sigset_t local_set;
        childStart = (float)clock() / CLOCKS_PER_SEC; // начало работы потомка

        sigemptyset(&local_set);

        cout << getpid() << "  |  " << makeDate(childTime);

        // блокировка Ctrl+Z
        sigaddset(&local_set, SIGTSTP);
        sigprocmask(SIG_BLOCK, &local_set, NULL);

        cout << " |  " << finalTime(childStart); // потомок

        exit(EXIT_SUCCESS);
    }
    else
    {
        waitpid(childPID, &childStatus, 0); // ожидание потомка
        cout << "        | " << finalTime(parentStart) << endl;
    }
}

int main(int argc, char *argv[])
{
    if (argc != 3) {
        cout << "Usage: " << argv[0] << " <period> <lNum>" << endl;
        return 1;
    }

    int period = atoi(argv[1]); // время на 1 запуск
    lNum = atoi(argv[2]); // кол-во запусков
    struct itimerval value; // новое значение таймера
    struct sigaction sigact; //структура, в которой описывается реакция на сигнал

    // обработчик сигнала
    sigact.sa_handler = processFunction;
    sigemptyset(&sigact.sa_mask);
    sigact.sa_flags = 0;

    // блокировка Ctrl+Z
    sigaddset(&sigact.sa_mask, SIGTSTP);
    sigprocmask(SIG_BLOCK, &sigact.sa_mask, NULL);

    // установка SIGALARM
    sigaction(SIGALRM, &sigact, NULL);

    // время до первого срабатывания таймера
    value.it_value.tv_sec = period;
    value.it_value.tv_usec = 0;

    // интервал между срабатываниями таймера
    value.it_interval.tv_sec = period;
    value.it_interval.tv_usec = 0;

    //установка таймера
    setitimer(ITIMER_REAL, &value, NULL);

    // бесконечный цикл ожидания
    cout << "  PID  |" << "      " << "  child start" << "        " << "| child worked" << "     " << "| parent worked"<< endl;
    while (lCnt < lNum) {
        lCnt++;
        pause(); // ожидание SIGALRM
    }

    return 0;
}
