#include <signal.h>
#include <unistd.h>
#include <iostream>

int main(int argc, char* argv[]) {
    pid_t pid2 = std::stoi(argv[1]);

    // Send SIGUSR1 signal to child 2
    kill(pid2, SIGUSR1);
    return 0;
}
