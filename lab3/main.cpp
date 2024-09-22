#include <iostream>
#include <fstream>
#include <unistd.h>
#include <sys/wait.h>

using namespace std;

void outFile(char* fileName, string processName){
    ofstream out;
    out.open(fileName, ios::app);
    if (out.is_open()) {
        pid_t pid = getpid();
        out << processName << ": Process ID    = " << pid << endl;
        out << processName << ": Parent ID     = " << getppid() << endl;
        out << processName << ": Session ID    = " << getsid(pid) << endl;
        out << processName << ": GID           = " << getpgid(pid) << endl;
        out << processName << ": Real UID      = " << getuid() << endl;
        out << processName << ": Effect UID    = " << geteuid() << endl;
        out << processName << ": Real GID      = " << getgid() << endl;
        out << processName << ": Effect GID    = " << getegid() << endl;
        out << "=======================================================\n";
        out.close();
    }
}

int main(int argc, char *argv[]) {
    bool cond = (argc == 5);
    if (cond){
        char* exeFileName = argv[0]; // имя текущего исполняемого файла
        char* fileName = argv[1];    // имя файла с логами
        int sleep_1 = atoi(argv[2]); // время ожидания процесса fork
        int sleep_2 = atoi(argv[3]); // время ожидания процесса vfork
        int sleep_3 = atoi(argv[4]); // время ожидания родительского процесса

        // 1-й процесс-потомок через fork
        int first_ch = fork();

        if (first_ch < 0)
            cout << "ОШИБКА: не удалось fork\n";
        else if (first_ch == 0) {
            cout << "запущен fork" << endl;
            sleep(sleep_1);
            outFile(fileName, "child_1");
            cout << "окончен fork" << endl;
        }
        else {
            int second_ch = vfork();
            if (second_ch < 0)
                cout << "ОШИБКА: не удалось vfork\n";

            // 2-й процесс-потомок через vfork
            else if (second_ch == 0){
                if (execl("./child", fileName, argv[3], NULL) < 0){
                    exit(1);
                }
            }

            // родительский процесс
            else {
                sleep(sleep_3);
                outFile(fileName, "parent");
                if (waitpid(first_ch, 0, 0) < 0){
                    cout << "ОШИБКА: невозможно дождаться fork-процесс\n";
                    exit(1);
                }
            }
        }
    return 0;

    }
    else {
        cout << "ОШИБКА: Кол-во параметров д. б. равным 5" << endl;
        return -1;
    }
}
