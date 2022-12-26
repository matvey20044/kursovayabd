#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <locale.h>

/*
• Производитель – это строка, содержащая заглавные и строчные буквы.
• Диагональ – это строка, содержащая вещественное числа.
• Разрешение – это строка, содержащая числа и букву.
• Тип Матрицы - строка, содержащая заглавные и строчные буквы.
• Изогнутый экран +\- - строка содержащая знаки (+\-).
• Разъем HDMI – строка, содержащая строчные буквы(да\нет).

 Ввод и вывод названия файла на экран;
– Выход из программы путем нажатия определенной клавиши +
– Ввод данных путем нажатия определенной клавиши +
– Вывод данных путем нажатия определенной клавиши +
– Сохранение данных в файле путем нажатия определенной клавиши +
– Считывание данных путем нажатия определенной клавиши +
– Поиск по диагонали(дюймы) +
– Поиск по типу матрицы +
– Сортировка данных по производителям(алфавитный порядок)
- Вывод в файл +

В задании требуется выполнить поиск по диагонали(дюймы), поиск по типу матрицы. Будет выполняться он следующим образом: ввод данных  сопоставление с данными из файла, при нахождения соответствия производится вывод данных.
Согласно заданию необходимо реализовать сортировку массива по наименованию производителя(в алфавитном порядке от а до я).
 */
#define PRODUCER_NAME 30
#define RES_LEN 20
#define MAX_ROWS 30
#define MAX_FILENAME 30
#define MATRIX_LEN 30
#define DIAG_LEN 9
#define EPS 0.00001

#define OK 0
#define ERR_MONITOR 1
#define ERR_HDMI 2
#define ERR_CURED 3
#define ERR_RES 4
#define ERR_DIAG 5
#define ERR_OVERFLOW 6
#define ERR_EMPTY_FILE 7
#define ERR_MEM 8
#define ERR_MATRIX_LEN 9

typedef struct
{
    char producer[PRODUCER_NAME];
    double diag;
    char matrix[MATRIX_LEN];
    char res[RES_LEN];
    int cured;
    int hdmi;
} monitor_t;

// Сортировка по значению диагонали
void sort(monitor_t* monitor, int size);
// Функцию удаляет перевод строки
void delete_enter(char* str);
// Функция для считывания строки из потока ввода
size_t read_line(char* dst, size_t n);
// Функция для считывания целого числа из потока ввода
int read_number(int* dst, size_t n);
// Функция для считывания вещественного числа из потока ввода
int read_double(double* dst, size_t n);
// Добавление записи в БД с клавиатуры
void add(monitor_t* monitor, int* len);
// Считвание одной записи из файла
int read_node(FILE* stream, monitor_t* monitor);
// Считывание БД из файла
int read_data(FILE* stream, monitor_t* monitor, int* len);
// Поиск по диагонали
void search_by_diag(monitor_t* monitors, int len, monitor_t* found, int* found_len, double sample);
// Поиск по типу матрицы
void search_by_matrix(monitor_t* monitors, int len);
// Вывод БД на экран
void output(monitor_t* monitors, int len);
// Меню
int menu(int len);
// Вывод БД в файл
void output_file(FILE* stream, monitor_t* monitors, int len);
// Компаратор
int cmp_diag(const void* left_, const void* right_);
// Сортировка по производителям
void sort(monitor_t* monitor, int size);
// Функция открывает файл и вызывает функцию read_data
void read_file(char* filename, monitor_t* monitor, int* len);
// Функция открывает файл и вызывает функцию output_file
void output_file_ex(char* filename, monitor_t* monitor, int len);


int main() {
    setlocale(LC_ALL, "RUS");
    int len = 0;
    char filename[MAX_FILENAME] = "";
    monitor_t* arr = malloc(sizeof(monitor_t) * MAX_ROWS);
    if (!arr)
    {
        puts("Ошибка выделения памяти");
        return ERR_MEM;
    }
    while (1)
    {
        int choice = menu(len);
        if (choice == 0)
            break;
        else if (choice == 1) // Ввод из файла
        {
            printf("Введите имя файла: ");
            read_line(filename, MAX_FILENAME);
            delete_enter(filename);
            printf("Имя файла: %s\n", filename);
            read_file(filename, arr, &len);
        }
        else if (choice == 2)
        {
            if (!len)
            {
                printf("БД пуста\n");
                continue;
            }
            output(arr, len);
        }
        else if (choice == 3)
        {
            if (!len)
            {
                printf("БД пуста\n");
                continue;
            }
            double search;
            printf("Введите значение диагонали: ");
            read_double(&search, 10);

            monitor_t* found = malloc(MAX_ROWS * sizeof(monitor_t));
            int found_len = 0;
            search_by_diag(arr, len, found, &found_len, search);
            printf("Найдено %d записей\n", found_len);
            if (found_len)
                output(found, found_len);
        }
        else if (choice == 4)
        {
            add(arr, &len);
        }
        else if (choice == 5)
        {
            if (!len)
            {
                printf("БД пуста\n");
                continue;
            }
            search_by_matrix(arr, len);
        }
        else if (choice == 6)
        {
            if (!len)
            {
                printf("БД пуста\n");
                continue;
            }

            printf("Введите имя файла: ");
            read_line(filename, MAX_FILENAME);
            printf("Имя файла: %s\n", filename);
            delete_enter(filename);
            output_file_ex(filename, arr, len);
        }
        else if (choice == 7)
        {
            if (!len)
            {
                printf("БД пуста\n");
                continue;
            }
            sort(arr, len);
        }
    }
    return OK;
}

void read_file(char* filename, monitor_t* monitor, int* len)
{
    FILE* stream = fopen(filename, "r");
    if (!stream)
    {
        fprintf(stderr, "Файл не найден.");
        return;
    }
    read_data(stream, monitor, len);
    fclose(stream);
}

void output_file_ex(char* filename, monitor_t* monitor, int len)
{
    FILE* stream = fopen(filename, "w");
    if (!stream)
    {
        fprintf(stderr, "Ошибка.");
        return;
    }
    output_file(stream, monitor, len);
    fclose(stream);
    printf("Данные записаны!\n");
}

void delete_enter(char* str)
{
    if (!str)
        return;
    if (str[strlen(str) - 1] == '\n')
        str[strlen(str) - 1] = '\0';
}

size_t read_line(char* dst, size_t n)
{
    int ch;
    size_t i = 0;
    while ((ch = getchar()) != '\n' && ch != EOF)
    {
        if (i < n)
            dst[i++] = ch;
    }
    dst[i] = '\0';
    return i;
}

int read_number(int* dst, size_t n)
{
    char chars[MAX_FILENAME + 1];
    size_t num = read_line(chars, n + 1);
    if (num == 0 || num > n)
        return -1;
    char* end_ptr;
    *dst = (int)strtol(chars, &end_ptr, 10);

    if (*end_ptr)
        return -1;
    return OK;
}

int read_double(double* dst, size_t n)
{
    char chars[MAX_FILENAME + 1];
    size_t num = read_line(chars, n + 1);
    if (num == 0 || num > n)
        return -1;
    char* end_ptr;
    *dst = (double)strtod(chars, &end_ptr);

    if (*end_ptr)
        return -1;
    return OK;
}

void add(monitor_t* monitor, int* len)
{
    if (*len == MAX_ROWS)
    {
        printf("БД заполнена. Запись невозможна.");
        return;
    }
    monitor_t monitor_new;

    printf("Введите название производителя (до %d символов): ", PRODUCER_NAME - 1);
    read_line(monitor_new.producer, PRODUCER_NAME);


    printf("Введите диагональ (до %d символов): ", DIAG_LEN);
    read_double(&monitor_new.diag, DIAG_LEN);

    printf("Введите матрицу (до %d символов): ", MATRIX_LEN - 1);
    read_line(monitor_new.matrix, MATRIX_LEN);

    printf("Введите разрешение (до %d символов): ", RES_LEN - 1);
    read_line(monitor_new.res, RES_LEN);

    printf("Изогнутый? (1 - да / 0 - нет) (до %d символов): ", 1);
    read_number(&monitor_new.cured, 1);

    printf("Есть hdmi? (1 - да / 0 - нет) (до %d символов): ", 1);
    read_number(&monitor_new.hdmi, 1);

    (*len)++;
    monitor[*len - 1] = monitor_new;
    printf("Запись успешно добавлена!\n");
}

int read_node(FILE* stream, monitor_t* monitor)
{
    int rc;
    if (!fgets(monitor->producer, PRODUCER_NAME - 1, stream))
    {
        puts("Ошибка при чтении названия монитора.");
        return ERR_MONITOR;
    }
    if ((rc = fscanf(stream, "%lf\n", &monitor->diag)) != 1 || monitor->diag <= 0)
    {
        puts("Ошибка при чтении диагонали.");
        return ERR_DIAG;
    }
    if (!fgets(monitor->res, RES_LEN - 1, stream))
    {
        puts("Ошибка при чтении разрешения.");
        return ERR_RES;
    }
    if (!fgets(monitor->matrix, MATRIX_LEN - 1, stream))
    {
        puts("Ошибка при чтении матрицы.");
        return ERR_MATRIX_LEN;
    }
    if (fscanf(stream, "%d\n", &monitor->cured) != 1)
    {
        puts("Ошибка при чтении параметра изогнутости экрана.");
        return ERR_CURED;
    }
    if (fscanf(stream, "%d\n", &monitor->hdmi) != 1) //TODO проверка +/-
    {
        puts("Ошибка при чтении параметра изогнутости экрана.");
        return ERR_HDMI;
    }
    delete_enter(monitor->producer);
    delete_enter(monitor->res);
    delete_enter(monitor->matrix);
    return OK;
}

int read_data(FILE* stream, monitor_t* monitor, int* len)
{
    if (fscanf(stream, "%d\n", len) != 1)
    {
        puts("Файл пустой.");
        return ERR_EMPTY_FILE;
    }
    if (*len > MAX_ROWS)
    {
        puts("Ошибка. В файле больше записей, чем БД может принять");
        return ERR_OVERFLOW;
    }

    for (int i = 0; i < *len; i++)
    {
        monitor_t monitor_tmp;
        int rc = read_node(stream, &monitor_tmp);
        if (rc != OK)
            return rc;
        monitor[i] = monitor_tmp;
    }
    return OK;
}

void search_by_diag(monitor_t* monitors, int len, monitor_t* found, int* found_len, double sample)
{
    *found_len = 0;
    for (int i = 0; i < len; i++)
    {
        if (fabs(monitors[i].diag - sample) <= EPS)
            found[(*found_len)++] = monitors[i];
    }
}

void search_by_matrix(monitor_t* monitors, int len)
{
    char search[MATRIX_LEN];
    printf("Введите значение матрицы: ");
    read_line(search, MATRIX_LEN);
    int counter = 0;
    for (int i = 0; i < len; i++)
    {
        if (!strcmp(search, monitors[i].matrix))
        {
            counter++;
            if (counter == 1)
            {
                printf("-----------------------------------------------------------------------------------------------------------------\n");
                printf("|Производитель                  |Диагональ|Разрешение          |Матрица                       |Изогнутый? |HDMI?|\n");
                printf("|-------------------------------|---------|--------------------|------------------------------|-----------|-----|\n");
            }
            printf("|%-31s|%-9g|%-20s|%-30s|%-11d|%-5d|\n", monitors[i].producer, monitors[i].diag, monitors[i].res, monitors[i].matrix, monitors[i].cured, monitors[i].hdmi);
        }
    }
    if (counter)
        printf("-----------------------------------------------------------------------------------------------------------------\n");
    printf("Найдено %d записей\n", counter);

}

void output(monitor_t* monitors, int len)
{
    printf("-----------------------------------------------------------------------------------------------------------------\n");
    printf("|Производитель                  |Диагональ|Разрешение          |Матрица                       |Изогнутый? |HDMI?|\n");
    printf("|-------------------------------|---------|--------------------|------------------------------|-----------|-----|\n");
    for (int i = 0; i < len; i++)
    {
        printf("|%-31s|%-9g|%-20s|%-30s|%-11d|%-5d|\n", monitors[i].producer, monitors[i].diag, monitors[i].res, monitors[i].matrix, monitors[i].cured, monitors[i].hdmi);
        if (i != len - 1)
            printf("|-------------------------------|---------|--------------------|------------------------------|-----------|-----|\n");
    }
    printf("-----------------------------------------------------------------------------------------------------------------\n");
}

int menu(int len)
{
    printf("Меню:\n"
        "0. Выход\n"
        "1. Ввод БД из файла\n"
        "2. Вывод БД на экран\n"
        "3. Поиск по диагонали\n"
        "4. Добавить запись в БД\n"
        "5. Поиск по матрице\n"
        "6. Вывести БД в файл\n"
        "7. Отсортировать по производителю (алфавитный порядок)\n"
        "Записей в таблице: %d"
        "\n\nВыберите пункт: ", len);
    int choice;
    while (1)
    {
        if (read_number(&choice, 2) == OK)
            break;
        printf("Ошибка ввода.\nВыберите пункт меню: ");
    }
    printf("\n\n");
    return choice;
}

void output_file(FILE* stream, monitor_t* monitors, int len)
{
    fprintf(stream, "%d\n", len);
    for (int i = 0; i < len; i++)
        fprintf(stream, "%s\n%g\n%s\n%s\n%d\n%d\n", monitors[i].producer, monitors[i].diag, monitors[i].res, monitors[i].matrix, monitors[i].cured, monitors[i].hdmi);
}

int cmp_diag(const void* left_, const void* right_)
{
    monitor_t const* left = left_;
    monitor_t const* right = right_;
    return strcmp(left->producer, right->producer);
}

void sort(monitor_t* monitor, int size)
{
    qsort(monitor, size, sizeof(monitor_t), cmp_diag);
    printf("Сортировка завершена\n");
}

