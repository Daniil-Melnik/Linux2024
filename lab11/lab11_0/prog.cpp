#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <algorithm>

using namespace std;

int main(int argc, char* argv[]) {
    char subsequence_buf[10];

    int client_socket = atoi(argv[1]); // получение сокета для обраной отправки

    recv(client_socket, subsequence_buf, 10, 0); // получение строки от клиента по сокету
    cout << "Message was received: " << subsequence_buf << endl;

    sort(subsequence_buf, subsequence_buf + sizeof(subsequence_buf)); // сортировка строки

    if (send(client_socket, subsequence_buf, 10, 0) > 0) { // отправка обратного сообщения
        cout << "Message sent: " << subsequence_buf << endl;
    } else {
        cout << "Error sending message";
    }
    exit(0);
}
