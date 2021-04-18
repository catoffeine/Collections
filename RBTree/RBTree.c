#include <stdio.h>
#include <stdlib.h>
#include "RBTree.h"

RBNode_t * searchTree(RBNode_t *root, long long value, int *ERROR_CODE) {
    RBNode_t *p = root;
    if (!root) {
        return NULL;
    }
    do {
        if (p->value < value) {
            if (!p->right) return NULL;
            p = p->right;
        } else if (value < p->value) {
            if (!p->left) return NULL;
            p = p->left;
        } else {
            return p;
        }
    } while (1);
}


RBNode_t * addValue(RBNode_t *root, long long value, int *ERROR_CODE) {
    printf("Input in addValue\n");
    RBNode_t *p, *next = root;
    char isLeafLeft = 0;
    if (root) {
        do {
            p = next;
            if (value > next->value) {
                if (!next->right) {
                    isLeafLeft = 0;
                    break;
                }
                next = next->right;
            } else if (value < next->value) {
                if (!next->left) {
                    isLeafLeft = 1;
                    break;
                }
                next = next->left;
            } else {
                *ERROR_CODE = 1; //Добавляемый элемент уже существует в дереве
                return NULL;
            }
        } while(1);
    } else {
        RBNode_t *newNode = (RBNode_t*)malloc(sizeof(RBNode_t));
        if (!newNode) {
            *ERROR_CODE = 2;
            return NULL;
        }
        newNode->color = 0;
        newNode->value = value;
        newNode->right = NULL;
        newNode->left = NULL;
        newNode->parent = NULL;
        printf("End of addValue\n");
        return newNode;
    }
    RBNode_t *newNode = (RBNode_t*)malloc(sizeof(RBNode_t));
    if (!newNode) {
        *ERROR_CODE = 2;
        return NULL;
    }

    newNode->parent = p;
    newNode->color = 1;
    newNode->right = NULL;
    newNode->left = NULL;
    newNode->value = value;

    if (isLeafLeft) p->left = newNode;
    else p->right = newNode;

    balanceTree(newNode, ERROR_CODE);
    if (*ERROR_CODE) return NULL;
    printf("END of addValue\n");
    return newNode;
}

void blackDeleteBalanceTree(RBNode_t *Node, int isLeft, int *ERROR_CODE) {
    RBNode_t *p = Node, *tmp = 0;
    if (!Node) {
        *ERROR_CODE = 5;
        return;
    }


    // while (Node->parent && !Node->color) {
    //     if (isLeft) {
    //         if (Node->right->color) { //Если правый потомок - красный
    //             Node->right->color = 0;
    //             Node->color = 1;
    //             leftBigRotate(Node, ERROR_CODE);
    //             Node = Node->parent;
    //         }
    //
    //         if (!Node->right->right->color && !Node->right->left->color) { //Если потомки - черные
    //             Node->right->color = 1;
    //         } else {
    //             if (!Node->right->right || !Node->right->right->color) {
    //                 if (Node->right->left) {
    //                     Node->right->left->color = 0;
    //                 }
    //                 Node->right->color = 1;
    //                 rightBigRotate(Node->right, ERROR_CODE);
    //                 Node = Node->parent->right;
    //             }
    //             Node->color = Node->parent->color;
    //             Node->parent->color = 0;
    //             if (Node->right) Node->right->color = 0;
    //             leftBigRotate(Node->parent, ERROR_CODE);
    //             return;
    //         }
    //
    //     } else {
    //         if (Node->left->color) { //Если левый потомок - красный
    //             Node->left->color = 0;
    //             Node->color = 1;
    //             rightBigRotate(Node, ERROR_CODE);
    //             Node = Node->parent;
    //         }
    //
    //         if (!Node->left->right->color && !Node->left->left->color) { //Если потомки - черные
    //             Node->left->color = 1;
    //         } else {
    //             if (!Node->left->left->color) {
    //                 if (Node->right->left) {
    //                     Node->right->left->color = 0;
    //                 }
    //                 Node->left->color = 1;
    //                 leftBigRotate(Node->left, ERROR_CODE);
    //                 Node = Node->parent->left;
    //             }
    //             Node->color = Node->parent->color;
    //             Node->parent->color = 0;
    //             if (Node->left) Node->left->color = 0;
    //             leftBigRotate(Node->parent, ERROR_CODE);
    //             return;
    //         }
    //     }
    // }


}

void deleteNode(RBNode_t *el, int *ERROR_CODE) {
    RBNode_t *p = el, *tmp = 0;
    int isLeft = 0;
    if (!el) {
        *ERROR_CODE = 5;
        return;
    }

    if (!el->right && !el->left) { //Если отсутствуют потомки
        tmp = el;
        if (el->parent) { //Если есть родитель, делаем указатель на этот элемент - NULL;
            if (el->parent->left == el) {
                el->parent->left = NULL;
                isLeft = 1;
            } else {
                el->parent->right = NULL;
                isLeft = 0;
            }
        }

        free(el);
        el = NULL;

        if (!tmp->color) { //Если черный
            blackDeleteBalanceTree(el->parent, isLeft, ERROR_CODE);
        }

        return;
    }

    if ((!el->right && el->left) || (el->right && !el->left)) { //Если у вершины только один ребенок
        if (!el->color) { //Если элемент черный
            if (el->right) {
                el->value = el->right->value;
                free(el->right);
                el->right = NULL;
            } else {
                el->value = el->left->value;
                free(el->left);
                el->left = NULL;
            }
        } else {
            *ERROR_CODE = 6; //У красного не может быть никакого одного ребенка
        }
        return;
    }

    if (el->right && el->left) { //Если оба ребенка существуют
        p = p->right;
        while (p->left) { //Ищем минимальный справа
            p = p->left;
        }
        tmp = el; //Меняем местами с наименьшим справа
        el->value = p->value;
        p->value = tmp->value;
        deleteNode(p, ERROR_CODE); //Вызываем удаление вершины для данного элемента
    }

}


void balanceTree(RBNode_t *el, int *ERROR_CODE) { //Предпологаем, что el->color==1 - красный
    printf("INPUT in balanceTree\n");
    RBNode_t *p = el;
    char isLeftEl = 0;

    if (!el) {
        *ERROR_CODE = 3;
        return;
    }

    if (!el->parent)  {
        el->color = 0;
        return;
    }

    isLeftEl = el->parent->left == el ? 1 : 0;

    if (el->parent->color) { //Если красный
        if (isLeftEl) { //Если проверяемый элемент находится слева от родителя
            if (el->parent->parent->right == el->parent) { //Если дедушка проверяемого элемента находится по другую сторону
                rightSmallRotate(el->parent, ERROR_CODE); //Вызываем правое малое вращение от родителя
                if (*ERROR_CODE) return;
                balanceTree(el->left, ERROR_CODE);
                if (*ERROR_CODE) return;

            } else { //Если дедушка проверяемого элемента находится на одной стороне
                if (el->parent->parent->left->color && !el->parent->parent->left) { //Если дядя проверяемого элемента красный
                    el->parent->parent->left->color = 0; //Меняем цвета на черный для дяди и для родителя
                    el->parent->color = 0;
                    balanceTree(el->parent->parent, ERROR_CODE);
                    if (*ERROR_CODE) return;
                } else { //Дядя черный (или NULL)
                    rightBigRotate(el->parent->parent, ERROR_CODE); //Вызываем большое вращение от дедушки проверяемого элемента
                    if (*ERROR_CODE) return;
                }
            }
        } else { //Если проверяемый элемент находится справа от родителя
            if (el->parent->parent->left == el->parent) { //Если дедушка проверяемого элемента находится по другую сторону
                leftSmallRotate(el->parent, ERROR_CODE); //Вызываем левое малое вращение для родителя
                if (*ERROR_CODE) return;
                balanceTree(el->left, ERROR_CODE);
                if (*ERROR_CODE) return;
            } else {
                if (el->parent->parent->right->color && !el->parent->parent->right) { //Если дядя проверяемого элемента тоже красный
                    el->parent->parent->right->color = 0;
                    el->parent->color = 0;
                    balanceTree(el->parent->parent, ERROR_CODE);
                    if (*ERROR_CODE) return;
                } else { //Дядя черный
                    leftBigRotate(el->parent->parent, ERROR_CODE); //Вызываем большое вращение от дедушки проверяемого элемента
                    if (*ERROR_CODE) return;
                }
            }
        }
    } else {
        printf("END balanceTree\n");
        return;
    }

}

void leftSmallRotate(RBNode_t *Node, int *ERROR_CODE) {
    printf("INPUT in leftSmallRotate\n");
    if (!Node) {
        *ERROR_CODE = 4;
        return;
    }

    Node->parent->left = Node->right; //Выставляем для отца левый элемент - правый сын данной ноды
    Node->right = Node->right->left; //Выставляем указатель для ноды на правого потомка в качетсве левого для правого бывшего потомка данной ноды (хз что тут написано)
    Node->parent->left->left = Node; //Выставляем уже для потомка отца указатель на левого сына (проверяемый элемент)
    Node->parent->left->parent = Node->parent; //Указываем новых родителей
    Node->parent = Node->parent->left; //Указываем новых родителей
    printf("END of leftSmallRotate\n");
}

void rightSmallRotate(RBNode_t *Node, int *ERROR_CODE) {
    printf("INPUT in rightSmallRotate\n");
    if (!Node) {
        *ERROR_CODE = 4;
        return;
    }

    Node->parent->right = Node->left; //Выставляем для отца правый элемент - левый сын данной ноды
    Node->left = Node->left->right;
    Node->parent->right->right = Node; //Выставляем уже для потомка отца указатель на правую ноду - проверяемый элемент
    Node->parent->right->parent = Node->parent; //Указываем новых родителей
    Node->parent = Node->parent->right;  //Указываем новых родителей
    printf("End of rightSmallRotate\n");
}

void leftBigRotate(RBNode_t *Node, int *ERROR_CODE) {
    printf("INPUT in leftBigRotate\n");
    RBNode_t *p = Node, *parent = 0, *tmp = 0;

    if (!Node) {
        *ERROR_CODE = 4;
        return;
    }

    if (!p->right) {
        *ERROR_CODE = 5; //Нет правого потомка
        return;
    }

    if (!p->parent) {
        p->right->parent = NULL; //Так как родитель - корень, выставляем в качестве родителя - нулевой указатель
    } else {
        parent = p->parent;
        p->right->parent = parent; //в качестве родителя указываем родителя данного элемента
        if (parent->right == p) { //Перемещаем указатель на с данной ноды на правого потомка
            parent->right = p->right;
        } else {
            parent->left = p->right;
        }
    }

    tmp = p->right->left; //Запоминаем указатель на левого потомка правой ноды

    p->parent = p->right; //Указываем в качестве родителя правую ноду
    p->right->left = p; //Для правого потомка в качестве левого указываем данную ноду
    p->right = tmp; //Для данной ноды в качестве правого потомка указываем ранее запомненного левого потомка правой ноды
    p = p->parent; //Перемещаем указатель на родителя элемента

    p->color = 0; //Черный (Меняем цвета перемещаемых элементов)
    p->left->color = 1; //Красный
    balanceTree(p, ERROR_CODE); //Запускаем балансировку для последнего элемента
    printf("END of leftBigRotate\n");
}

void rightBigRotate(RBNode_t *Node, int *ERROR_CODE) {
    printf("INPUT in rightBigRotate\n");
    RBNode_t *p = Node, *parent = 0, *tmp = 0;

    if (!Node) {
        *ERROR_CODE = 4;
        return;
    }

    if (!p->left) {
        *ERROR_CODE = 5; //Нет левого потомка

        return;
    }

    if (!p->parent) {
        p->left->parent = NULL; //Так как родитель - корень, выставляем в качестве родителя - нулевой указатель
    } else {
        parent = p->parent;
        p->left->parent = parent; //в качестве родителя указываем родителя данного элемента
        if (parent->right == p) {//Перемещаем указатель на с данной ноды на левого потомка
            parent->right = p->left;
        } else {
            parent->left = p->left;
        }
    }

    tmp = p->left->right; //Запоминаем указатель на правого потомка левой ноды

    p->parent = p->left; //Указываем в качестве родителя левую ноду
    p->left->right = p; //Для левого потомка в качестве правого указываем данную ноду
    p->left = tmp; //Для данной ноды в качестве левого потомка указываем ранее запомненного правого потомка правой ноды
    p = p->parent; //Перемещаем указатель на родителя элемента

    p->color = 0; //Черный (Меняем цвета перемещаемых элементов)рлдр рлрддрд
    p->right->color = 1; //Красный
    balanceTree(p, ERROR_CODE); //Запускаем балансировку для последнего элемента
    printf("END of rightBigRotate\n");
}
