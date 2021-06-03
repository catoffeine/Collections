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

    printf("all is okey\n");
    return 0;
}
