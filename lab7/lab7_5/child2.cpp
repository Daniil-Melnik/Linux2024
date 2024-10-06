#include <iostream>
#include <unistd.h>
#include <signal.h>

using namespace std;

int fildes[2];

char ch;
int pipe_read_is_done;

bool mainWrite = true;

void signal_handler(int signum, siginfo_t* info, void* context) {
    cout << "Child 2 received signal " << signum << " from child 1" << endl;
    if (signum == SIGRTMIN + 3){
        mainWrite = false;
        cout << "child 2 exit" << endl;
        sleep(1);
    }
    if (signum == SIGRTMIN + 1){
        sleep(1);
        if (pipe_read_is_done = read(fildes[0], &ch, 1) > 0){
            cout << "child 2 #" << ch << endl;
        }
        int signal_value = SIGRTMIN + 2; // define response signal value
        kill(0, signal_value);
        if (!(pipe_read_is_done > 0 || mainWrite)){
            exit(0);
        }
    }
}

int main(int argc, char *argv[]) {
    fildes[0] = *argv[1];
    fildes[1] = *argv[2];

    cout << "Hello from child 2" << endl;
    // Install signal handler for custom signal
    struct sigaction sa;
    sa.sa_sigaction = signal_handler;
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGRTMIN + 2);
    sigprocmask(SIG_BLOCK, &mask, nullptr);
    sa.sa_flags = SA_SIGINFO;

    int signal_value_1 = SIGRTMIN + 1; // define custom signal value
    int signal_value_2 = SIGRTMIN + 3; // define custom signal value

    if (sigaction(signal_value_1, &sa, nullptr) == -1) {
        cerr << "Error installing signal handler 1" << endl;
        return 1;
    }

    if (sigaction(signal_value_2, &sa, nullptr) == -1) {
        cerr << "Error installing signal handler 2" << endl;
        return 1;
    }


    while (true) {
        // Wait for child to send signal
        pause();
    }
    return 0;
}
