#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

using namespace std;

int main(int argc, char* argv[]) {
    if (argc != 2) { // проверка корректности переменных
        cout << "Missing arg timeout: ./server {sec}" << endl;
        exit(0);
    }

    int client_socket; // сокет для прослушки клиентов
    int server_socket; // сокет на стороне сервера - принимающий
    sockaddr_in socket_address_in; // адресс взаимодействия по сокетам
    timeval time_value;
    fd_set fds;
    char socket_client_name[255];

    server_socket = socket(AF_INET, SOCK_STREAM, 0); // создание серверного сокета
    if (server_socket < 0) {
        perror("Error creating socket");
        exit(1);
    }

    socket_address_in.sin_family = AF_INET; // заполнение параметров адресного взаимодействия
    socket_address_in.sin_port = htons(3434);
    socket_address_in.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (bind(server_socket, (sockaddr*) &socket_address_in, sizeof(socket_address_in)) < 0) { // привязка серверного сокета к адресу
        perror("Error bind socket");
        exit(2);
    }

    FD_ZERO(&fds); // очистка старых и установка новых дескрипторов серверного сокета
    FD_SET(server_socket, &fds);
    listen(server_socket, 5); // очередь из пяти привязочных сокетов

    while (true) {

        time_value.tv_sec = atoi(argv[1]); // читаем таймаут
        time_value.tv_usec = 0;

        if (select(FD_SETSIZE, &fds, nullptr, nullptr, &time_value) == 0) { // FD_SETSIZE - опрашиваем все возможные
            cout << "Timeout for client request has expired" << endl; // если нет откликов - конец работы
            break;
        } else {
            client_socket = accept(server_socket, nullptr, nullptr); // иначе смотрим привяязочный сокет
            if (client_socket < 0) { // привязка сломана
                perror("Error accept socket");
                exit(1);
            }

            sprintf(socket_client_name, "%d", client_socket); // форматирование клиентского сокета в строку для передачи далее
            pid_t process_id = fork(); // создание вспомогательной программы

            if (process_id == 0) {
                execl("prog", "prog", socket_client_name, nullptr);
            }
        }

    }

    close(server_socket);
    return 0;
}
