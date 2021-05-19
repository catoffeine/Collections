#include <stdio.h>
#include <stdlib.h>
#include "RBTree.h"

#define RED 1
#define BLACK 0

// #define DBG

#ifdef DBG

#define PRINT_IF_DBG(cond, msg) {if (cond) fprintf(stderr, "%s: % 3d: %s\n", __FILE__, __LINE__, msg);}
#define PRINT_EC { fprintf(stderr, "%s: % 3d: *ERROR_CODE: %d\n", __FILE__, __LINE__, *ERROR_CODE); }

#else

#define PRINT_IF_DBG(cond, msg) {}
#define PRINT_EC

#endif

int nodeColor(const RBNode_t *Node) {
    if (!Node) return BLACK;
    if (Node->color) return RED;
    else return BLACK;
}


void deleteTree(RBNode_t *Node) {
    if (!Node) return;
    deleteTree(Node->left);
    deleteTree(Node->right);
    free(Node);
}

//Функция проверки условий соблюдения свойств красно-черного дерева, написать тесты и добавить DBG везде

RBNode_t * searchTree(RBNode_t **root, RBNode_value_t value, int *ERROR_CODE) {
    RBNode_t *p;
    if (!root || !*root) {
        return NULL;
    }
    p = *root;
    do {
        if (p->value < value) {
            if (!p->right) {
                *ERROR_CODE = 6;
                PRINT_EC
                return NULL;
            }
            p = p->right;
        } else if (value < p->value) {
            if (!p->left) {
                *ERROR_CODE = 6;
                PRINT_EC
                return NULL;
            }
            p = p->left;
        } else {
            return p;
        }
    } while (1);
}

RBNode_t * findRoot(RBNode_t *current) {
    while (current->parent) current = current->parent;
    return current;
}

RBNode_t * addValue(RBNode_t **root, RBNode_value_t value, int *ERROR_CODE) {
    PRINT_IF_DBG(1, "BEGIN function addValue");
    RBNode_t *p, *next;
    char isLeafLeft = 0;
    if (!root) {
        *ERROR_CODE = 3;
        return NULL;
    }
    next = *root;
    if (*root) {
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
        newNode->color = BLACK;
        newNode->value = value;
        newNode->right = NULL;
        newNode->left = NULL;
        newNode->parent = NULL;
        *root = newNode;
        PRINT_IF_DBG(1, "END of function addValue");
        return newNode;
    }
    RBNode_t *newNode = (RBNode_t*)malloc(sizeof(RBNode_t));
    if (!newNode) {
        *ERROR_CODE = 2;
        return NULL;
    }

    newNode->parent = p;
    newNode->color = RED;
    newNode->right = NULL;
    newNode->left = NULL;
    newNode->value = value;

    if (isLeafLeft) p->left = newNode;
    else p->right = newNode;

    balanceTree(newNode, ERROR_CODE);
    if (*ERROR_CODE) return NULL;
    *root = findRoot(newNode);
    PRINT_IF_DBG(1, "END of function addValue");
    return newNode;
}

void blackDeleteBalanceTree(RBNode_t *Node, int *ERROR_CODE) {
    PRINT_IF_DBG(1, "BEGIN function blackDeleteBalanceTree");
    RBNode_t *pNode = Node, *tmp = 0;
    RBNode_t *root;
    if (!Node) {
        *ERROR_CODE = 3;
        return;
    }
    root = findRoot(Node);

    while (Node->parent && !Node->color) {
        pNode = Node->parent;
        if (Node == Node->parent->left) { //Если элемент находится слева
            if (nodeColor(pNode->right)) { //Если правый потомок - красный
                // pNode->right->color = BLACK; //Меняем цвета дяди и родителя на черный и красный соответственно
                // pNode->color = RED;
                leftBigRotate(pNode, ERROR_CODE);
                pNode = Node->parent; //Переходим на уровень выше из-за вращения
            }

            if (!nodeColor(pNode->right->right) && !nodeColor(pNode->right->left)) { //Если потомки правого сына - черные
                pNode->right->color = RED;
                Node = Node->parent;
            } else {
                if (!nodeColor(pNode->right->right)) { //Если левый красный и правый черный - потомки правого сына
                    if (pNode->right->left) pNode->right->left->color = BLACK;
                    pNode->right->color = RED;
                    rightBigRotate(pNode->right, ERROR_CODE);
                    if (*ERROR_CODE) return;
                    pNode = Node->parent; //на уровень выше из-за врашения
                }
                pNode->right->color = pNode->parent->color;
                Node->parent->color = BLACK;
                if (pNode->right->right) pNode->right->right->color = BLACK;
                leftBigRotate(Node->parent, ERROR_CODE);
                root->color = BLACK;
                PRINT_IF_DBG(1, "END function blackDeleteBalanceTree");
                return;
            }

        } else {
            if (nodeColor(pNode->left)) { //Если левый потомок - красный
                // pNode->left->color = BLACK;
                // pNode->color = RED;
                rightBigRotate(pNode, ERROR_CODE);
                pNode = Node->parent;
            }

            if (!nodeColor(pNode->left->right) && !nodeColor(pNode->left->left)) { //Если потомки - черные
                pNode->left->color = BLACK; //RED maybe
                Node = Node->parent;
            } else {
                if (!nodeColor(pNode->left->left)) {
                    if (pNode->left->right) pNode->left->right->color = BLACK;
                    Node->left->color = RED;
                    leftBigRotate(Node->left, ERROR_CODE);
                    pNode = Node->parent; //на уровень выше из-за врашения
                }
                pNode->left->color = pNode->parent->color;
                Node->parent->color = BLACK;
                if (pNode->left) pNode->left->color = BLACK;
                leftBigRotate(Node->parent, ERROR_CODE);
                root->color = BLACK;
                PRINT_IF_DBG(1, "END function blackDeleteBalanceTree");
                return;
            }
        }
    }
    PRINT_IF_DBG(1, "END function blackDeleteBalanceTree");


}

void deleteNode(RBNode_t **root, RBNode_value_t value, int *ERROR_CODE) {
    PRINT_IF_DBG(1, "BEGIN function deleteNode");
    RBNode_t *p = 0, *tmp = 0;
    RBNode_t *el;
    RBNode_value_t tmpN = 0;
    if (!root || !*root) {
        *ERROR_CODE = 3;
        return;
    }
    el = searchTree(root, value, ERROR_CODE);
    p = el;
    if (*ERROR_CODE) {
        return;
    }

    if (!el->right && !el->left) { //Если отсутствуют потомки
        if (el->parent) { //Если есть родитель, делаем указатель на этот элемент - NULL;
            if (el->parent->left == el) {
                el->parent->left = NULL;
            } else {
                el->parent->right = NULL;
            }
        }
        if (!nodeColor(el)) { //Если черный
            blackDeleteBalanceTree(el, ERROR_CODE);
        }
        *root = findRoot(el);
        free(el);
        el = NULL;
        PRINT_IF_DBG(1, "END function deleteNode");
        return;
    }

    if ((!el->right && el->left) || (el->right && !el->left)) { //Если у вершины только один ребенок
        if (!el->color) { //Если элемент черный
            if (el->right) { //Если ребенок справа
                PRINT_IF_DBG(nodeColor(el->right), "BLACK RIGHT SINGLE CHILD, IMPOSSIBLE")
                el->value = el->right->value; //Присваеваем значение ребенка и удаляем ребенка
                free(el->right);
                el->right = NULL;
            } else {  //Если ребенок слева
                PRINT_IF_DBG(nodeColor(el->right), "BLACK LEFT SINGLE CHILD, IMPOSSIBLE")
                el->value = el->left->value; //Присваеваем значение ребенка и удаляем ребенка
                free(el->left);
                el->left = NULL;
            }
        } else {
            *ERROR_CODE = 7; //У красного не может быть никакого одного ребенка
            PRINT_EC
        }
        *root = findRoot(el);
        PRINT_IF_DBG(1, "END function deleteNode");
        return;
    }

    if (el->right && el->left) { //Если оба ребенка существуют
        p = p->right;
        while (p->left) { //Ищем минимальный справа
            p = p->left;
        }

        tmpN = p->value;
        p->value = el->value;
        el->value = tmpN;

        if (!nodeColor(p)) { //Если удаляемый элемент - черный
            if (p->right) { //Если есть правый ребенок
                PRINT_IF_DBG(nodeColor(el->right), "BLACK RIGHT SINGLE CHILD, IMPOSSIBLE")
                p->value = p->right->value; //Присваеваем значение ребенка и удаляем ребенка
                free(p->right);
                p->right = NULL;
            } else {
                blackDeleteBalanceTree(p, ERROR_CODE); //Вызываем балансировку от p
            }
            *root = findRoot(p);
            PRINT_IF_DBG(1, "END function deleteNode");
            return;
        } else { // Если красный - удаляем и меняем указатель на NULL;
            if (!p->parent) {
                *root = NULL;
            } else {
                p->parent->left = NULL;
            }
            free(p);
            p = NULL;
        }
    }
    PRINT_IF_DBG(1, "END function deleteNode");
}


void balanceTree(RBNode_t *el, int *ERROR_CODE) { //Предпологаем, что el->color==1 - красный
    PRINT_IF_DBG(1, "BEGIN function balanceTree");
    RBNode_t *p = el;
    char isLeftEl = 0;

    if (!el) {
        *ERROR_CODE = 3;
        return;
    }

    if (!el->parent)  {
        el->color = BLACK;
        return;
    }

    isLeftEl = el->parent->left == el ? 1 : 0;

    if (el->parent->color) { //Если красный
        if (isLeftEl) { //Если проверяемый элемент находится слева от родителя
            PRINT_IF_DBG(!el->parent->parent, "BALANCE TREE: THERE IS NO GRANDPARENT");
            if (el->parent->parent->right == el->parent) { //Если дедушка проверяемого элемента находится по другую сторону
                rightSmallRotate(el->parent, ERROR_CODE); //Вызываем правое малое вращение от родителя
                if (*ERROR_CODE) return;
                balanceTree(el->right, ERROR_CODE);
                if (*ERROR_CODE) return;

            } else { //Если дедушка проверяемого элемента находится на одной стороне
                if (nodeColor(el->parent->parent->right)) { //Если дядя проверяемого элемента красный
                    el->parent->parent->right->color = BLACK; //Меняем цвета на черный для дяди и для родителя
                    el->parent->color = BLACK;
                    el->parent->parent->color = RED; //Меняем цвет деда на красный
                    balanceTree(el->parent->parent, ERROR_CODE);
                    if (*ERROR_CODE) return;
                } else { //Дядя черный (или NULL)
                    rightBigRotate(el->parent->parent, ERROR_CODE); //Вызываем большое вращение от дедушки проверяемого элемента
                }
            }
        } else { //Если проверяемый элемент находится справа от родителя
            PRINT_IF_DBG(!el->parent->parent, "BALANCE TREE: THERE IS NO GRANDPARENT");
            if (el->parent->parent->left == el->parent) { //Если дедушка проверяемого элемента находится по другую сторону
                leftSmallRotate(el->parent, ERROR_CODE); //Вызываем левое малое вращение для родителя
                if (*ERROR_CODE) return;
                balanceTree(el->left, ERROR_CODE);
                if (*ERROR_CODE) return;
            } else {
                if (nodeColor(el->parent->parent->left)) { //Если дядя проверяемого элемента тоже красный
                    el->parent->parent->left->color = BLACK;
                    el->parent->color = BLACK;
                    el->parent->parent->color = RED;
                    balanceTree(el->parent->parent, ERROR_CODE);
                    if (*ERROR_CODE) return;
                } else { //Дядя черный
                    leftBigRotate(el->parent->parent, ERROR_CODE); //Вызываем большое вращение от дедушки проверяемого элемента
                }
            }
        }
    } else {
        PRINT_IF_DBG(1, "END of function balanceTree");
        return;
    }
}

void leftSmallRotate(RBNode_t *Node, int *ERROR_CODE) {
    PRINT_IF_DBG(1, "BEGIN of function leftSmallRotate");
    if (!Node) {
        *ERROR_CODE = 3;
        return;
    }

    Node->parent->left = Node->right; //Выставляем для отца левый элемент - правый сын данной ноды
    Node->right = Node->right->left; //Выставляем указатель для ноды на правого потомка в качетсве левого для правого бывшего потомка данной ноды (хз что тут написано)
    Node->parent->left->left = Node; //Выставляем уже для потомка отца указатель на левого сына (проверяемый элемент)
    Node->parent->left->parent = Node->parent; //Указываем новых родителей
    Node->parent = Node->parent->left; //Указываем новых родителей
    PRINT_IF_DBG(1, "END of function leftSmallRotate");
}

void rightSmallRotate(RBNode_t *Node, int *ERROR_CODE) {
    PRINT_IF_DBG(1, "BEGIN of function rightSmallRotate");
    if (!Node) {
        *ERROR_CODE = 3;
        return;
    }

    Node->parent->right = Node->left; //Выставляем для отца правый элемент - левый сын данной ноды
    Node->left = Node->left->right;
    Node->parent->right->right = Node; //Выставляем уже для потомка отца указатель на правую ноду - проверяемый элемент
    Node->parent->right->parent = Node->parent; //Указываем новых родителей
    Node->parent = Node->parent->right;  //Указываем новых родителей
    PRINT_IF_DBG(1, "END of function rightSmallRotate");
}

void leftBigRotate(RBNode_t *Node, int *ERROR_CODE) {
    PRINT_IF_DBG(1, "BEGIN of function leftBigRotate");
    RBNode_t *p = Node, *parent = 0, *tmp = 0;

    if (!Node) {
        *ERROR_CODE = 3;
        return;
    }

    if (!p->right) {
        *ERROR_CODE = 5; //Нет правого потомка
        PRINT_EC
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

    p->color = BLACK; //Черный (Меняем цвета перемещаемых элементов)
    p->left->color = RED; //Красный
    PRINT_IF_DBG(1, "END of function leftBigRotate");
}

void rightBigRotate(RBNode_t *Node, int *ERROR_CODE) {
    PRINT_IF_DBG(1, "BEGIN of function rightBigRotate");
    RBNode_t *p = Node, *parent = 0, *tmp = 0;

    if (!Node) {
        *ERROR_CODE = 3;
        return;
    }

    if (!p->left) {
        *ERROR_CODE = 5; //Нет левого потомка
        PRINT_EC
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

    p->color = BLACK; //Черный (Меняем цвета перемещаемых элементов)
    p->right->color = RED; //Красный
    PRINT_IF_DBG(1, "END of function rightBigRotate");
}
