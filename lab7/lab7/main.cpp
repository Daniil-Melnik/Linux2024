#include <iostream>
#include <fstream>
#include <fcntl.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <wait.h>

using namespace std;

int main(int argc, char *argv[])
{
    int fildes[2];// обработчики каналов, fildes[0] -- чтение из канала, fildes[1] -- запись в канал
    pid_t pid_1; // pid 1 дочернего процесса
    pid_t pid_2; // pid 2 дочернего процесса
    char ch[80]; // символьный буфер
    char *c = NULL; // индикатор конца файла
    sigset_t set; // маска из сигналов
    FILE* fp = NULL; // имя файла для чтения родителем

    fp = fopen(argv[1], "r"); // открытие файла с флагом для чтения
    if (!fp)
    {
        cout << "---------- FILE HAS NOT BEEN OPENED SUCCESSFULLY ----------\n";
        exit(2);
    }
    cout << "---------- FILE HAS BEEN OPENED SUCCESSFULLY ----------\n";

    //добавление синхр. сигналов к маске
    sigaddset(&set, SIGQUIT);
    sigaddset(&set, SIGUSR1);
    sigaddset(&set, SIGUSR2);
    sigprocmask(SIG_BLOCK, &set, NULL); // блокирование пред. сигналов

    // создание канала связи
    if(pipe2(fildes/*???*/, O_NONBLOCK) == -1) //если программа не создала канал, тогда программа завершается
    {
        cout << "---------- PIPE HAS NOT BEEN CREATED SUCCESSFULLY ----------\n";
        exit(1);
    }

    cout << "---------- PIPE HAS BEEN CREATED SUCCESSFULLY ----------\n";
    pid_1 = fork(); // создание 1 дочернего процесса
    if(pid_1 == 0)
    {
        cout << "---------- CHILD PROCESS 1 BEGINS ----------\n";
        close(fildes[1]); // закрытие канала для записи 1 дочерним процессом
        execl("executable_1", "executable_1", &fildes[0], &fildes[1], "output_file_1.txt", NULL);
    }
    else
    {
        pid_2 = fork(); // создание 2 дочернего процесса
        if(pid_2 == 0)
        {
            cout << "---------- CHILD PROCESS 2 BEGINS ----------\n";
            close(fildes[1]); // закрытие канала для записи 2 дочерним процессом
            execl("executable_2", "executable_2", &fildes[0], &fildes[1], "output_file_2.txt", NULL);
        }
    }
    close(fildes[0]); // закрытие канала для прочтения процессом-родителем
    cout << "---------- PARENT PROCESS BEGINS WRITING DATA TO THE PIPE ----------\n";
    c = fgets(ch, 80, fp);
    while(c)
    {
        write(fildes[1], ch, strlen(ch) - 1); // запись данных в канал
        c = fgets(ch, 80, fp);
    }
    cout << "---------- PARENT PROCESS ENDS WRITING DATA TO THE PIPE ----------\n";
    kill(pid_1, SIGQUIT);
    kill(pid_2, SIGQUIT);

    waitpid(pid_1, NULL, 0); // ожидание завершения 1 дочернего процесса
    cout << "---------- CHILD PROCESS 1 ENDS ----------\n";
    waitpid(pid_2, NULL, 0); // ожидание завершения 2 дочернего процесса
    cout << "---------- CHILD PROCESS 2 ENDS ----------\n";

    close(fildes[1]); // закрытие канала для записи процессом-родителем

    return 0;
}
