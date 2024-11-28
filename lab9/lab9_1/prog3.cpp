#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <sys/shm.h>

using namespace std;

int shared_mem_seg_ptr; //указатель на разделяемую память

struct LamportStruct //структура, объект которого является общим (видимым для всех!)
{
	bool choosing[3]; //массив со значениями, показывающий, какие процессы в данный момент заняты выбором
	int number[3]; //массив со значениями приоритетов каждого процесса
	//если у двух процессов одинаковый номер в очереди, то первее обслуживается тот, у кого меньший номер j (в цикле)
};

void controlInput(int argc, char *argv[]){
    if (argc != 3) // проверка кол-ва аргументов (М)
	{
		cout << "Syntax error. Not enough arguments, must be 2: \"./executable_0 interval_time number_of_strings\"!\n";
		exit(-1);
	}
	if (atoi(argv[1]) < 1)
	{
		cout << "Syntax error. Interval time to write file argument must be in range [1; +inf)!\n";
		exit(-1);
	}
	if (atoi(argv[2]) < 1)
	{
		cout << "Syntax error. Number of strings to write file argument must be in range [1; +inf)!\n";
		exit(-1);
	}
}

void checkStart(){
    cout << "for start press ENTER" << endl;
    getchar();
}

void getSharedMem(int key, bool shared_mem_seg_owner){
    shared_mem_seg_ptr = shmget(key, sizeof(LamportStruct), 0666 | IPC_CREAT | IPC_EXCL);

	if (shared_mem_seg_ptr != -1)
	{
		shared_mem_seg_owner = true;
		cout << "SHARED MEMORY SEGMENT HAS BEEN CREATED\n\n";
	}
	else
	{
		shared_mem_seg_ptr = shmget(key, sizeof(LamportStruct), 0666 | IPC_CREAT);
		if(shared_mem_seg_ptr == -1)
		{
			cout << "SHARED MEMORY SEGMENT HAS NOT BEEN OPENED\n\n";
			exit(-1);
		}
		else
		{
			cout << "SHARED MEMORY SEGMENT HAS BEEN OPENED\n\n";
		}
	}
}

int main (int argc, char *argv[])
{
	// ---------- Подготовка ----------

    controlInput(argc, argv);
    checkStart();

	int program_id = 2;
	int interval_time = atoi(argv[1]);
	int number_of_strings = atoi(argv[2]);
	int key = 420; // номер ключа для разделяемой памяти
	string filename = "shared_file.txt";

	bool shared_mem_seg_owner; //кто является владельцем разделяемой памяти (текущий ли процесс?)
	int local_token = -1; //номер текущего приоритета
	string written_string = "Written by program number " + to_string(program_id) + "\n";
	LamportStruct* shared_mem_seg; // необходимые для алгоритма данные

	cout << "---------- PROGRAM NUMBER " << program_id << " ----------\n";
	cout << "---------- OUTPUT FILENAME IS " << filename << " ----------\n";
	cout << "---------- INTERVAL TIME/NUMBER OF STRINGS: " << interval_time << "/" << number_of_strings << " ----------\n";
	cout << "---------- KEY IS " << to_string(key) << " ----------\n";

	// ---------- Создание/открытие разделяемой памяти ----------

	getSharedMem(key, shared_mem_seg_owner);

	// ---------- Включение разделяемой памяти в адресное пространство процесса ----------

	shared_mem_seg = (LamportStruct*)shmat(shared_mem_seg_ptr, 0, 0);

	// ---------- Алгоритм пекарни Лампорта ----------

	for (int i = 0; i < number_of_strings; i++) // Цикл с кол-вом вписываемых строк
	{
		// https://www.javatpoint.com/lamports-bakery-algorithm
        // все переменные в массиве choosing иниц. в false, а в массиве number -  в 0
		sleep(interval_time);
		shared_mem_seg->choosing[program_id] = true; // установка choosing[id] в true, чтобы другие программы знали, что данная программа занята выбором
		local_token = -1;

        //когда процесс хочет войти в критическую секцию, он выбирает больший номер приоритета, чем любой предыдущий номер
		for (int k = 0; k < 3; k++)
		{
			if (shared_mem_seg->number[k] > local_token)
			{
				local_token = shared_mem_seg->number[k];
			}
		}

		shared_mem_seg->number[program_id] = local_token + 1; // выбор наибольшего номера
		shared_mem_seg->choosing[program_id] = false; // после завершения выбора конкретный процесс помечает значение в false

		// ожидание других процессов
		for(int j = 0; j < 3; j++) // процесс входит в цикл для оценки состояния других процессов
		{
			// процесс «i» ждет, пока какой-либо другой процесс «j» не выберет номер своего приоритета
			while(shared_mem_seg->choosing[j] == true)
			{}
			//j от 0 до 3 - номера программ
			//цикл - ожидание, пока все остальные процессы присвоят себе номер (приоритет)
			//убеждаемся в том, что все выбрали номера

			// Когда поток хочет войти в критическую секцию, он должен проверить, может ли он это сейчас сделать.
			while((shared_mem_seg->number[j] != 0)//если у процесса существует какой-либо приоритет (по дефолту - 0)
			&& ((shared_mem_seg->number[j] < shared_mem_seg->number[program_id]) //само сравнение приоритета (если наш приоритет больше какой-либо программы, мы ждем в очереди)
			|| ((shared_mem_seg->number[j] == shared_mem_seg->number[program_id])//если у нас равные приоритеты (одни и те же номерки), то мы сравниваем не только по приоритету, но и по id этих процессов
			&& (j < program_id))))
			{}
			//см. в Википедии со сравнениями приоритетов
			//после ожидания заходим на новую итерацию цикла
			//после неудовлетворения условия в цикле процесс входит в критическую секцию
		}

		cout << "OPEN OUTPUT FILE \"" << filename << "\" BY PROCESS №" << program_id << " \n";
		ofstream local_file(filename, ios_base::app);
		cout << "WRITE STRING №" << i << " BY PROCESS №" << program_id << " \n";
		local_file << written_string;
		cout << "CLOSE OUTPUT FILE \"" << filename << "\" BY PROCESS №" << program_id << " \n\n";
		local_file.close();

		// когда процесс завершает выполнение своей критической секции, он сбрасывает свой приоритет в 0
		shared_mem_seg->number[program_id] = 0;
	}

	// ---------- Открепление разделяемой памяти от процесса ----------
	shmdt((void*)shared_mem_seg);

	if(shared_mem_seg_owner == true)
	{
		//Управление характеристиками разд. памяти (в данном случае, удаление)
		shmctl(shared_mem_seg_ptr, IPC_RMID, NULL);
		cout << "\nSHARED MEMORY SEGMENT HAS BEEN CLOSED\n";
	}

	return 0;
}
