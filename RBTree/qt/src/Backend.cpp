#include <iostream> 
#include <QDebug>
#include <QPushButton>
#include <QBoxLayout>
#include <QTextEdit>
#include <string>

#include "Backend.h"
#include "../RBTree.h"

#include <unistd.h>

static inline size_t unwrap_or(const void *const ptr, size_t ofst, size_t default_value) {
    return (ptr ? (*(size_t*)(((char*)ptr) + ofst)) : default_value);
}

void printStateNodes(StateForWidthCalc *stateFWCalc) {
    StackNode *p{0};
    if (!stateFWCalc) {
        return;   
    }
    p = stateFWCalc->top;
    qDebug() << "________________________________________";
    while (p) {
        qDebug() << "RBNode_t's value: " << p->nodePtr->value;
        qDebug() << "StackNode's width position: " << p->width;
        qDebug() << "StackNode's Y position: " << p->y;
        p = p->previous;
    }
    qDebug() << "________________________________________";
}

void freeStackNodes(StateForWidthCalc *state) {
    StackNode *p = NULL;
    StackNode *tmp = NULL;
    if (!state || !state->top) return;
    p = state->top;
    while (p) {
       tmp = p->previous;
       free(p);
       p = tmp;
    }
    state->top = NULL;
}

void calcWidthX(const RBNode_t *node, void* state) {
    StateForWidthCalc *st = (StateForWidthCalc*)state;
    if (!st) return;    
//    if (st->top) {
//        size_t ofst = offsetof(StackNode, width);
//        st->top->width = 2*st->r+2*std::max(unwrap_or(SN_find(st->top, node->left), ofst, st->minWidth), unwrap_or(SN_find(st->top, node->right), ofst, st->minWidth));
//    }
//    if (st->top && SN_find(st->top, node)) {
//        st->top->y = *(st->rdata.y_lvl_cur) + 1;
//        return;
//    }
    SN_push(&(st->top), node);
    size_t ofst = offsetof(StackNode, width);
    st->top->width = 2*st->r+2*std::max(unwrap_or(SN_find(st->top, node->left), ofst, st->minWidth), unwrap_or(SN_find(st->top, node->right), ofst, st->minWidth));
    
 //   qDebug() << "st->top->width: " << st->top->width;
    st->top->y = *(st->rdata.y_lvl_cur) + 1;
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
    static size_t y_lvl = 0;
    if (!root) {
        return;
    }
//    if (state) {*(size_t**)state = &y_lvl;}

    RunOverTheTreeData *st = (RunOverTheTreeData*)(state);
    if (!y_lvl && st) {st->y_lvl_cur = &y_lvl;}

    y_lvl++;
    runOverTheTree(root->left, func, state);
    runOverTheTree(root->right, func, state);
    y_lvl--;
//    qDebug() << "y_lvl is " << y_lvl;
    func(root, state);
}

const StackNode* SN_find(const StackNode *top, const RBNode_t *node) {
    const StackNode *p = top; 
    while (p && p->nodePtr != node) p = p->previous; 
    return p;
}

StackNode* SN_find_rw(StackNode *top, const RBNode_t *node) {
    StackNode *p = top; 
    qDebug() << "StackNode *top is " << top << "\nRBNode_t node is " << node << "\n----------------------------\n";
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
        (*Node)->width = 0; //минимальная ширина
    } else {
        StackNode *newNode = new StackNode;
        newNode->nodePtr = value;
        newNode->previous = *Node;
        newNode->width = 0;
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

void pushFront(ListNode **Node, RBNode_t *value) {
    if (!Node) {
        throw std::runtime_error("Node is invalid\n");
    }
    if (!*Node) {
        *Node = new ListNode;
        (*Node)->right = nullptr;
        (*Node)->left = nullptr;
        (*Node)->value = value;
        (*Node)->index = 0;
    } else {
        ListNode *newNode = new ListNode;
        newNode->right = *Node;
        newNode->left = nullptr;
        newNode->value = value;
        newNode->index = (*Node)->index + 1;
        (*Node)->left = newNode; 
        *Node = newNode;
    }
}

const RBNode_t *popBack(ListNode **Node) {
    if (!Node || !*Node) return nullptr;
    ListNode *nodeToDelete = *Node;
    while (nodeToDelete->right) nodeToDelete = nodeToDelete->right; 
    const RBNode_t *delValue = nodeToDelete->value;
    if (nodeToDelete->left) nodeToDelete->left->right = nullptr;
    else *Node = nullptr;
    delete nodeToDelete;
    return delValue;
}
void printList(const ListNode *Node) {
    const ListNode *p = Node;
    if (!Node) return;
    while (p) {
        qDebug() << "RBNode value is " << p->value->value;
        p = p->right;
    }
}

void runWidthTheTree(RBNode_t *root, void(*func)(const RBNode_t *Node, void *state), void *state) {
    long long index = 0;
    ListNode *Node{nullptr};
    try {
        pushFront(&Node, root);
        while (Node) {
//            printList(Node);
            const RBNode_t *rbNode = popBack(&Node);
            func(rbNode, state);
            if (rbNode->left) pushFront(&Node, rbNode->left);
            if (rbNode->right) pushFront(&Node, rbNode->right);
        }
    } catch(const std::exception &ex) {
       qDebug() << ex.what(); 
    }
}

void calcShiftX(const RBNode_t *root, void *state) {
    StateForWidthCalc *st = static_cast<StateForWidthCalc*>(state);
    StackNode *p = st->top;
    StackNode *snParent{0};
    const RBNode_t *rbNode = root;
    if (!root || !st) {
        throw;
    }
    p = SN_find_rw(st->top, rbNode);
    if (!p->nodePtr->parent) { st->top->x = 0; }
    else {
        snParent = SN_find_rw(st->top, rbNode->parent);
        if (snParent->nodePtr->right == rbNode) {
            p->x = snParent->x + snParent->width; 
        } else {
            p->x = snParent->x - snParent->width;
        }
//        qDebug() << "snParent->width: " << snParent->width << ", function: " << __FUNCTION__;
//        qDebug() << "p->x: " << (ssize_t)p->x << ", function: " << __FUNCTION__;
    }
}

Backend::Backend(QWidget *parent): QWidget(parent), factor{1}, factorX{1}, rbroot{nullptr}, ERROR_CODE{0}, treeChanged{false} {
//    addValue(&rbroot, );
               
    trW = new TreeWidth;
    stateFWCalc.minWidth = 5; //минимальная ширина

    QWidget *wgt = new QWidget(this);
    wgt->resize(200, 300);

    QGridLayout *gd = new QGridLayout(wgt);

//    QVBoxLayout *vlayout = new QVBoxLayout(wgt);

    QHBoxLayout *hlayout = new QHBoxLayout();
    QHBoxLayout *hlayoutX = new QHBoxLayout();

    QPushButton *button1 = new QPushButton("double it");
    QObject::connect(button1, SIGNAL(clicked()), this, SLOT(doubleFactor()));
    QPushButton *button2 = new QPushButton("half it");
    QObject::connect(button2, &QPushButton::clicked, this, [&]() {factor /= 2; repaint();});
    
    QPushButton *halfX = new QPushButton("half x");
    QObject::connect(halfX, SIGNAL(clicked()), this, SLOT(halfFactorX()));

    hlayout->addWidget(button1);
    hlayout->addWidget(button2);

    hlayoutX->addWidget(halfX);

    gd->addLayout(hlayout, 0, 0, 1, 2);
    gd->addLayout(hlayoutX, 1, 0, 1, 2);

//    vlayout->addWidget(hlayout);

    qte = new QTextEdit();
    QObject::connect(qte, &QTextEdit::textChanged, this, [this](){qDebug() << qte->toPlainText();});
    gd->addWidget(qte, 2, 0, 2, 2);


    QPushButton *buttonAddValue = new QPushButton("add value");
    QPushButton *buttonAddValueTest = new QPushButton("start addValue tests");
    QObject::connect(buttonAddValue, SIGNAL(clicked()), this, SLOT(slotAddValue()));
    QObject::connect(buttonAddValueTest, SIGNAL(clicked()), this, SLOT(startTestAddValue()));
    QPushButton *buttonDeleteValue = new QPushButton("delete value");
    QObject::connect(buttonDeleteValue, SIGNAL(clicked()), this, SLOT(slotDeleteValue()));

    gd->addWidget(buttonAddValue, 5, 0, 1, 1);
    gd->addWidget(buttonDeleteValue, 5, 1, 1, 1);
    gd->addWidget(buttonAddValueTest, 6, 0, 1, 1);

    QPushButton *buttonDeleteTree = new QPushButton("delete tree");
    QObject::connect(buttonDeleteTree, SIGNAL(clicked()), this, SLOT(slotDeleteTree()));
    gd->addWidget(buttonDeleteTree, 7, 0, 1, 2);
    
}

Backend::~Backend() {
    free(rbroot);
}

void Backend::halfFactorX() {
    factorX/=2;
    treeChanged = true;
    repaint();
}


void Backend::paintEvent(QPaintEvent *ev) {
    Q_UNUSED(ev);

    QPainter *QPainterPtr = new QPainter(this);
    QPainterPtr->setPen(QPen(Qt::white, 2, Qt::SolidLine, Qt::FlatCap));

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


        stateFWCalc.r = 50*factor;
        if (treeChanged) {
            freeStackNodes(&stateFWCalc);

            runOverTheTree(rbroot, calcWidthX, &stateFWCalc); 
            printStateNodes(&stateFWCalc);
            runWidthTheTree(rbroot, calcShiftX, &stateFWCalc);
        }
        
        StackNode *tmp = stateFWCalc.top;
        StackNode *tmpTop = tmp;
        StackNode *tmpParent;

        size_t addWidth = width()/2;
        size_t addHeight = 20;
        ssize_t heightFactor = 50;

        while (tmp) {
            QPainterPtr->setPen(QPen(Qt::white, 2, Qt::SolidLine, Qt::FlatCap));
            if (nodeColor(tmp->nodePtr)) QPainterPtr->setBrush(QBrush(Qt::red, Qt::SolidPattern));
            else QPainterPtr->setBrush(QBrush(Qt::black, Qt::SolidPattern));
            tmpParent = SN_find_rw(tmpTop, tmp->nodePtr->parent);
            if (tmpParent) {
                ssize_t x1 = (ssize_t)tmp->x * factorX + addWidth + stateFWCalc.r/2;
                ssize_t y1 = (ssize_t)tmp->y * heightFactor + addHeight;
                ssize_t x2 = (ssize_t)tmpParent->x * factorX + addWidth + stateFWCalc.r/2;
                ssize_t y2 = (ssize_t)tmpParent->y * heightFactor + addHeight + stateFWCalc.r;
                QPainterPtr->drawLine(x1, y1, x2, y2);
                qDebug() << "x1, y1: " << x1 << ", " << y1 << "\nx2, y2: " << x2 << ", " << y2 << "\n";
            } else qDebug() << "tmpParent hasn't founded";
//            qDebug() << "drawEllipse: x:" << (ssize_t)((ssize_t)tmp->x * factorX + addWidth) << ", y:" << tmp->y * heightFactor + addHeight << ", value: " << tmp->nodePtr->value;
            QPainterPtr->drawEllipse((ssize_t)tmp->x * factorX + addWidth, tmp->y * heightFactor + addHeight, stateFWCalc.r, stateFWCalc.r);
            //QPainterPtr->drawText(tmp->x + addWidth, tmp->y * 5, QString::number(tmp->nodePtr->value));
            QPainterPtr->drawText((ssize_t)tmp->x * factorX + addWidth + stateFWCalc.r/2, tmp->y * heightFactor + stateFWCalc.r/2 + addHeight, QString::number(tmp->nodePtr->value)); 
            tmp = tmp->previous;
        }

//
//        size_t yHeight = calcHeightY(st->top->nodePtr);
//        size_t yHFactor = 10;
//        size_t xShift{0};
//        if (st->top->nodePtr->parent) {
//            if (st->top->nodePtr->parent->left == st->top->nodePtr) xShift = -(st->top->width);
//            else xShift = st->top->width;
//        }
//        size_t r = 50;
//
//        if (node->color) st->painter->setBrush(QBrush(Qt::red, Qt::SolidPattern)); 
//        else st->painter->setBrush(QBrush(Qt::black, Qt::SolidPattern)); //        st->painter->drawEllipse(xShift, yHeight*yHFactor, r, r);
//        st->painter->drawText(xShift+r/2, yHeight*yHFactor+r/2, QString::number(node->value));
    }
    QPainterPtr->end();
    treeChanged = false;
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
    tmpValue = qte->toPlainText().trimmed().toLongLong();
    RBNode_t *tmp = addValue(&rbroot, tmpValue, &ERROR_CODE);
    
    if (ERROR_CODE) {
        qDebug() << "function addValue returned ERROR_CODE " << ERROR_CODE;
        qDebug() << "the value was " << tmpValue;
        qDebug() << "ERROR_CODE will be changed to 0";
        ERROR_CODE = 0;
    }

    if (tmp){
        treeChanged = true;
        qDebug() << "treeChanged is set true";
    } else {
        qDebug() << "addValue returned null";
    }
    //runWidthTheTree(rbroot, buildTree, trW);
    //runOverTheTree(rbroot, something, trW);
    repaint();
}


void Backend::slotDeleteValue() {
    tmpValue = qte->toPlainText().trimmed().toLongLong();
    deleteNode(&rbroot, tmpValue, &ERROR_CODE);
    if (ERROR_CODE) {
        qDebug() << "function deleteNode returned ERROR_CODE " << ERROR_CODE;
        qDebug() << "the value was " << tmpValue;
        qDebug() << "ERROR_CODE will be changed to 0";
        ERROR_CODE = 0;
    }
    treeChanged = true;
    repaint();
}

int * shiftArr(int *arr, int size, int index) {
    int i = index;
    for (; i < size-1; i++) arr[i] = arr[i+1];
    return arr;
}

void Backend::startTestAddValue() {

    int minD = 0, maxD = 0, splitInd = 0;
    int isDip = 0;
    std::string str = qte->toPlainText().trimmed().toUtf8().constData();
    for (int i = 0; i < str.length(); i++) {
       if (str[i] == ',') {
          minD = std::stoi(str.data());
          maxD = std::stoi(str.data() + i + 1);
          isDip = 1;
          break;
       }
    }

    int i = 0, randNum = 0, randInd = 0;
    int *numArr = 0, *numToDeleteArr = 0;
    int size = 0;
    srand(time(NULL));
    numArr = (int*)malloc(size * sizeof(int));
    numToDeleteArr = (int*)malloc(size * sizeof(int));

    qDebug() << "{minD, maxD} -> {" << minD << ", " << maxD << "}"; 

    if (isDip) {
       size = maxD-minD+1; 
    } else {
        size = qte->toPlainText().trimmed().toLongLong();
        minD = 0;
        maxD = size;
    }

    RBNode_t *tmp = NULL;
    for (i = 0; i < size; i++) {
        numArr[i] = i+minD;
        numToDeleteArr[i] = i+minD;
    }

    for (i = 0; i < size; i++) {
        //sleep(1);
        randInd = random() % (size - i);
        randNum = numArr[randInd];
        shiftArr(numArr, size, randInd);
        tmp = addValue(&rbroot, randNum, &ERROR_CODE);
        //deleteNode(&rbroot, randNum, &ERROR_CODE);
        if (tmp){
            treeChanged = true;
            qDebug() << "treeChanged is set true";
        } else {
            qDebug() << "addValue returned null";
        }
        repaint();

        if (ERROR_CODE) {
            qDebug() << "function deleteNode returned ERROR_CODE " << ERROR_CODE;
            qDebug() << "the value was " << tmpValue;
            qDebug() << "ERROR_CODE will be changed to 0";
            ERROR_CODE = 0;
        }
//        errorStr = checkErorCode(ERROR_CODE);
//        if (errorStr != "OK") {
//            fprintf(stderr, "Test №%d failed in addValue: %s\n", i+1, errorStr);
//            return 0;
//        }
        qDebug() << "Test №" << i+1 << " passed (additing Value)";
    }
}

void Backend::slotDeleteTree() {
    deleteTree(rbroot);
    rbroot = NULL;
    treeChanged = true;
    repaint();
}
