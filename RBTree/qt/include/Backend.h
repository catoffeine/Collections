#pragma once

#include <cstddef>

#include <QObject>
#include <QWidget>
#include <QPainter>
#include <QString>
#include <QTextEdit>
#include "../RBTree.h"

struct TreeWidth;
void buildTree(const RBNode_t *Node, long long index, void *state);
size_t calcHeightY(const RBNode_t *nodeToAdd);
struct StackNode;

struct RunOverTheTreeData {
    const size_t *y_lvl_cur;
};

struct StateForWidthCalc {
    RunOverTheTreeData rdata;
    StackNode *top = nullptr;
    size_t size = 0;
    size_t minWidth = 0;
//    QPainter *painter;
    size_t r = 0;
};

class Backend : public QWidget {
    Q_OBJECT
    double factor;
    double factorX;

    RBNode_t *rbroot;
    int ERROR_CODE;
    long long tmpValue;
    TreeWidth *trW;
    StateForWidthCalc stateFWCalc;
    bool treeChanged;

    QTextEdit *qte;
public:
    Backend(QWidget *parent = nullptr);
    ~Backend();

    void paintEvent(QPaintEvent *ev) override;
public slots:
    void ButtonOnClick(QString str);
    void doubleFactor();
    void halfFactorX();
    void doubleFactorX();
    void slotAddValue();
    void slotDeleteValue();
    void startTestAddValue();
    void slotDeleteTree();
};


void SN_push(StackNode **Node, const RBNode_t *value);
const StackNode* SN_find(const StackNode *top, const RBNode_t *node);
StackNode* SN_find_rw(StackNode *top, const RBNode_t *node);

void freeStackNodes(StateForWidthCalc *state);

struct TreeWidth {
    StackNode *stack;
    long long w;
    long long value;
    long long *tree;
    long long size;
};


struct StackNode {
    const RBNode_t *nodePtr;
    StackNode *previous = NULL;
    size_t width = 0;
    size_t x = 0, y = 0;
};

struct ListNode {
    ListNode *right = nullptr;
    ListNode *left = nullptr;
    const RBNode_t *value = nullptr;
    size_t index = 0;
};

void printList(const ListNode *Node);
void pushFront(ListNode **Node, RBNode_t *value);
const RBNode_t *popBack(ListNode **Node);
