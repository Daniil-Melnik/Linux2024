#include <iostream>
#include <signal.h>

using namespace std;

void zeroDivisonErrHandler(int sig)
{
    cout << "SIGFPE = " << sig << " \n";
    cout << "Ошибка деления на ноль. Возврат 1" << endl;
    exit(1);
}

void segmentationErrHandler(int sig)
{
    cout << "SIGSEGV = " << sig << endl;
    cout << "Ошибка сегментации. Возврат 2" << endl;
    exit(2);
}

int setSignalHandlers()
{
    __sighandler_t zeroDivisionErrSignal = signal(SIGFPE, zeroDivisonErrHandler);
    __sighandler_t segmentationErrSugnal = signal(SIGSEGV, segmentationErrHandler);

    if ( zeroDivisionErrSignal == SIG_ERR || segmentationErrSugnal == SIG_ERR){
        perror("Ошибка формирования перехватчиков");
        return EXIT_FAILURE;
    }
    return 0;
}

int main(int argc, char* argv[])
{
    if(setSignalHandlers() < 0)
        return EXIT_FAILURE;

    if(argc != 2){
        cout << "Неверное количество параметров" << endl;
        return EXIT_FAILURE;
    }

    int errorType= atoi(argv[1]);

    if(errorType == 1){
        int num = 1 / 0;
    }

    else if(errorType == 2){
        int* p = NULL;
        cout << *p << std::endl;
    }

    else{
        cout << "Кодом ошибки является 1 или 2" << endl;
        return EXIT_FAILURE;
    }

    return 0;
}

