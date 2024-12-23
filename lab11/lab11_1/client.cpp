#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

using namespace std;

int main(int argc, char* argv[]) {

    int skClient;
    int begin_time;
    int isConnected = 0;
    char sendMSG[10];
    char recvMSG[10];
    fd_set readfds;
    sockaddr_in skServerAddr;
    timeval timeoutVal;

    const int timeout = atoi(argv[1]);
    srand(time(nullptr));

    skClient = socket(AF_INET, SOCK_STREAM, 0);
    if (skClient < 0) {
        perror("ERROR creating socket");
        exit(1);
    }

    skServerAddr.sin_family = AF_INET;
    skServerAddr.sin_port = htons(3434);
    skServerAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    begin_time = time(nullptr);

    while ((time(nullptr) - begin_time) < timeout
    && (isConnected = connect(skClient, (sockaddr*) &skServerAddr, sizeof(skServerAddr))) < 0)
    {}

    if (isConnected == -1) {
        cout << "Timeout connect server expired" << endl;
        close(skClient);
        exit(-1);
    }

    for (int i = 0; i < 10; i++) {
        sendMSG[i] = '0' + rand() % 10;
    }

    timeoutVal.tv_sec = timeout;
    timeoutVal.tv_usec = 0;

    FD_ZERO(&readfds);
    FD_SET(skClient, &readfds);

    setsockopt(skClient, SOL_SOCKET, SO_SNDTIMEO, (const char*)&timeoutVal, sizeof(timeoutVal));

    if (int bytesSent = send(skClient, sendMSG, sizeof(sendMSG), 0) == -1){
        cout << "Timeout sending expired" << endl;
        close(skClient);
        exit(-1);
    } else {
        cout << "Message sent: " << sendMSG << endl;
    }

    if (select(FD_SETSIZE, &readfds, nullptr, nullptr, &timeoutVal) == 0) {
        cout << "Timeout for server response has expired" << endl;
    } else {
        recv(skClient, recvMSG, sizeof(recvMSG), 0);
        cout << "Message was received: " << recvMSG << endl;
    }

    close(skClient);
    return 0;
}
