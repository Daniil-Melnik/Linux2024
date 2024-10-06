#include <iostream>
#include <unistd.h>
#include <signal.h>

using namespace std;

int cnt = 0;

int fildes[2];
bool mainWrite = true;

char ch;
int pipe_read_is_done;

void signal_handler(int signum, siginfo_t* info, void* context) {
    cout << "Child 1 received response signal " << signum << " from child 2" << endl;
    if (signum == SIGRTMIN + 3){
        mainWrite = false;
        cout << "child 1 exit" << endl;
    }
    if (signum == SIGRTMIN + 2){
        sleep(1);
        if (pipe_read_is_done = read(fildes[0], &ch, 1) > 0){
            cout << "child 1 #" << ch << endl;
        }
        int signal_value = SIGRTMIN + 1; // define custom signal value
        kill(0, signal_value);
        if (!(pipe_read_is_done > 0 || mainWrite)){
            exit(0);
        }
    }
}

int main(int argc, char *argv[]) {
    fildes[0] = *argv[1];
    fildes[1] = *argv[2];

    cout << "Hello from child 1" << endl;
    // Install signal handler for response signal
    struct sigaction sa;
    sa.sa_sigaction = signal_handler;
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGRTMIN + 1);
    sigprocmask(SIG_BLOCK, &mask, nullptr);
    sa.sa_flags = SA_SIGINFO;

    int signal_value_1 = SIGRTMIN + 2; // define custom signal value
    int signal_value_2 = SIGRTMIN + 3; // define custom signal value

    if (sigaction(signal_value_1, &sa, nullptr) == -1) {
        cerr << "Error installing signal handler 1" << endl;
        return 1;
    }

    if (sigaction(signal_value_2, &sa, nullptr) == -1) {
        cerr << "Error installing signal handler 2" << endl;
        return 1;
    }

    int signal_value = SIGRTMIN + 1; // define custom signal value
    kill(0, signal_value);
    cout << "Start signal sent" << endl;

    // Wait for response signal from parent
    while (true){
       pause();
    }


    return 0;
}

