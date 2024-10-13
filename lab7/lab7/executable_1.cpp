// WARNING: create file in Linux to have "\n" ending (LF) instead of "\r\n" (CRLF) in Windows file
// LF -- line feed ("\n")
// CRLF -- carriage return line feed ("\r\n")

#include <iostream>
#include <fstream>
#include <unistd.h>
#include <signal.h>

using namespace std;

// false -- запись в канал НЕ завершена
// true -- запись в канал завершена
bool pipe_write_is_finished = false; // индикатор конца записи
void LocalHandler (int local_int);

int main(int argc, char *argv[])
{
    //FILE* output_file_1 = fopen(argv[3], "w"); // открытие файла с флагом записи
    int sig; // для sigwait
    //int pipe_read_is_done = 1; // if > 0, канал не готов для чтения, иначе готов
    //int fildes[2]; // обработчики каналов, fildes[0] -- чтение из канала, fildes[1] -- запись в канал
    //char ch; // символьный буфер

    sigset_t b_set;
    sigset_t set;
    struct sigaction sigact;
    int cnt = 0;

    //fildes[0] = *argv[1];
    //fildes[1] = *argv[2];

    sigaddset(&set, SIGUSR1); // доабвление SIGUSR1 к маске сигналов для 1 дочернего процесса
    sigact.sa_handler = &LocalHandler; // установка обработчика
    sigaction(SIGQUIT, &sigact, NULL); // изменение реакции функции на SIGQUIT
    sigaddset(&b_set, SIGQUIT); // добавление SIGQUIT к маске сигналов
    sigprocmask(SIG_UNBLOCK, &b_set, NULL); // unblock SIGQUIT w/ set signals reaction????

    cout << "---------- CHILD PROCESS 1 BEGINS WRITING DATA F/ THE PIPE TO FILE ----------\n"; //???

    //pipe_read_is_done = read(fildes[0], &ch, 1); // чтение из канала
    // pipe finish condition is in priority -- if pipe writing is not finished, this program MUST wait
    // условие завершения работы канала в приоритете - если запись не закончена, эта программа ДОЛЖНА подождать
    while (!pipe_write_is_finished || (cnt < 5))
    {//пока канал занят записью или запись не закончена
        //if (pipe_read_is_done > 0)
        //{//дополнительная проверка на завершение работы записи в канал (в пред. условии было ||, а не &&)
            //fputc(ch, output_file_1); // запись в файл
            //cout << "proc1 >> " << ch << endl;
            cout << "proc1 - " << cnt << endl;
            cnt++;
            kill(0, SIGUSR2); // дочерний процесс 1 дает сигнал дочернему процессу 2
            //sigwait(&set, &sig); //дочерний процесс 1 ожидает дочерний процесс 2
            pause();
        //}
        //pipe_read_is_done = read(fildes[0], &ch, 1); // чтение из канала
    }
    kill(0, SIGUSR2); // дочерний процесс 1 дает сигнал дочернему процессу 2 ПЕРЕД уничтожением
    //fclose(output_file_1); // закрытие выходного файла
    //close(fildes[0]); // закрытие канала для записи
    exit(0);
}
void LocalHandler (int local_int)
{
    cout << "handler 1" << endl;
    pipe_write_is_finished = true; // запись в канал завершена
}
