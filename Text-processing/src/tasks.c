#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <limits.h>
#include "tasks.h"
#include "additional.h"
#include "tasks.h"
#include <wchar.h>



void rmDuplicate(char* str) {
    char* start = str;
    char* tochka = NULL;

    // Подсчёт количества предложений
    while ((tochka = strchr(start, '.')) != NULL) {
        //Начиная с точки и до конца текста брать каждое предложение и сравнивать
        // if(*start == '\n'){printf("n");}
        // else{printf("%c", *start);}
        while(*start == ' '){start++;} // Исключили лидирующие пробелы
        while(*start == '\n'){start++;}
        // printL(start, tochka);

        char* startNext = tochka + 1;
        char* tochkaNext = startNext;
        while(((tochkaNext = strchr(startNext, '.')) != NULL)){
          while(*startNext == ' '){startNext++;} // Исключили лидирующие пробелы
          while(*startNext == '\n'){startNext++;}
          // Сравнить все символы без учета регистра, если кол-во символов в start отличается от кол-ва в startNext то это не дубль, переход к следующему
          int lenStart = tochka - start;
          int lenNext = tochkaNext - startNext;
        //   printL(start, tochka);
        //   printL(startNext, tochkaNext);
        //   printf("%d %d\n",lenStart, lenNext);
          int diff = lenStart - lenNext;
          if(diff != 0){ // Предложения разные
            startNext = tochkaNext + 1;
          }
          else{
            char *pch = start;
            char *pchNext = startNext;
            int isequal = 1;
            while(pch <= tochka && pchNext <= tochkaNext){ // is equal
              char ch1 = *pch;
              char ch2 = *pchNext;
              if(isalpha(*pch) && isalpha(*pchNext)){
                ch1 = tolower(ch1);
                ch2 = tolower(ch2);
                if(ch1 != ch2){
                  isequal = 0;
                  break;
                }
              }
              else if(*pch != *pchNext){ // Двое указателей не буквы, возможно цифры, знаки пунктуации, какая-то буква
                isequal = 0;
                break;
              }

              pch++;
              pchNext++;
            } // end while
            if(isequal){ // Удаляем дубль
              memmove(startNext, tochkaNext+1, strlen(tochkaNext+1) + 1);
            }
            else{ //Разные предложения
              startNext = tochkaNext + 1; // Сравниваем следующее предложение
            }
          }
        }
        start = tochka + 1;
    }
}

void option_0(){
    int N = 102400;
    char* str = (char*)malloc(N * sizeof(char));
    input(str);
    rmDuplicate(str);
    output(str);
    free(str);
}


void fun1(char* str) {
    char* start = str;
    char* tochka = str;
    while (*start == ' ') start++;
    while ( (tochka = strchr(start, '.')) != NULL)
    {
        // printL(start, tochka);

        while(*start == ' ') start++;
        // printf("%c ", *start);
        if (isalpha(*start)) {
            *start = toupper(*start);
            start++;

        }
        for (char* p = start; p <= tochka; p++) {
            if (isalpha(*p)) {
                *start = tolower(*p);
            }
            start++;
        }

        if(*(tochka + 1) == '\n'){
            start = tochka + 2;
        }
        else{
            start = tochka + 1;
        }
    }
}

void option_1(){
    int N = 102400;
    char* str = (char*)malloc(N * sizeof(char));
    input(str);
    rmDuplicate(str);
    fun1(str);
    output(str);
    free(str);
}


void fun2(char* str, const char* search) {
    char* start = str;
    char* finish = NULL;

    while ( (finish = strchr(start, '.')) != NULL)
    {
        *finish = '\0';
        //ищем в предложении слово 2018
        if (strstr(start, search) != NULL) {
            *finish = '.';
            //удалить предложение
            memmove( start, finish+1, strlen(finish+1) + 1);
            //printf("%s\n", finish + 1);
        }
        else {
            *finish = '.';
            start = finish + 1;
        }
    }
}


void option_2(){
    int N = 102400;
    char* str = (char*)malloc(N * sizeof(char));
    const char* search = "2018";
    input(str);
    rmDuplicate(str);
    fun2(str, search);
    output(str);
    free(str);
}

// Функция для нахождения указателей на начало и конец предложения
void find(char** start, char** finish, int index) {
    int count = 0;
    while ((*finish = strchr(*start, '.')) != NULL) {
        while (**start == ' ') (*start)++; // Убираем пробелы в начале предложения
        if (count == index) break; // Нашли нужное предложение
        *start = *finish + 1; // Переход к следующему предложению
        count++;
    }
}

int summary(char* start){
    int sum = 0;
    int has_digits = 0;
    for (char* pch = start; *pch != '\0'; pch++) {
        if (isdigit(*pch)) {
            sum += (*pch - '0');
            has_digits = 1;
        }
    }
    if (!has_digits) return INT_MAX;
    return sum;
}

int compare( const void *arg1, const void *arg2 )
{
   int summary1 = summary( * ( char** ) arg1);
   int summary2 = summary( * ( char** ) arg2);
   if(summary1 > summary2){
    return 1;
   }
   else if(summary1 < summary2){
    return -1;
   }
   return 0;
}

// Основная функция для сортировки предложений по сумме цифр
void fun3(char* str) {
    char* start = str;
    char* tochka = NULL;
    int count = 0;

    // Подсчёт количества предложений
    while ((tochka = strchr(start, '.')) != NULL) {
        count++;
        start = tochka + 1;
    }


    // Считаем сумму цифр в каждом предложении
    start = str;
    int index = 0;
    while ((tochka = strchr(start, '.')) != NULL) {
        summary(start); // Если сумма равна 0, назначаем INT_MAX
        index++;
        start = tochka + 1;
    }

    char** sentences = (char**)malloc(count * sizeof(char*));


    char* finish = NULL;
    start = str;
    int ind = 0;
    while ((finish = strchr(start, '.')) != NULL) {

        while (*start == ' '|| *start == '\n') start++; // Убираем пробелы в начале предложения
        sentences[ind] = start;

        *finish = '\0';
        // printf("%s\n", start);
        start = finish + 1; // Переход к следующему предложению

        ind++;
    }


    /* Sort remaining args using Quicksort algorithm: */
    qsort( (void *)sentences, (size_t)count, sizeof( char * ), compare );

    for(int j = 0; j < count; j++){
        printf("%s.\n", sentences[j]);
        sentences[j][strlen(sentences[j])] = '.';
    }
    free(sentences);
}

void option_3(){
    int N = 102400;
    char* str = (char*)malloc(N * sizeof(char));
    input(str);
    rmDuplicate(str);
    fun3(str);
    free(str);
}

void fun4(char* str) {
	char* start = str;

	char* finish = str + strlen(str);

	const char* search = "0123456789";

	//arr - если в массиве каждый элемент
	//больше или равен 1 - предложение вывести

	while ((finish = strchr(start, '.')) != NULL) {
		//по предложению от start до finish
		int arr[10] = {};
		int isPrint = 1;
		for (char* p = start; p < finish; p++)
		{
			if (strchr(search, *p)) {
				int index = *p - '0'; //0
				arr[index]++;
            }
		}
		for (int i = 0; i < 10; i++)
		{
			if (arr[i] == 0) {
				isPrint = 0;
                break;
            }
		}
		//Вывожу предложение
		if (isPrint) {
            while (*start == ' '|| *start == '\n') start++;
			printL(start, finish);
		}
		start = finish + 1;
	}
}

void option_4(){
    int N = 102400;
    char* str = (char*)malloc(N * sizeof(char));
    input(str);
    rmDuplicate(str);
    fun4(str);
    free(str);
}

void option_5(){
    char text[] = "0)Находит и удаляет все повторно встречающиеся предложения.\n"
                    "1)Преобразовывает предложения так, чтобы каждое слово в нем начиналось с заглавной буквы, а остальные символы слова были строчными.\n"
                    "2)Удаляет все предложения, которые содержат число 2018 (даже если оно внутри какого-то слова).\n"
                    "3)Сортирует предложения по увеличению суммы цифр встречаемых в предложении. Если в предложении нет цифр, то сумма цифр данного предложения равняется бесконечности.\n"
                    "4)Выводит на экран все предложения, в которых встречаются все цифры хотя бы один раз.\n"
                    "5)Справка о функциях.\n";
    printf("%s\n", text);
}

void fun8(char* str) {
    char* start = str;
    char* finish = NULL;

    while ( (finish = strchr(start, '.')) != NULL)
    {
        int delete = 0;
        *finish = '\0';
        char* pch = start;
        while(pch < finish){
            if(isupper(*pch) != 0) {
                *finish = '.';
                delete = 1;
                break;
            }
            pch++;
        }
        if (delete == 1) {
            *finish = '.';
            memmove( start, finish+1, strlen(finish+1) + 1);
        }
        else {
            *finish = '.';
            start = finish + 1;
        }
    }

}

// hello.Delete me.No Delete, please.new sentence.
void option_8(){
    int N = 102400;
    char* str = (char*)malloc(N * sizeof(char));
    input(str);
    rmDuplicate(str);
    fun8(str);
    output(str);
    free(str);
}