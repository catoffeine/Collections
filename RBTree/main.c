#include <stdio.h>
#include <stdlib.h>
#include "RBTree.h"

int main () {
    int ERROR_CODE = 0;

    RBNode_t *root = addValue(NULL, 10, &ERROR_CODE);

    addValue(root, 20, &ERROR_CODE);
    addValue(root, 10, &ERROR_CODE);
    addValue(root, 30, &ERROR_CODE);

    if (ERROR_CODE) fprintf(stderr, "ERROR: code %d\n", ERROR_CODE);
    else printf("OK\n");

    return 0;
}
