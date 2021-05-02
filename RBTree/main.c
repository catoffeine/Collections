#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "RBTree.h"


const char * checkErorCode(int ERROR_CODE) {
    switch(ERROR_CODE) {
        case 0: {
            return "OK";
        }
        case 1: {
            return "ERROR: (1) Additing element is already in the Tree";
        }
        case 2: {
            return "ERROR: (2) Bad allocation memory";
        }
        case 3: {
            return "ERROR: (3) Null pointer in function as an Argument";
        }
        case 4: {
            return "ERROR: (4) There is no left child";
        }
        case 5: {
            return "ERROR: (5)";
        }
        case 6: {
            return "ERROR: (6)";
        }
        case 7: {
            return "ERROR: (7) Red child mustn't contain single any child";
        }
        default: {
            return "Unknown error";
        }
    }
}

int blackHeightCheck(RBNode_t *node, int height) {
    if (!node) return height + 1;
    if (!node->color) height++;
    if (blackHeightCheck(node->left, height) != blackHeightCheck(node->right, height)) return 0;
    else return 1;
}

int * shiftArr(int *arr, int size, int index) {
    int i = index;
    for (; i < size-1; i++) arr[i] = arr[i+1];
    return arr;
}

int main () {
    int ERROR_CODE = 0;
    RBNode_t *root = NULL;
    addValue(&root, 10, &ERROR_CODE);
    //TEST_________________________________________________
    int i = 0, randNum = 0, randInd = 0;
    int *numArr = 0, *numToDeleteArr = 0;
    int size = 100;
    const char * errorStr = NULL;
    srand(time(NULL));
    numArr = (int*)malloc(size * sizeof(int));
    numToDeleteArr = (int*)malloc(size * sizeof(int));
    for (i = 0; i < size; i++) {
        numArr[i] = i+1;
        numToDeleteArr[i] = i+1;
    }

    for (i = 0; i < size; i++) {
        randInd = random() % (size - i);
        randNum = numArr[randInd];
        shiftArr(numArr, size, randInd);
        addValue(&root, randNum, &ERROR_CODE);
        errorStr = checkErorCode(ERROR_CODE);
        if (errorStr != "OK") {
            fprintf(stderr, "Test №%d failed in addValue: %s\n", i+1, errorStr);
            return 0;
        }
        printf("Test №%d passed (additing Value)\n", i+1);
    }

    for (i = 0; i < size; i++) {
        randInd = random() % (size - i);
        randNum = numToDeleteArr[randInd];
        shiftArr(numToDeleteArr, size, randInd);
        deleteNode(&root, randNum, &ERROR_CODE);
        errorStr = checkErorCode(ERROR_CODE);
        if (errorStr != "OK") {
            fprintf(stderr, "Test №%d failed in deleteNode: %s\n", i+1, errorStr);
            return 0;
        }
        printf("Test №%d passed (deleting Node)\n", i+1);
    }

    // for (i = 0; i < size; i++) {
    //     randInd = random() % (size - i);
    //     randNum = numArr[randInd];
    //     shiftArr(numArr, size, randInd);
    //     switch( (random() % 2 - 1) ) {
    //         case 0: {
    //             addValue(&root, randNum, &ERROR_CODE);
    //             errorStr = checkErorCode(ERROR_CODE);
    //             if (errorStr != "OK") {
    //                 fprintf(stderr, "In case 0: %s\n", errorStr);
    //                 return 0;
    //             }
    //             printf("Test №%d passed (case 0)\n", i+1);
    //             break;
    //         }
    //         case 1: {
    //             deleteNode(&root, randNum, &ERROR_CODE);
    //             errorStr = checkErorCode(ERROR_CODE);
    //             if (errorStr != "OK") {
    //                 fprintf(stderr, "In case 1: %s\n", errorStr);
    //                 return 0;
    //             }
    //             printf("Test №%d passed (case 1)\n", i+1);
    //             break;
    //         }
    //     }
    // }

    //TEST_________________________________________________
    //
    // if (ERROR_CODE) fprintf(stderr, "ERROR: code %d\n", ERROR_CODE);
    // else printf("OK\n");
    //
    // addValue(&root, 20, &ERROR_CODE);
    // if (ERROR_CODE) {
    //     printf("ERROR: %d\n", ERROR_CODE);
    //     return 0;
    // }
    // addValue(&root, 10, &ERROR_CODE);
    // if (ERROR_CODE) {
    //     printf("ERROR: %d\n", ERROR_CODE);
    // }
    // ERROR_CODE = 0;
    // addValue(&root, 30, &ERROR_CODE);
    // if (ERROR_CODE) {
    //     printf("ERROR: %d\n", ERROR_CODE);
    //     return 0;
    // }
    // deleteNode(&root, 20, &ERROR_CODE);
    // if (ERROR_CODE) {
    //     printf("ERROR: %d\n", ERROR_CODE);
    //     return 0;
    // }
    printf("all is okey\n");
    return 0;
}
