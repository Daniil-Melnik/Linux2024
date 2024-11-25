#include <iostream>
#include <fstream>
#include <pthread.h>
#include <unistd.h>
using namespace std;

ifstream input;
ofstream output1, output2;

static void * threadFunc1(void *arg){
    string line = *((string*) arg);
    output1 << line << endl;
    // cout << "Write line 1" << endl;
    // usleep(1000000);
    pthread_exit(0);
}

static void * threadFunc2(void *arg){
    string line = *((string*) arg);
    output2 << line << endl;
    // cout << "Write line 2" << endl;
    // usleep(1000000);
    pthread_exit(0);
}

int main(){
    input.open("input.txt");
    int lCnt = 0;
    bool cond1 = true, cond2 = true;

    pthread_t thread_1, thread_2;
    pthread_attr_t attr;
    string line1, line2;

    if(!input.is_open()){
        perror("Не удалось открыть входной файл");
        exit(1);
    }

    input.seekg(0);
    output1.open("output1.txt");
    output2.open("output2.txt");

    pthread_attr_init(&attr);
    while(cond1 && cond2){
        cond1 = true;
        cond2 = true;

        if(getline(input, line1))
            pthread_create(&thread_1, &attr, &threadFunc1, &line1);
        else cond1 = false;

        if(getline(input, line2))
            pthread_create(&thread_2, &attr, &threadFunc2, &line2);
        else cond2 = false;
        pthread_join(thread_1, NULL);
        pthread_join(thread_2, NULL);
    }
    pthread_attr_destroy(&attr);
    input.close();
    output1.close();
    output2.close();

    return 0;
}
