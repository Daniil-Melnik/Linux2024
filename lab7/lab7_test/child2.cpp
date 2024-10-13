#include <iostream>
#include <signal.h>

void signalHandler(int signal) {
    if (signal == SIGUSR1) {
        std::cout << "Hello" << std::endl;
    }
}

int main() {
    struct sigaction sa;
    sa.sa_handler = signalHandler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGUSR1, &sa, NULL);

    // Wait for signal
    pause();

    return 0;
}
