#include <iostream>

#include <QDebug>
#include <QPushButton>
#include <QBoxLayout>
#include <QTextEdit>

#include "Backend.h"
#include "../RBTree.h"

static inline size_t unwrap_or(const void *const ptr, size_t ofst, size_t default_value) {
    return (ptr ? (*(size_t*)(((char*)ptr) + ofst)) : default_value);
}

void calcWidthX(const RBNode_t *node, void* state) {
    StateForWidthCalc *st = (StateForWidthCalc*)state;
    if (!st) return;    
    SN_push(&(st->top), node);
    size_t ofst = offsetof(StackNode, width);
    st->top->width = std::max(unwrap_or(SN_find(st->top, node->left), ofst, st->minWidth), unwrap_or(SN_find(st->top, node->right), ofst, st->minWidth));

    size_t yHeight = calcHeightY(st->top->nodePtr);
    size_t yHFactor = 10;
    size_t xShift{0};
    if (st->top->nodePtr->parent) {
        if (st->top->nodePtr->parent->left == st->top->nodePtr) xShift = -(st->top->width);
        else xShift = st->top->width;
    }
    size_t r = 50;

    if (node->color) st->painter->setBrush(QBrush(Qt::red, Qt::SolidPattern)); 
    else st->painter->setBrush(QBrush(Qt::black, Qt::SolidPattern));
    st->painter->drawEllipse(xShift, yHeight*yHFactor, r, r);
    st->painter->drawText(xShift+r/2, yHeight*yHFactor+r/2, QString::number(node->value));
}

size_t calcHeightY(const RBNode_t *nodeToAdd) {
    size_t yHeight{0};
    const RBNode_t *tmp = nodeToAdd;
    if (!tmp) {
        return -1;
    }
    while (tmp->parent) {
        tmp = tmp->parent;
        yHeight++;
    }
    return yHeight;
}

//void buildTree(const RBNode_t *Node, long long index, void *state) {
//    TreeWidth *treeW = static_cast<TreeWidth*>(state);
//    if (!treeW) {
//        treeW = new TreeWidth;
//    }
//    if (!treeW->tree) treeW->tree = new long long;
//    if (treeW->treeSize <= index) treeW->tree = (long long*)realloc(treeW->tree, ++(treeW->treeSize)*sizeof(TreeWidth));
//    (treeW->tree)[index] = Node->value;
//    qDebug() << QStringLiteral("treeW->tree[index]: ") << (treeW->tree)[index] << "\n"; 
//}

void runOverTheTree(RBNode_t *root, void(*func)(const RBNode_t *Node, void* state), void* state) {
    if (!root) {
        return;
    }
    runOverTheTree(root->left, func, state);
    runOverTheTree(root->right, func, state);
    func(root, state);
}

const StackNode* SN_find(const StackNode *top, const RBNode_t *node) {
    const StackNode *p = top; 
    while (p && p->nodePtr != node) p = p->previous; 
    return p;
}

void SN_push(StackNode **Node, const RBNode_t *value) {
    if (!Node) {
        throw std::runtime_error(std::string(__FUNCTION__) + ": Invalid StackNode");
    }
    if (!*Node) {
        *Node = new StackNode;
        (*Node)->nodePtr = value;
        (*Node)->previous = nullptr;
        (*Node)->width = 20; //минимальная ширина
    } else {
        StackNode *newNode = new StackNode;
        newNode->nodePtr = value;
        newNode->previous = *Node;
        *Node = newNode;
    }
}

void SN_pop(StackNode **Node) {
    if (!Node) throw std::runtime_error(std::string(__FUNCTION__) + ": Invalid StackNode"); 
    if (!*Node) return;
    StackNode *nodeToDelete = *Node;
    *Node = (*Node)->previous;
    delete nodeToDelete;
}

//void pushFront(StackNode **Node, RBNode_t *value) {
//    if (!Node) {
//        throw std::runtime_error("Node is invalid\n");
//    }
//    if (!*Node) {
//        *Node = new StackNode;
//        (*Node)->right = nullptr;
//        (*Node)->left = nullptr;
//        (*Node)->value = value;
//        (*Node)->size = 0;
//    } else {
//        StackNode *newNode = new StackNode;
//        newNode->right = *Node;
//        newNode->left = nullptr;
//        newNode->value = value;
//        newNode->size = (*Node)->size + 1;
//        (*Node)->left = newNode; 
//        *Node = newNode;
//    }
//}
//
//RBNode_t* popFront(StackNode **Node) {
//    if (!Node || !*Node) return nullptr;
//    StackNode *nodeToDelete = *Node;
//    RBNode_t *delValue = nodeToDelete->value;
//    *Node = (*Node)->right;
//    if (*Node) (*Node)->left = nullptr;
//    delete nodeToDelete;
//    return delValue;
//}

//void runWidthTheTree(RBNode_t *root, void(*func)(const RBNode_t *Node, long long index, void *state), void *state) {
//    long long index = 0;
//    StackNode *Node{nullptr};
//    try {
//        pushFront(&Node, root);
//        while (Node->size) {
//            RBNode_t *rbNode = popFront(&Node);
//            func(rbNode, index++, state);
//            if (rbNode->left) pushFront(&Node, rbNode->left);
//            if (rbNode->right) pushFront(&Node, rbNode->right);
//        }
//        popFront(&Node);
//    } catch(const std::exception &ex) {
//       qDebug() << ex.what(); 
//    }
////}

Backend::Backend(QWidget *parent): QWidget(parent), factor{1}, rbroot{nullptr} {
//    addValue(&rbroot, );
               
    trW = new TreeWidth;
    stateFWCalc = new StateForWidthCalc;
    stateFWCalc->minWidth = 20; //минимальная ширина
    stateFWCalc->painter = new QPainter(this);
    stateFWCalc->painter->setPen(QPen(Qt::white, 2, Qt::SolidLine, Qt::FlatCap));

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

    QPainter *QPainterPtr = stateFWCalc->painter;

    if (!rbroot) {
        QPainterPtr->eraseRect(0, 0, width(), height());
//        double r = 50*factor;
//        double x = (width() - r)/2;
//        double y = (height() - r)/2;
//
//        QPainterPtr.setPen(QPen(Qt::white, 10, Qt::SolidLine, Qt::FlatCap));
//        QPainterPtr.setBrush(QBrush(Qt::green, Qt::SolidPattern));
//        QPainterPtr.drawEllipse(x, y, r, r);
//        QPainterPtr.drawText(x, y, "Text 1");
//        QPainterPtr.drawEllipse(x + r, y + r, r, r);
//        QPainterPtr.drawText(x + r, y + r, "Text 2");
    } else {
        QPainterPtr->eraseRect(0, 0, width(), height());

//        double r = 50*factor;
//        double x = (width() - r)/2;
//        double y = (height() - r)/2;

        runOverTheTree(rbroot, calcWidthX, stateFWCalc);
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
    //runWidthTheTree(rbroot, buildTree, trW);
    //runOverTheTree(rbroot, something, trW);
    repaint();
}



