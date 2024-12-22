#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

using namespace std;

int main(int argc, char* argv[]) {
    int skClient; // сокет для прослушки клиентов
    int skServer; // сокет на стороне сервера - принимающий
    sockaddr_in socketInAddress; // адресс взаимодействия по сокетам
    timeval timeoutVal;
    fd_set fds;
    char skClientStr[255];

    skServer = socket(AF_INET, SOCK_STREAM, 0); // создание серверного сокета
    if (skServer < 0) {
        perror("ERROR creating socket");
        exit(1);
    }

    socketInAddress.sin_family = AF_INET; // заполнение параметров адресного взаимодействия
    socketInAddress.sin_port = htons(3434);
    socketInAddress.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (bind(skServer, (sockaddr*) &socketInAddress, sizeof(socketInAddress)) < 0) { // привязка серверного сокета к адресу
        perror("ERROR bind socket");
        exit(2);
    }

    FD_ZERO(&fds); // очистка старых и установка новых дескрипторов серверного сокета
    FD_SET(skServer, &fds);
    listen(skServer, 5); // очередь из пяти привязочных сокетов

    while (true) {

        timeoutVal.tv_sec = atoi(argv[1]); // читаем таймаут
        timeoutVal.tv_usec = 0;

        if (select(FD_SETSIZE, &fds, nullptr, nullptr, &timeoutVal) == 0) { // FD_SETSIZE - опрашиваем все возможные
            cout << "Timeout client expire" << endl; // если нет откликов - конец работы
            break;
        } else {
            skClient = accept(skServer, nullptr, nullptr); // иначе смотрим привяязочный сокет
            if (skClient < 0) { // привязка сломана
                perror("ERROR accept socket");
                exit(1);
            }

            sprintf(skClientStr, "%d", skClient); // форматирование клиентского сокета в строку для передачи далее
            pid_t chPID = fork(); // создание вспомогательной программы

            if (chPID == 0) {
                execl("serverExec", "serverExec", skClientStr, nullptr);
            }
        }

    }

    close(skServer);
    return 0;
}
