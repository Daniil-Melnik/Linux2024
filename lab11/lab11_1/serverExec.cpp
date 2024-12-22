#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <algorithm>

using namespace std;

int main(int argc, char* argv[]) {
    char msgBuf[10];

    int skClient = atoi(argv[1]); // получение сокета для обраной отправки

    recv(skClient, msgBuf, 10, 0); // получение строки от клиента по сокету
    cout << "Msg received: " << msgBuf << endl;

    sort(msgBuf, msgBuf + sizeof(msgBuf)); // сортировка строки

    if (send(skClient, msgBuf, 10, 0) > 0) { // отправка обратного сообщения
        cout << "Msg sent: " << msgBuf << endl;
    } else {
        cout << "ERROR sending message";
    }
    exit(0);
}
