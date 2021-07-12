#include <iostream> 
#include <vector>
#include <stack>
#include <map>
#include <random>
#include <QDebug>
#include <QPushButton>
#include <QBoxLayout>
#include <QTextEdit>
#include <QLabel>
#include <string>
#include <new>
#include <cctype>

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


bool checkConsistencyOfTheTree(const RBNode_t *root) {
    if (!root) return true;

    if (root->parent) {
        std::cout << "ERROR: root has parent: { root->value: " << root->value << ", root->parent->value: " << root->parent->value << std::endl;
        return false;
    }

    std::stack<const RBNode_t *> v;
    std::map<const RBNode_t *, int> m;

    v.push(root);
    m[root] = 0;

    while (!v.empty()) {
        const RBNode_t *cur = v.top();
        if (cur->left) {
            if (!m[cur]) {
                if (cur->left->parent != cur) {
                    std::cout << "ERROR: " << __PRETTY_FUNCTION__ << ": v.size() = " << v.size() << ": left has wrong parent" << std::endl;
                    std::cout << "ERROR: " << "current value is " << cur->value << ", left value is " << cur->left->value << std::endl;
                    std::cout << "ERROR: " << "current == current->left: " << (cur == cur->left) << std::endl;
                    if (cur->left->parent) {
                        std::cout << "ERROR: " << "left has parent, its value: " << cur->left->parent->value << std::endl;
                    } else {
                        std::cout << "ERROR: " << "left has no parent" << std::endl;
                    }
                    return false;
                }
                v.push(cur->left);
                m[cur] = 1;
                continue;
            }
        } else if (!m[cur]){
            m[cur] = 1;
        }
        if (cur->right) {
            if (m[cur] == 1) {
                if (cur->right->parent != cur) {
                    std::cout << "ERROR: " << __PRETTY_FUNCTION__ << ": v.size() = " << v.size() << ": right has wrong parent" << std::endl;
                    std::cout << "ERROR: " << "current value is " << cur->value << ", right value is " << cur->right->value << std::endl;
                    std::cout << "ERROR: " << "{current, current->right, current->right->par}\n" << "{" << cur << ", " << cur->right << ", " << cur->right->parent << "}" << std::endl;
                    if (cur->right->parent) {
                        std::cout << "ERROR: " << "right has parent, its value: " << cur->right->parent->value << std::endl;
                    } else {
                        std::cout << "ERROR: " << "right has no parent" << std::endl;
                    }
                    return false;
                }
                v.push(cur->right);
                m[cur] = 2;
                continue;
            }
        } else if (m[cur] == 1) {
            m[cur] = 2;
        }
        if ((!cur->left && !cur->right) || m[cur] == 2) {
            v.pop();
        }
    }

    return true;
}

void runOverTheTree(RBNode_t *root, void(*func)(const RBNode_t *Node, void* state), void* state) {
    static size_t y_lvl = 0;

    if (!root) {
        return;
    }

    //qDebug() << "runOverTheTree's called";
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
    while (p && p->nodePtr != node) {
        p = p->previous; 
    }
    return p;
}

StackNode* SN_find_rw(StackNode *top, const RBNode_t *node) {
    StackNode *p = top; 
    //qDebug() << "StackNode *top is " << top << "\nRBNode_t node is " << node << "\n----------------------------\n";
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

Backend::Backend(QApplication &_app, QWidget *parent): QWidget(parent), app{_app} {
//    addValue(&rbroot, );
               
    trW = new TreeWidth;
    stateFWCalc.minWidth = 10; //минимальная ширина

    QWidget *wgt = new QWidget(this);
    //wgt->resize(200, auto);
    wgt->setFixedWidth(200);

    QGridLayout *gd = new QGridLayout(wgt);

//    QVBoxLayout *vlayout = new QVBoxLayout(wgt);

    QHBoxLayout *hlayout = new QHBoxLayout();
    QHBoxLayout *hlayoutX = new QHBoxLayout();

    QPushButton *button1 = new QPushButton("double it");
    QObject::connect(button1, SIGNAL(clicked()), this, SLOT(doubleFactor()));
    QPushButton *button2 = new QPushButton("half it");
    QObject::connect(button2, &QPushButton::clicked, this, [&]() {factor /= 1.5; treeChanged = true; repaint();});
    
    QPushButton *halfX = new QPushButton("half x");
    QObject::connect(halfX, SIGNAL(clicked()), this, SLOT(halfFactorX()));

    QPushButton *doubleX = new QPushButton("double x");
    QObject::connect(doubleX, SIGNAL(clicked()), this, SLOT(doubleFactorX()));

    hlayout->addWidget(button1);
    hlayout->addWidget(button2);

    hlayoutX->addWidget(halfX);
    hlayoutX->addWidget(doubleX);

    gd->addLayout(hlayout, 0, 0, 1, 2);
    gd->addLayout(hlayoutX, 1, 0, 1, 2);

//    vlayout->addWidget(hlayout);

    qte = new QTextEdit();
    QObject::connect(qte, &QTextEdit::textChanged, this, [this](){qDebug() << qte->toPlainText();});
    gd->addWidget(qte, 2, 0, 2, 2);


    QPushButton *buttonAddValue = new QPushButton("add value");
    QPushButton *buttonStartTest = new QPushButton("start tests");
    QObject::connect(buttonAddValue, SIGNAL(clicked()), this, SLOT(slotAddValue()));
    QObject::connect(buttonStartTest, SIGNAL(clicked()), this, SLOT(slotStartTest()));
    
    numberOfTests = new QTextEdit();
    numberOfTests->setPlaceholderText(QStringLiteral("tests number"));
    numberOfTests->setFixedHeight((QFontMetrics(numberOfTests->currentFont()).lineSpacing())*2);
    numberOfTests->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    numberOfTests->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    QPushButton *buttonDeleteValue = new QPushButton("delete value");
    QObject::connect(buttonDeleteValue, SIGNAL(clicked()), this, SLOT(slotDeleteValue()));

    gd->addWidget(buttonAddValue, 5, 0, 1, 1);
    gd->addWidget(buttonDeleteValue, 5, 1, 1, 1);
    gd->addWidget(buttonStartTest, 6, 0, 1, 1);
    gd->addWidget(numberOfTests, 6, 1, 1, 1);

    QPushButton *buttonDeleteTree = new QPushButton("delete tree");
    QPushButton *buttonSaveTree = new QPushButton("saveTree");
    QPushButton *buttonRollBackTree = new QPushButton("Roll back");

    QObject::connect(buttonSaveTree, SIGNAL(clicked()), this, SLOT(slotSaveTree()));
    QObject::connect(buttonDeleteTree, SIGNAL(clicked()), this, SLOT(slotDeleteTree()));
    QObject::connect(buttonRollBackTree, SIGNAL(clicked()), this, SLOT(slotRollBackTree()));

    QPushButton *buttonHide = new QPushButton("hide");
    QPushButton *buttonShow = new QPushButton("show", this);
    buttonShow->hide();
    QObject::connect(buttonHide, &QPushButton::clicked, this, [w = wgt, buttonShow]{w->hide(); buttonShow->show();});
    QObject::connect(buttonShow, &QPushButton::clicked, this, [wgt, buttonShow]{buttonShow->hide(); wgt->show();});

    previousAction= new QLabel();
    nextAction = new QLabel();
    previousAction->setText("actionPrev");
    nextAction->setText("actionNext");
    
    gd->addWidget(buttonDeleteTree, 7, 0, 1, 2);
    gd->addWidget(buttonSaveTree, 8, 0, 1, 1);
    gd->addWidget(buttonRollBackTree, 8, 1, 1, 1);
    gd->addWidget(previousAction, 9, 0, 1, 1);
    gd->addWidget(nextAction, 9, 1, 1, 1);

    gd->addWidget(buttonHide, 10, 0, 1, 1);

    exitButton = new QPushButton("exit", this);
    exitButton->move(this->width() - exitButton->width(), 0);
    QObject::connect(exitButton, &QPushButton::clicked, &app, &QCoreApplication::quit, Qt::QueuedConnection);
    
}

Backend::~Backend() {
    free(rbroot);
}

void Backend::halfFactorX() {
    factorX/=2;
    treeChanged = true;
    repaint();
}

void Backend::doubleFactorX() {
    factorX *= 2;
    treeChanged = true;
    repaint();
}


void Backend::slotSaveTree() {
    RBNode_t *cpRoot = copyTreeFunc(rbroot, &ERROR_CODE); 
    if (ERROR_CODE) {
        qDebug() << "ERROR: function copyTreeFunc returned ERROR_CODE " << ERROR_CODE; 
    }
    try {
        treeBackup.emplace_back(cpRoot);
    } catch (const std::exception &ex) {
        qDebug() << "ERROR in vector: " << ex.what();
    }
    qDebug() << "Saving tree completed succesefull";
    qDebug() << "The added vertex address is " << treeBackup.back();
}

void Backend::slotRollBackTree() {
    if (!rbroot || !treeBackup.size()) {
        return;
    }
    deleteTree(rbroot);
    rbroot = treeBackup.back();
    treeBackup.pop_back();
    treeChanged = true;
    repaint();
}

void Backend::paintEvent(QPaintEvent *ev) {
    Q_UNUSED(ev);

    QPainter *QPainterPtr = new QPainter(this);
    QPainterPtr->setPen(QPen(Qt::white, 2, Qt::SolidLine, Qt::FlatCap));

    if (!rbroot) {
        QPainterPtr->eraseRect(0, 0, width(), height());
    } else {
        QPainterPtr->eraseRect(0, 0, width(), height());
        stateFWCalc.r = 50*factor;
        if (treeChanged) {
            freeStackNodes(&stateFWCalc);

            runOverTheTree(rbroot, calcWidthX, &stateFWCalc); 
//            printStateNodes(&stateFWCalc);
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
                //qDebug() << "x1, y1: " << x1 << ", " << y1 << "\nx2, y2: " << x2 << ", " << y2 << "\n";
            } //else qDebug() << "tmpParent hasn't founded";

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
    factor *= 1.5;
    repaint();
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
        qDebug() << "checkConsistencyOfTheTree(rbroot): " << checkConsistencyOfTheTree(rbroot);
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
    for (; i < size - 1; i++) arr[i] = arr[i + 1];
//    for (i = index; i < size; i++) std::cout << arr[i] << " ";
    return arr;
}
void Backend::slotStartTest() {
    int maxVertexes = 40;
    int countOfTests = numberOfTests->toPlainText().trimmed().toLongLong();
    if (numberOfTests->toPlainText().isEmpty()) {
        qDebug() << "input count of Tests";
        return;
    }
    int minD = 0, maxD = 0, isDip = 0, size = 0;
    std::string inputStr = qte->toPlainText().trimmed().toUtf8().constData();
    if (qte->toPlainText().isEmpty()) {
        qDebug() << "input the range of the tests";
        return;
    }
    for (size_t i = 0; i < inputStr.length(); i++) {
       if (inputStr[i] == ',') {
          minD = std::stoi(inputStr.data());
          maxD = std::stoi(inputStr.data() + i + 1);
          isDip = 1;
          break;
       }
    }

    if (isDip) {
        size = maxD - minD + 1;
    } else {
        size = std::stoi(inputStr.data());
        minD = 0;
        maxD = size;
    }
    
    qDebug() << "{minD, maxD} -> {" << minD << ", " << maxD << "}"; 

    std::random_device rd;
    std::mt19937 mersen(rd());
    int randNum; RBNode_t *returnedValue;

    StackNode *p = stateFWCalc.top;
    while (p) {
        if (p->nodePtr) if (std::find(currentTree.begin(), currentTree.end(), p->nodePtr->value) == currentTree.end()) currentTree.push_back(p->nodePtr->value);
        p = p->previous;
    }

    float currentChance = currentTree.size() / (double)maxVertexes * 100; char isIncreased = 2;
    std::string choice;
     
    while (countOfTests) {
        std::cout << "currentTree: ";
        std::copy(currentTree.begin(), currentTree.end(), std::ostream_iterator<size_t>(std::cout, " "));
        std::cout << std::endl;
        if (isIncreased == 1) {
            qDebug() << "isIncreased is 1, increasing the chance...";
            currentChance = currentTree.size() / (double)maxVertexes * 100;
        } else if (isIncreased == 0) {
            qDebug() << "isIncreased is 0, decreasing the chance...";
            currentChance = currentTree.size() / (double)maxVertexes * 100;
        }
        std::cout << "currentChance is " << currentChance << std::endl;
        float chance = mersen() % 100 + 1 + (float)((mersen() % 100 + 1) / 100.0); //генерирует шанс с дробной частью до 2 знаков после запятой
        std::cout << "generated chance is " << chance << std::endl;
        if (chance > currentChance) { 
           randNum = mersen() % (maxD - minD) + minD; 
           qDebug() << "RandNum to add was generated, it's " << randNum;
           if (std::find(currentTree.begin(), currentTree.end(), randNum) == currentTree.end()) {
                qDebug() << "no such value in the tree, additing value...";
                if (treeBackup.size() > 100) {
                     treeBackup.erase(treeBackup.begin());
                }
                slotSaveTree();
                returnedValue = addValue(&rbroot, randNum, &ERROR_CODE); 
                if (ERROR_CODE) {
                    qDebug() << "addValue function has returned the ERROR_CODE " << ERROR_CODE;
                    if (ERROR_CODE == 1) {
                        qDebug() << "additing value is already is the tree";
                    }
                    qDebug() << "Do you want to continue? (y/n)";
                    std::getline(std::cin, choice);
                    if (std::tolower(choice[0]) == 'y') {isIncreased = 2; continue;}
                    else { return; }
                } else {
                    treeChanged = true;
                    repaint();
                    qDebug() << "---------------\nadditing value completed succesefully\nnew node's value is " << returnedValue->value;
                    std::cout << "new node parent's value is ";
                    if (returnedValue->parent) std::cout << returnedValue->parent->value << std::endl;
                    else std::cout << "<no parent>\n";
                    std::cout << "new node left child's value is "; 
                    if (returnedValue->left) std::cout << returnedValue->left->value << std::endl;
                    else std::cout << "<no left child>\n";
                    std::cout << "new node right child's value is "; 
                    if (returnedValue->right) std::cout << returnedValue->right->value << std::endl; 
                    else std::cout << "<no right child>\n";
                    qDebug() << "---------------";
                    currentTree.push_back(returnedValue->value);
                    isIncreased = 1;
                }
           } else {
                isIncreased = 2;
           }
        } else {
           randNum = mersen() % currentTree.size(); 
           qDebug() << "RandNum to delete was generated, it's " << randNum;
           if (treeBackup.size() > 40) {
                treeBackup.erase(treeBackup.begin());
           }
           slotSaveTree();
        
            deleteNode(&rbroot, currentTree[randNum], &ERROR_CODE); 
            if (ERROR_CODE) {
                qDebug() << "deleting function has returned the ERROR_CODE " << ERROR_CODE;
                qDebug() << "Do you want to continue? (y/n)";
                std::getline(std::cin, choice);
                if (std::tolower(choice[0]) == 'y') {isIncreased = 2; continue;}
                else {return;}
            } else {
                qDebug() << "---------------\ndeleting value completed succesefully\n----------------";
                currentTree.erase(currentTree.begin() + randNum);
                isIncreased = 0;
            }
        }
        countOfTests--;
    }

}

//void Backend::startTestAddValue() {
//    int minD = 0, maxD = 0;
//    int isDip = 0;
//    std::string str = qte->toPlainText().trimmed().toUtf8().constData();
//    for (size_t i = 0; i < str.length(); i++) {
//       if (str[i] == ',') {
//          minD = std::stoi(str.data());
//          maxD = std::stoi(str.data() + i + 1);
//          isDip = 1;
//          break;
//       }
//    }
//
//    int i = 0, randNum = 0, randInd = 0;
//    int *numArr = 0, *numToDeleteArr = 0;
//    int size = 0;
//    std::random_device rd;
//    std::mt19937 mersen(rd());
//
//    qDebug() << "{minD, maxD} -> {" << minD << ", " << maxD << "}"; 
//
//    if (isDip) {
//       size = maxD-minD+1; 
//    } else {
//        size = qte->toPlainText().trimmed().toLongLong();
//        minD = 0;
//        maxD = size;
//    }
//
//    numArr = (int*)malloc(size * sizeof(int));
//    numToDeleteArr = (int*)malloc(size * sizeof(int));
//
//    RBNode_t *tmp = NULL;
//    for (i = 0; i < size; i++) {
//        numArr[i] = i+minD;
//        numToDeleteArr[i] = i+minD;
//    }
//
//    for (i = 0; i < size; i++) {
//        //sleep(1);
//        if (size - i - 1 == 0) {
//            randInd = 0; 
//        } else {
//            randInd = mersen() % (size - i);
//        }
//        randNum = numArr[randInd];
//        shiftArr(numArr, size - i, randInd);
//        tmp = addValue(&rbroot, randNum, &ERROR_CODE);
//        //deleteNode(&rbroot, randNum, &ERROR_CODE);
//        if (tmp){
//            treeChanged = true;
////            qDebug() << "treeChanged is set true";
////            qDebug() << "checkConsistencyOfTheTree(rbroot): " << checkConsistencyOfTheTree(rbroot);
//        } else {
//            qDebug() << "addValue returned null";
//        }
//        repaint();
//
//        if (ERROR_CODE) {
//            qDebug() << "function deleteNode returned ERROR_CODE " << ERROR_CODE;
//            qDebug() << "the value was " << tmpValue;
//            qDebug() << "ERROR_CODE will be changed to 0";
//            ERROR_CODE = 0;
//        }
////        errorStr = checkErorCode(ERROR_CODE);
////        if (errorStr != "OK") {
////            fprintf(stderr, "Test №%d failed in addValue: %s\n", i+1, errorStr);
////            return 0;
////        }
//        qDebug() << "Test №" << i+1 << " passed (additing Value)";
//    }
//}

void Backend::slotDeleteTree() {
    deleteTree(rbroot);
    rbroot = NULL;
    treeChanged = true;
    repaint();
}

void Backend::resizeEvent(QResizeEvent *ev) {
    exitButton->move(this->width() - exitButton->width(), 0);
}
