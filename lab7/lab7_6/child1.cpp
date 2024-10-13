#include <iostream>
#include <unistd.h>
#include <signal.h>
#include <time.h>

using namespace std;

int fildes[2];

char ch;
int pipe_read_is_done;

FILE* outFile;

bool mainWrite = true;

void signal_handler(int signum, siginfo_t* info, void* context) {
    // cout << "Child 1 received signal " << signum << " from child 2" << endl;
    if (signum == SIGQUIT){
        mainWrite = false;
        // cout << "child 1 exit" << endl;
        //usleep(300000);
    }
    if (signum == SIGUSR1){
        //usleep(300000);
        if (pipe_read_is_done = read(fildes[0], &ch, 1) > 0){
            cout << "child 1 #" << ch << endl;
            fputc(ch, outFile);
        }
        int signal_value = SIGUSR2; // сигнал для посылки
        kill(0, signal_value); // посылка всем
        if (!(pipe_read_is_done > 0 || mainWrite)){ // если родитель закончил писать и в канале кончились символы
            fclose(outFile); // закрытие выходного файла
            close(fildes[0]); // закрытие канала
            exit(0); // завершение потомка
        }
    }
}

int main(int argc, char *argv[]) {
    fildes[0] = *argv[1];
    fildes[1] = *argv[2];

    outFile = fopen(argv[3], "w"); // открытие файла для печати

    cout << "Hello from child 1" << endl;

    // установка настроек сигналов
    struct sigaction sa;
    sa.sa_sigaction = signal_handler;
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGUSR2);
    sigprocmask(SIG_BLOCK, &mask, nullptr);
    sa.sa_flags = SA_SIGINFO;

    int signal_value_1 = SIGUSR1;
    int signal_value_2 = SIGQUIT;

    if (sigaction(signal_value_1, &sa, nullptr) == -1) {
        cerr << "Error installing signal handler 1" << endl;
        return 1;
    }

    if (sigaction(signal_value_2, &sa, nullptr) == -1) {
        cerr << "Error installing signal handler 2" << endl;
        return 1;
    }


    while (true) {
        pause(); // ожидание нового сигнала
    }
    return 0;
}
