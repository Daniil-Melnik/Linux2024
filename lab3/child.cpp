#include <iostream>
#include <fstream>
#include <unistd.h>

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
    cout << "запущен отд. файл" << endl;
    char* fileName = argv[0];
    int delay = atoi(argv[1]);
    sleep(delay);
    outFile(fileName, "child_2");
    cout << "окончен отд. файл" << endl;
    return 0;
}
