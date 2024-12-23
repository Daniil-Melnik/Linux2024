#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

using namespace std;

int main(int argc, char* argv[]) {
    int skClient;
    int skServer;
    sockaddr_in socketInAddress;
    timeval timeoutVal;
    fd_set fds;
    char skClientStr[255];

    skServer = socket(AF_INET, SOCK_STREAM, 0);
    if (skServer < 0) {
        perror("ERROR creating socket");
        exit(1);
    }

    socketInAddress.sin_family = AF_INET;
    socketInAddress.sin_port = htons(3434);
    socketInAddress.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (bind(skServer, (sockaddr*) &socketInAddress, sizeof(socketInAddress)) < 0) {
        perror("ERROR bind socket");
        exit(2);
    }

    FD_ZERO(&fds);
    FD_SET(skServer, &fds);
    listen(skServer, 5);

    while (true) {

        timeoutVal.tv_sec = atoi(argv[1]);
        timeoutVal.tv_usec = 0;

        if (select(FD_SETSIZE, &fds, nullptr, nullptr, &timeoutVal) == 0) {
            cout << "Timeout client expire" << endl;
            break;
        } else {
            skClient = accept(skServer, nullptr, nullptr);
            if (skClient < 0) {
                perror("ERROR accept socket");
                exit(1);
            }

            sprintf(skClientStr, "%d", skClient);
            pid_t chPID = fork();

            if (chPID == 0) {
                execl("serverExec", "serverExec", skClientStr, argv[1], nullptr);
            }
        }

    }

    close(skServer);
    return 0;
}
