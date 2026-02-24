#include <stdio.h>
#include "additional.h"
#include <ctype.h>

void input(char* str) {
    int c;
    int i = 0;
    int prev_c = 0; // Предыдущий символ

    while ((c = getchar()) != EOF) {
        if (prev_c == '\n' && c == '\n') { break; }

        if (c == '\n') {
            prev_c = c;
            continue; 
        }
        
        // Пропуск пробела после точки
        if (i > 0 && str[i - 1] == '.' && c == ' ') { 
            prev_c = c; 
            continue; 
        }
        
        // Пропуск пробела после \n
        if (prev_c == '\n' && c == ' ') {
            prev_c = c;
            continue;
        }

        str[i] = (char)c;
        i++;
        prev_c = c;
        // 102400 - размер, выделенный в tasks.c
        if (i >= 102400 - 1) break;
    }
    str[i] = '\0';
}


void printL(char* start, char* tochka) {
    for (char* p = start; p <= tochka; p++) {
        printf("%c", *p);
    }
    printf("\n");
}


void output(char* str) {
    int i = 0;
    while (str[i] != '\0')
    {
        if (str[i] == '\n') {
        }
        else if (str[i] == '.') {
            printf("%c\n", str[i]);
        }
        else printf("%c", str[i]);
        i++;

    }
}