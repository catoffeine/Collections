#pragma once

#include <QObject>
#include <QWidget>
#include <QPainter>
#include <QString>
#include "../RBTree.h"

class Backend: public QWidget {
    Q_OBJECT
    double factor;
public:
    Backend(QWidget *parent = nullptr);
    ~Backend() = default;

    void paintEvent(QPaintEvent *ev) override;

public slots:
    void ButtonOnClick(QString str);
    void doubleFactor();
};

struct TreeWidth {
    RBNode_t *Node;
    long long minX;
    long long radius;
    long long *tree;
};

struct Stack {
    Stack *left;
    Stack *right;
    RBNode_t *value;
    long long size;
};

void pushFront(Stack **Node, RBNode_t *value); 
RBNode_t* popFront(Stack **Node);

