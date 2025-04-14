#include <stdio.h>

#include <stdlib.h>

int *ptrHandler;

int main() {
    ptrHandler = malloc(5 * sizeof(int));
    ptrHandler[0] = 2;
    ptrHandler[1] = 3;
    ptrHandler[2] = 4;
    ptrHandler[3] = 5;
    ptrHandler[4] = 6;

    int size = sizeof(ptrHandler);
    printf("size :%d\n", size);
    for (int i = 0; i < 5; i++) {
        printf("p[%d]: %d\n", i, *ptrHandler);
        printf("p[%d]: %p\n", i, (int *) ptrHandler);
        ptrHandler++;
    }
    return 0;
}
