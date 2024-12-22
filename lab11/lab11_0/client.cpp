#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

using namespace std;

int main(int argc, char* argv[]) {

    if (argc != 2){
        cout << "Missing arg timeout: ./server {sec}" << endl;
        exit(0);
    }

    int client_socket;
    int begin_time;
    int is_connected = 0;
    char message_send[10]; // отправляемое сообщение
    char message_receive[10]; // получаемое обработанное сообщение
    fd_set readfds;
    sockaddr_in server_socket_add; // адрес серверного сокета
    timeval time_value; // таймаут

    const int timeout = atoi(argv[1]);
    srand(time(nullptr));

    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0) {
        perror("Error creating socket");
        exit(1);
    }

    server_socket_add.sin_family = AF_INET; // настройка параметров серверного сокета
    server_socket_add.sin_port = htons(3434);
    server_socket_add.sin_addr.s_addr = inet_addr("127.0.0.1");

    begin_time = time(nullptr); // время начала

    while ((time(nullptr) - begin_time) < timeout
    && (is_connected = connect(client_socket, (sockaddr*) &server_socket_add, sizeof(server_socket_add))) < 0) // попытка соединения между сокетами
    {}

    if (is_connected == -1) {
        cout << "Timeout for connect to server has expired" << endl;
        close(client_socket);
        exit(-1);
    }

    for (int i = 0; i < 10; i++) { // генерация десятизначной строки
        message_send[i] = '0' + rand() % 10;
    }

    time_value.tv_sec = timeout; // формирование таймаута
    time_value.tv_usec = 0;

    FD_ZERO(&readfds);
    FD_SET(client_socket, &readfds);

    setsockopt(client_socket, SOL_SOCKET, SO_SNDTIMEO, (const char*)&time_value, sizeof(time_value)); // установка таймаута для сокета

    send(client_socket, message_send, sizeof(message_send), 0); // отправка сообщения

    cout << "Message sent: " << message_send << endl;

    if (select(FD_SETSIZE, &readfds, nullptr, nullptr, &time_value) == 0) { // проверка реакции всех сокетов
        cout << "Timeout for server response has expired" << endl; // ответов нет - конец
    } else {
        recv(client_socket, message_receive, sizeof(message_receive), 0); // получение ответа
        cout << "Message was received: " << message_receive << endl;
    }

    close(client_socket);
    return 0;
}
