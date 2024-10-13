#include <iostream>
#include <unistd.h>
#include <wait.h>

int main() {
    pid_t pid1, pid2;

    // Create child 1
    pid1 = fork();
    if (pid1 == 0) {
        // Create child 2
        pid2 = fork();
        if (pid2 == 0) {
            // Child 2
            execl("./child2", "child2", (char*)NULL);
        } else {
            // Child 1
            execl("./child1", "child1", std::to_string(pid2).c_str(), (char*)NULL);
        }
    } else {
        // Parent
        waitpid(pid1, NULL, 0);
        waitpid(pid2, NULL, 0);
    }

    return 0;
}
