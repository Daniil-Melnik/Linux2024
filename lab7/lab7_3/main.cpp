#include <iostream>
#include <unistd.h>
#include <signal.h>
#include <wait.h>
#include <sys/types.h>

using namespace std;


void handler(int signum) {
    cout << "parent had signal" << signum << endl;
    sleep(1);
}


int main() {
    pid_t pid1, pid2;

    // Set up signal handlers
    struct sigaction sa;
    sa.sa_handler = &handler; // Ignore the signal
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    sigaction(SIGRTMIN + 1, &sa, NULL);
    sigaction(SIGRTMIN + 2, &sa, NULL);

    // Create child 1
    pid1 = fork();
    if (pid1 == 0) {
        execl("./child2", "child2", (char*)NULL);
    }
    else {
        // Create child 2
        pid2 = fork();
        if (pid2 == 0){
            execl("./child1", "child1", (char*)NULL);
        }
        else{
            cout << pid1 << " " << pid2 << endl;
            if (pid1 > 0 && pid2 > 0){
                cout << "Both process started" << endl;
            }

            int status;
            waitpid(pid1, &status, 0);
            waitpid(pid2, &status, 0);

            cout << "parent end" << endl;
        }
    }

    return 0;
}


