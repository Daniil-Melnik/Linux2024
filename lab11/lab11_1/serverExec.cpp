#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <algorithm>

using namespace std;

int main(int argc, char* argv[]) {
    char msgBuf[10];

    int skClient = atoi(argv[1]);

    struct timeval timeout;
    timeout.tv_sec = 5;
    timeout.tv_usec = 0;

    setsockopt(skClient, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout));

    recv(skClient, msgBuf, 10, 0);

    cout << "Msg received: " << msgBuf << endl;
    sort(msgBuf, msgBuf + sizeof(msgBuf));

    if (send(skClient, msgBuf, 10, 0) > 0) {
        cout << "Msg sent: " << msgBuf << endl << endl;
    } else {
        cout << "ERROR sending message";
    }
    exit(0);
}
