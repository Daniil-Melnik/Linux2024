#include <iostream>
#include <unistd.h>
#include <signal.h>

using namespace std;

void signal_handler(int signum, siginfo_t* info, void* context) {
    cout << "Child 2 received signal " << signum << " from child 1" << endl;
    sleep(1);
    int signal_value = SIGRTMIN + 2; // define response signal value
    kill(0, signal_value);
}

int main() {
    cout << "Hello from child 2" << endl;
    // Install signal handler for custom signal
    struct sigaction sa;
    sa.sa_sigaction = signal_handler;
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGRTMIN + 2);
    sigprocmask(SIG_BLOCK, &mask, nullptr);
    sa.sa_flags = SA_SIGINFO;

    int signal_value = SIGRTMIN + 1; // define custom signal value

    if (sigaction(signal_value, &sa, nullptr) == -1) {
        cerr << "Error installing signal handler" << endl;
        return 1;
    }

    while (true) {
        // Wait for child to send signal
        pause();
    }
    return 0;
}
