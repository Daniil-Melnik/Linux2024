#include <iostream>
#include <unistd.h>
#include <time.h>
#include <signal.h>

using namespace std;

int cnt = 0;

int fildes[2];
bool mainWrite = true;

FILE * outFile;

char ch;
int pipe_read_is_done;

void signal_handler(int signum, siginfo_t* info, void* context) {
    // cout << "Child 1 received response signal " << signum << " from child 2" << endl;
    if (signum == SIGQUIT){
        mainWrite = false;
        // cout << "child 1 exit" << endl;
    }
    if (signum == SIGUSR2){
        usleep(300000);
        if (pipe_read_is_done = read(fildes[0], &ch, 1) > 0){
            cout << "child 2 #" << ch << endl;
            fputc(ch, outFile);
        }
        int signal_value = SIGUSR1;
        kill(0, signal_value);
        if (!(pipe_read_is_done > 0 || mainWrite)){
            fclose(outFile);
            close(fildes[0]);
            exit(0);
        }
    }
}

int main(int argc, char *argv[]) {
    fildes[0] = *argv[1];
    fildes[1] = *argv[2];

    outFile = fopen(argv[3], "w");

    cout << "Hello from child 2" << endl;

    struct sigaction sa;
    sa.sa_sigaction = signal_handler;
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGUSR1);
    sigprocmask(SIG_BLOCK, &mask, nullptr);
    sa.sa_flags = SA_SIGINFO;

    int signal_value_1 = SIGUSR2;
    int signal_value_2 = SIGQUIT;

    if (sigaction(signal_value_1, &sa, nullptr) == -1) {
        cerr << "Error installing signal handler 1" << endl;
        return 1;
    }

    if (sigaction(signal_value_2, &sa, nullptr) == -1) {
        cerr << "Error installing signal handler 2" << endl;
        return 1;
    }

    int signal_value = SIGUSR1;
    kill(0, signal_value);
    cout << "Start signal sent" << endl;

    while (true){
       pause();
    }

    return 0;
}

