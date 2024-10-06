#include <iostream>
#include <fstream>
#include <fcntl.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <wait.h>

using namespace std;

char ch[50];


void handler(int signum) {
    cout << "parent had signal" << signum << endl;
    sleep(1);
}


int main() {
    pid_t pid1, pid2;

    // Set up signal handlers
    struct sigaction sa;
    sa.sa_handler = SIG_IGN; // Ignore the signal
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    sigaction(SIGRTMIN + 1, &sa, NULL);
    sigaction(SIGRTMIN + 2, &sa, NULL);
    sigaction(SIGRTMIN + 3, &sa, NULL);
    /*ifstream file("test.txt");

    if (file.is_open()){

        cout << "file success : " << ch << endl;
    }
    else{
        cout << "file error" << endl;
        return -1;
    }*/


    int fildes[2]; // обработчики каналов, fildes[0] -- чтение из канала, fildes[1] -- запись в канал

    if(pipe2(fildes, O_NONBLOCK) == -1) //если программа не создала канал, тогда программа завершается
    {
        cout << "pipe error\n";
        exit(1);
    }
    cout <<"pipe success\n";

    // Create child 1
    pid1 = fork();
    if (pid1 == 0) {
        execl("./child2", "child2", &fildes[0], &fildes[1], (char*)NULL);
    }
    else {
        // Create child 2
        pid2 = fork();
        if (pid2 == 0){
            execl("./child1", "child1", &fildes[0], &fildes[1], (char*)NULL);
        }
        else{
            cout << pid1 << " " << pid2 << endl;
            if (pid1 > 0 && pid2 > 0){
                cout << "Both process started" << endl;
            }

            //sleep(10);
            ifstream file("test.txt");
            file.getline(ch, 50);
            write(fildes[1], ch, strlen(ch) - 1);

            file.getline(ch, 50);
            while (strlen(ch) != 0){
                write(fildes[1], ch, strlen(ch) - 1);
                file.getline(ch, 50);
                cout << "while success "  << strlen(ch) << " *"<< endl;
            }

            //kill(0, SIGRTMIN + 3);
            sleep(1);
            kill(0, SIGRTMIN + 3);


            cout << "pipe written ALL" << endl;

            file.close();


            int status;
            waitpid(pid1, &status, 0);
            waitpid(pid2, &status, 0);

            cout << "parent end" << endl;
        }
    }

    return 0;
}
