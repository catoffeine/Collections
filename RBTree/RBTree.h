#ifndef RBTREE_H
#define RBTREE_H

struct RBNode_t;

typedef struct RBNode_t RBNode_t;

struct RBNode_t {
    RBNode_t *right, *left, *parent;
    long long value;
    char color; //1 - Red, 0 - black
};

RBNode_t * addValue(RBNode_t *root, long long value, int *ERROR_CODE);
RBNode_t * searchTree(RBNode_t *root, long long value, int *ERROR_CODE);
void deleteNode(RBNode_t *root, long long value, int *ERROR_CODE);
void blackDeleteBalanceTree(RBNode_t *Node, int *ERROR_CODE);

void balanceTree(RBNode_t *el, int *ERROR_CODE);
void leftSmallRotate(RBNode_t *Node, int *ERROR_CODE);
void rightSmallRotate(RBNode_t *Node, int *ERROR_CODE);
void leftBigRotate(RBNode_t *Node, int *ERROR_CODE);
void rightBigRotate(RBNode_t *Node, int *ERROR_CODE);

#endif