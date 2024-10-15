# Linux2024
Лабораторные работы по дисциплине "Организация процессов и программирование в среде Linux"

## Лабораторная работа №3
Порядок выполнения работы
1. Разработать программу, которая порождает 2 потомка. Первый потомок порождается с помощью fork, второй – с помощью vfork с последующей заменой на другую программу. Все 3 процесса должны вывести в один
файл свои атрибуты с предварительным указанием имени процесса (например: Предок, Потомок1, Потомок2). Имя выходного файла задается при запуске программы. Порядок вывода атрибутов в файл должен определяться
задержками процессов, которые задаются в качестве параметров программы
и выводятся в начало файла.
2. Откомпилировать программу и запустить ее 3 раза с различными сочитаниями задержек.
- Запуск вида: `<путь к exe>/main <файл выхода> <задержка 1> <задержка 2> <задержка 3>`
- Пример запуска: `./main test.txt 1 2 3`

## Лабораторная работа №4
Порядок выполнения работы
1. Написать программу, которая открывает входной файл и 2 выходных файла. Затем она должна в цикле построчно читать входной файл и порождать 2 потока. Одному потоку передавать нечетную строку, а другому – четную. Оба потока должны работать параллельно. Каждый поток
записывает в свой выходной файл полученную строку и завершает работу. Программа должна ожидать завершения работы каждого потока и повторять цикл порождения потоков и чтения строк входного файла, пока не прочтет последнюю строку, после чего закрыть все файлы.
2. Откомпилировать программу и запустить ее.
- Запуск вида: `<путь к exe>/main_2`
- Пример запуска: `./main_2`

## Лабораторная работа №5
Порядок выполнения работы
1) Написать программу, которая реагирует на ошибки при выполнении операции деления и неверном использовании указателя (деление на ноль, нарушение защиты памяти). При обнаружении ошибки программа должна передать управление функции, которая выведет сообщение и завершит работу программы с кодом ошибки (1 или 2). Тип ошибки, который должна зафиксировать программа, задается как параметр при ее запуске.
2) Откомпилировать программу и дважды запустить ее с разными значениями типа ошибки.
- Запуск вида: `<путь к exe>/main <тип ошибки>`
- Пример запуска: `./main 2`

## Лабораторная работа №6
Порядок выполнения работы
1) Написать периодическую программу, в которой период запуска и количество запусков должны задаваться в качестве ее параметров. При каждом очередном запуске программа должна порождать новый процесс, который выводить на экран свой идентификатор, дату и время старта. Программа и ее дочерний процесс должны быть заблокированы от завершения при нажатии клавиши Ctrl/z. После завершения дочернего процесса программа должна вывести на экран информацию о времени своей работы и дочернего процесса.
2)  Откомпилировать программу и запустить ее.
- Запуск вида: `<путь к exe>/main <период> <кол-во запусков>`
- Пример запуска: `./main 2 5`

## Лабораторная работа №7
Порядок выполнения работы
1)Написать программу, которая обменивается данными через канал с двумя потомками. Программа открывает входной файл, построчно читает из него данные и записывает их в канал. Потомки выполняют свои программы и поочередно читают символы из канала и записывают их в свои выходные файлы: первый потомок – нечетные символы, а второй – четные. Синхронизация работы потомков должна осуществляться напрямую с использованием сигналов SIGUSR1 и SIGUSR2. Об окончании записи файла в канал программа оповещает потомков сигналом SIGQUIT и ожидает завершения работы потомков. Когда они заканчивают работу, программа закрывает канал.
2) Откомпилировать программу и запустить ее.

