#include <iostream>
#include <fstream>
#include <fcntl.h>
#include <time.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <wait.h>

using namespace std;

char ch[50];


int main(int argc, char * argv[]) {
    pid_t pid1, pid2;

    // Установка перехватчиков
    struct sigaction sa;
    sa.sa_handler = SIG_IGN; // Игнорирование сигналов
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    sigaction(SIGUSR1, &sa, NULL); // реакции на сигналы
    sigaction(SIGUSR2, &sa, NULL);
    sigaction(SIGQUIT, &sa, NULL);

    int fildes[2]; // fildes[0] -- чтение
                   // fildes[1] -- запись

    if(pipe2(fildes, O_NONBLOCK) == -1) // ошибка создания канала
    {
        cout << "pipe error\n";
        exit(1);
    }
    cout <<"pipe success\n";

    // Создание потомка 1
    pid1 = fork();
    if (pid1 == 0) {
        execl("./child1", "child1", &fildes[0], &fildes[1], "output1.txt", (char*)NULL);
    }
    else {
        // Создание потомка 2
        pid2 = fork();
        if (pid2 == 0){
            execl("./child2", "child2", &fildes[0], &fildes[1], "output2.txt", (char*)NULL);
        }
        else{
            cout << pid1 << " " << pid2 << endl; // вывод PID потомков
            if (pid1 > 0 && pid2 > 0){
                cout << "Both process started" << endl;
            }

            ifstream file(argv[1]); // входной файла данных
            file.getline(ch, 50); // чтение первой строки
            write(fildes[1], ch, strlen(ch) - 1); // запись строк в канал

            file.getline(ch, 50); // чтение второй строки
            while (strlen(ch) != 0){ // пока строки не кончатся
                write(fildes[1], ch, strlen(ch) - 1);
                file.getline(ch, 50);
               // cout << "while success "  << strlen(ch) << " *"<< endl;
            }

            //usleep(300000);
            kill(0, SIGQUIT);


            // cout << "pipe written ALL" << endl;

            file.close();


            int status;
            waitpid(pid1, &status, 0); // ожидание потомков
            waitpid(pid2, &status, 0);

            cout << "parent end" << endl;
        }
    }

    return 0;
}
