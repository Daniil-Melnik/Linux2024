# Linux2024
Лабораторные работы по дисциплине "Организация процессов и программирование в среде Linux"

## Лабораторная работа №3
Порядок выполнения работы
1. Разработать программу, которая порождает 2 потомка. Первый потомок порождается с помощью fork, второй – с помощью vfork с последующей заменой на другую программу. Все 3 процесса должны вывести в один
файл свои атрибуты с предварительным указанием имени процесса (например: Предок, Потомок1, Потомок2). Имя выходного файла задается при запуске программы. Порядок вывода атрибутов в файл должен определяться
задержками процессов, которые задаются в качестве параметров программы
и выводятся в начало файла.
2. Откомпилировать программу и запустить ее 3 раза с различными сочитаниями задержек.
Запуск вида: `<путь к exe>/main <файл выхода> <задержка 1> <задержка 2> <задержка 3>`
Пример запуска: `./main test.txt 1 2 3`
