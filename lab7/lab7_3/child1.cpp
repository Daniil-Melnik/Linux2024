#include <iostream>
#include <unistd.h>
#include <signal.h>

using namespace std;

int cnt = 0;

void signal_handler(int signum, siginfo_t* info, void* context) {
    cout << "Child 1 received response signal " << signum << " from child 2" << endl;
    sleep(1);
    int signal_value = SIGRTMIN + 1; // define custom signal value
    kill(0, signal_value);
}

int main() {
    cout << "Hello from child 1" << endl;

    // Install signal handler for response signal

    struct sigaction sa;
    sa.sa_sigaction = signal_handler;
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGRTMIN + 1);
    sigprocmask(SIG_BLOCK, &mask, nullptr);
    sa.sa_flags = SA_SIGINFO;

    int response_signal_value = SIGRTMIN + 2; // define response signal value
    if (sigaction(response_signal_value, &sa, nullptr) == -1) {
        cerr << "Error installing signal handler" << endl;
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

