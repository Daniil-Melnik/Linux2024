
// child.cpp
#include <iostream>
#include <unistd.h>
#include <signal.h>

using namespace std;

void signal_handler(int signum, siginfo_t* info, void* context) {
    cout << "Child received signal " << signum << " from parent" << endl;

    // Send a signal back to the parent
    pid_t parent_pid = getppid();
    int signal_value = SIGRTMIN + 1; // define custom signal value
    sleep(1);
    kill(0, signal_value);
}

int main() {
    cout << "Hello from child" << endl;

    // Install signal handler for response signal
    struct sigaction sa;
    sa.sa_sigaction = signal_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_SIGINFO;

    int response_signal_value = SIGRTMIN + 2; // define response signal value
    if (sigaction(response_signal_value, &sa, nullptr) == -1) {
        cerr << "Error installing signal handler" << endl;
        return 1;
    }

    // Send a custom signal to the parent process
    pid_t parent_pid = getppid();
    int signal_value = SIGRTMIN + 1; // define custom signal value
    kill(parent_pid, signal_value);


    return 0;
}
