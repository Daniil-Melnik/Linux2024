#include <iostream>
#include <unistd.h>
#include <signal.h>
#include <wait.h>
#include <sys/types.h>

using namespace std;

int main() {
    pid_t pid1, pid2;

    // Create child 1
    pid1 = fork();
    if (pid1 == 0) {
        execl("./child1", "child1", (char*)NULL);
    }
    else {
        // Create child 2
        pid2 = fork();
        if (pid2 == 0){
            execl("./child2", "child2", (char*)NULL);
        }
        else{
            // Wait for child 1 and child 2 to finish
            waitpid(pid1, NULL, 0);
            waitpid(pid2, NULL, 0);

            cout << "parent end" << endl;
        }
    }

    return 0;
}


