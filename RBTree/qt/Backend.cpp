#include <iostream>

#include <QDebug>
#include <QPushButton>
#include <QBoxLayout>
#include <QTextEdit>

#include "Backend.h"
#include "../RBTree.h"

void calcWidthX(const RBNode_t *Node, void* state) {
    TreeWidth *st = (TreeWidth*)state;
    if (!st) return;
    
}

void buildTree(RBNode_t *Node, long long index, void *state) {
    TreeWidth *treeW = static_cast<TreeWidth*>(state);
    if (!treeW) {
        treeW = new TreeWidth;
    }
    if (treeW->tree) treeW->tree = new long long;
    if (treeW->treeSize <= index) treeW->tree = (long long*)realloc(treeW->tree, ++(treeW->treeSize)*sizeof(TreeWidth));
    (treeW->tree)[index] = Node->value;
    qDebug() << QStringLiteral("treeW->tree[index]: ") << (treeW->tree)[index] << "\n"; 
}

void runOverTheTree(RBNode_t *root, void(*func)(const RBNode_t *Node, void* state), void* state) {
    if (!root) {
        return;
    }
    runOverTheTree(root->left, func, state);
    runOverTheTree(root->right, func, state);
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

void runWidthTheTree(RBNode_t *root, void(*func)(const RBNode_t *Node, long long index, void *state), void *state) {
    long long index = 0;
    Stack *Node;
    pushFront(&Node, root);
    while (Node->size) {
        RBNode_t *rbNode = popFront(&Node);
        func(rbNode, index++, state);
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


Backend::Backend(QWidget *parent): QWidget(parent), factor{1}, rbroot{nullptr} {
//    addValue(&rbroot, );
               
    trW = new TreeWidth;

    QWidget *wgt = new QWidget(this);
    wgt->resize(200, 200);

    QGridLayout *gd = new QGridLayout(wgt);

//    QVBoxLayout *vlayout = new QVBoxLayout(wgt);

    QHBoxLayout *hlayout = new QHBoxLayout();

    QPushButton *button1 = new QPushButton("double it");
    QObject::connect(button1, SIGNAL(clicked()), this, SLOT(doubleFactor()));
    QPushButton *button2 = new QPushButton("half it");
    QObject::connect(button2, &QPushButton::clicked, this, [&]() {factor /= 2; repaint();});

    hlayout->addWidget(button1);
    hlayout->addWidget(button2);

    gd->addLayout(hlayout, 0, 0, 1, 2);

//    vlayout->addWidget(hlayout);

    qte = new QTextEdit();
    QObject::connect(qte, &QTextEdit::textChanged, this, [this](){qDebug() << qte->toPlainText();});
    gd->addWidget(qte, 1, 0, 1, 2);

    QPushButton *buttonAddValue = new QPushButton("add value");
    QObject::connect(buttonAddValue, SIGNAL(clicked()), this, SLOT(slotAddValue()));

    gd->addWidget(buttonAddValue, 2,0,1,2);
}

Backend::~Backend() {
    free(rbroot);
}


void Backend::paintEvent(QPaintEvent *ev) {
    Q_UNUSED(ev);

    QPainter painter(this);

    if (!rbroot) {
        double r = 50*factor;
        double x = (width() - r)/2;
        double y = (height() - r)/2;

        painter.setPen(QPen(Qt::white, 10, Qt::SolidLine, Qt::FlatCap));
        painter.setBrush(QBrush(Qt::green, Qt::SolidPattern));
        painter.drawEllipse(x, y, r, r);
        painter.drawText(x, y, "Text 1");
        painter.drawEllipse(x + r, y + r, r, r);
        painter.drawText(x + r, y + r, "Text 2");
    } else {
        painter.eraseRect(0, 0, width(), height());

        double r = 50*factor;
        double x = (width() - r)/2;
        double y = (height() - r)/2;

        painter.setPen(QPen(Qt::white, 2, Qt::SolidLine, Qt::FlatCap));
        if (nodeColor(rbroot)) painter.setBrush(QBrush(Qt::red, Qt::SolidPattern)); 
        else painter.setBrush(QBrush(Qt::black, Qt::SolidPattern));
        painter.drawEllipse(x, y, r, r);
        painter.drawText(x+r/2, y+r/2, QString::number(rbroot->value));
    }
}

void Backend::doubleFactor() {
    qDebug() << __FUNCTION__;
    factor *= 2;
    repaint();
}

void Backend::ButtonOnClick(QString str) {
    qDebug() << QStringLiteral("Button has been clicked and has received the following: ") << str << "\n";
}

void Backend::slotAddValue() {
    additingValue = qte->toPlainText().trimmed().toLongLong();
    addValue(&rbroot, additingValue, &ERROR_CODE);
    runWidthTheTree(rbroot, buildTree, trW);
    //runOverTheTree(rbroot, something, trW);
    repaint();
}



