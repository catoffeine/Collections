#include <iostream>

#include <QDebug>
#include <QPushButton>
#include <QBoxLayout>

#include "Backend.h"
#include "../RBTree.h"

long long calcWidthX(const RBNode_t *Node) {
//    long long w1, w2;
//    if (!Node) {
//        return 2*minX+2*r;
//    }
// 
//    w1 = calcWidthX(Node->left, minX, r);
//    w2 = calcWidthX(Node->right, minX, r);
//    
//    return std::max(calcWidthX(Node->left, minX, r), calcWidthX(Node->right, minX, r));
}

void runOverTheTree(RBNode_t *root, void(*func)(const RBNode_t *Node, void* state), void* state) {
    if (!root) {
        return;
    }
    runOverTheTree(root->left, func);
    runOverTheTree(root->right, func);
    func(root, state);
}

void pushFront(Stack **Node, RBNode_t *value) {
    if (!Node) {
        Node = new Stack*;
        (*Node)->right = nullptr;
        (*Node)->left = nullptr;
        (*Node)->value = value;
        (*Node)->size++;
    } else {
        Stack *newNode = new Stack;
        newNode->right = *Node;
        newNode->left = nullptr;
        newNode->value = value;
        newNode->size = (*Node)->size++;
        (*Node)->left = newNode; *Node = newNode;
    }
} 

void runWidthTheTree(RBNode_t *root, long long(*func)(const RBNode_t *Node)) {
    Stack *Node;
    pushFront(&Node, root);
    while (Node->size) {
        RBNode_t *rbNode = popFront(&Node);
        func(rbNode);
        if (rbNode->left) pushFront(&Node, rbNode->left);
        if (rbNode->right) pushFront(&Node, rbNode->right);
    }
}


RBNode_t* popFront(Stack **Node) {
    if (!Node) return nullptr;
    Stack *deletingNode = *Node;
    RBNode_t *delValue = deletingNode->value;
    if ((*Node)->right) (*Node)->right->size = (*Node)->size--;
    *Node = (*Node)->right;
    (*Node)->left = nullptr;
    delete deletingNode;
    return delValue;
}

Backend::Backend(QWidget *parent): QWidget(parent), factor{1} {
    QHBoxLayout *layout = new QHBoxLayout(this);

    QPushButton *button1 = new QPushButton("double it");
    QObject::connect(button1, SIGNAL(clicked()), this, SLOT(doubleFactor()));
    QPushButton *button2 = new QPushButton("half it");
    QObject::connect(button2, &QPushButton::clicked, this, [&]() {factor /= 2; repaint();});

    layout->addWidget(button1);
    layout->addWidget(button2);
}

void Backend::paintEvent(QPaintEvent *ev) {
    Q_UNUSED(ev);

    QPainter painter(this);
    
    double r = 50*factor;
    double x = (width() - r)/2;
    double y = (height() - r)/2;

    painter.setPen(QPen(Qt::white, 10, Qt::SolidLine, Qt::FlatCap));
    painter.setBrush(QBrush(Qt::green, Qt::SolidPattern));
    painter.drawEllipse(x, y, r, r);
    painter.drawText(x, y, "Text 1");
    painter.drawEllipse(x + r, y + r, r, r);
    painter.drawText(x + r, y + r, "Text 2");
}

void Backend::doubleFactor() {
    qDebug() << __FUNCTION__;
    factor *= 2;
    repaint();
}

void Backend::ButtonOnClick(QString str) {
    qDebug() << QStringLiteral("Button has been clicked and has received the following: ") << str << "\n";
}
