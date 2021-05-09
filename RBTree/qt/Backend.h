#pragma once

#include <QObject>
#include <QWidget>
#include <QPainter>
#include <QString>
#include <QTextEdit>
#include "../RBTree.h"

class Backend: public QWidget {
    Q_OBJECT
    double factor;

    RBNode_t *rbroot;
    int ERROR_CODE;

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
//    RBNode_t *Node;
//    long long minX;
//    long long radius;
//    long long *tree;
};

struct Stack {
    Stack *left;
    Stack *right;
    RBNode_t *value;
    long long size;
};

void pushFront(Stack **Node, RBNode_t *value);
RBNode_t* popFront(Stack **Node);
