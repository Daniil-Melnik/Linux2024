#include <iostream>
#include <fstream>
#include <pthread.h>
#include <sched.h> // Для работы с приоритетами и классами планирования
#include <unistd.h> // Для проверки состояния файла

using namespace std;

// Функция потока
static void *threadFunc(void *arg) {
    fstream *file = static_cast<fstream*>(arg);

    // Получение планировщика и приоритетов
    int policy;
    struct sched_param param;
    pthread_getschedparam(pthread_self(), &policy, &param);

    // Вывод информации о планировании и приоритетах
    cout << "Планировщик: " << (policy == SCHED_OTHER ? "SCHED_OTHER" : policy == SCHED_RR ? "SCHED_RR" : "SCHED_FIFO") << endl;
    cout << "Текущий приоритет: " << param.sched_priority << endl;
    cout << "Минимальный приоритет: " << sched_get_priority_min(policy) << endl;
    cout << "Максимальный приоритет: " << sched_get_priority_max(policy) << endl;

    // Чтение содержимого файла
    string line;
    cout << "Содержимое файла:" << endl;
    if (file->is_open()) {
        while (getline(*file, line)) {
            cout << line << endl;
        }
        //file->close(); // удалить, чтобы поток не закрывал файл
    } else {
        cout << "Ошибка открытия файла." << endl;
    }

    pthread_exit(0);
}

int main() {
    pthread_t thread;
    pthread_attr_t attr;
    fstream inFile;

    // Открытие файла
    inFile.open("test.txt");
    if (!inFile.is_open()) {
        cerr << "Не удалось открыть файл test.txt" << endl;
        return 1;
    }

    // Инициализация атрибутов потока
    pthread_attr_init(&attr);

    // Создание потока
    pthread_create(&thread, &attr, &threadFunc, &inFile);

    // Ожидание завершения потока
    pthread_join(thread, NULL);

    // Проверка состояния файла
    if (!inFile.is_open()) {
        cout << "Файл закрыт." << endl;
    } else {
        cout << "Файл все еще открыт. Закрываю принудительно..." << endl;
        inFile.close();
    }

    // Вывод текущего приоритета в главном потоке
    int policy;
    struct sched_param param;
    pthread_getschedparam(pthread_self(), &policy, &param);
    cout << "Текущий приоритет главного потока: " << param.sched_priority << endl;

    return 0;
}
