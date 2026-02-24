#include <stdio.h>
#include <stdlib.h>
#include "tasks.h"


int main(){
    int choice;
    printf("Course work for option 4.2, created by Gleb Alyukin.\n");

    // printf("Choose option:");
    if (scanf("%d", &choice) != 1) {
        printf("Error: Invalid input.\n");
        return 0;
    }

    getchar();
    switch(choice){
        case 0: option_0(); break;
        case 1: option_1(); break;
        case 2: option_2(); break;
        case 3: option_3(); break;
        case 4: option_4(); break;
        case 5: option_5(); break;
        case 8: option_8(); break;
        default:
            printf("Error: Option does not exists.\n");
            break;
    }

    return 0;
}