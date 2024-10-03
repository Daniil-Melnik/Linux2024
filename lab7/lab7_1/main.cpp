#include <iostream>
#include <unistd.h>
#include <signal.h>
#include <wait.h>

using namespace std;

pid_t child_pid;
int response_signal_value;

void signal_handler(int signum, siginfo_t* info, void* context) {
    cout << "Parent received signal " << signum << " from child" << endl;
    sleep(1);

    // Send response signal back to child
    child_pid = info->si_pid;
    response_signal_value = SIGRTMIN + 2; // define response signal value
    kill(0, response_signal_value);
}

int main() {
    pid_t pid1;

    // Create child 1
    pid1 = fork();
    if (pid1 == 0) {
        execl("./child", "child", (char*)NULL);
    }
    else {
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
    }

    return 0;
}
