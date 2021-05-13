#pragma once

#include <QObject>
#include <QWidget>
#include <QPainter>
#include <QString>
#include <QTextEdit>
#include "../RBTree.h"

struct TreeWidth;
void buildTree(const RBNode_t *Node, long long index, void *state);

class Backend : public QWidget {
    Q_OBJECT
    double factor;

    RBNode_t *rbroot;
    int ERROR_CODE;
    long long additingValue;
    TreeWidth *trW;

    QTextEdit *qte;
public:
    Backend(QWidget *parent = nullptr);
    ~Backend();

    void paintEvent(QPaintEvent *ev) override;
public slots:
    void ButtonOnClick(QString str);
    void doubleFactor();
    void slotAddValue();
};


struct TreeWidth {
    long long w;
    long long value;
    long long *tree;
    long long treeSize;
//    long long lastChangedInd;
//    RBNode_t *Node;
//    long long minX;
//    long long radius;
//    long long *tree;
};

struct StackNode {
    StackNode *left;
    StackNode *right;
    RBNode_t *value;
    long long size;
};

void pushFront(StackNode **Node, RBNode_t *value);
RBNode_t* popFront(StackNode **Node);
