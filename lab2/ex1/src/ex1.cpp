#include <chrono>
#include <climits>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using namespace std;
using namespace filesystem;
using namespace chrono;

//斐波那契结点
struct FibonacciHeapNode {
    int                key;     //结点
    int                degree;  //度
    FibonacciHeapNode* left;    //左兄弟
    FibonacciHeapNode* right;   //右兄弟
    FibonacciHeapNode* parent;  //父结点
    FibonacciHeapNode* child;   //第一个孩子结点
    bool               marked;  //是否被删除第1个孩子
};

typedef FibonacciHeapNode FibNode;

//斐波那契堆
struct FibonacciHeap {
    int                 keyNum;          //堆中结点个数
    FibonacciHeapNode*  min;             //最小堆，根结点
    int                 maxNumOfDegree;  //最大度
    FibonacciHeapNode** cons;            //指向最大度的内存区域
};

typedef FibonacciHeap FibHeap;

//将x从双链表移除
inline void FibNodeRemove(FibNode* x) {
    x->left->right = x->right;
    x->right->left = x->left;
}

//将x堆结点加入y结点之前
void FibNodeAdd(FibNode* x, FibNode* y) {
    x->left        = y->left;
    y->left->right = x;
    x->right       = y;
    y->left        = x;
}

//初始化一个空的Fibonacci Heap
FibHeap* FibHeapMake() {
    FibHeap* heap = NULL;
    heap          = (FibHeap*)malloc(sizeof(FibHeap));
    if (NULL == heap) {
        puts("Out of Space!!");
        exit(1);
    }
    memset(heap, 0, sizeof(FibHeap));
    return heap;
}

//初始化结点x
FibNode* FibHeapNodeMake() {
    FibNode* x = NULL;
    x          = (FibNode*)malloc(sizeof(FibNode));
    if (NULL == x) {
        puts("Out of Space!!");
        exit(1);
    }
    memset(x, 0, sizeof(FibNode));
    x->left = x->right = x;
    return x;
}

//将堆的最小结点移出，并指向其有兄弟
static FibNode* FibHeapMinRemove(FibHeap* heap) {
    FibNode* min = heap->min;
    if (heap->min == min->right) {
        heap->min = NULL;
    } else {
        FibNodeRemove(min);
        heap->min = min->right;
    }
    min->left = min->right = min;
    return min;
}

//将x根结点链接到y根结点
void FibHeapLink(FibHeap* heap, FibNode* x, FibNode* y) {
    FibNodeRemove(x);
    if (NULL == y->child) {
        y->child = x;
    } else {
        FibNodeAdd(x, y->child);
    }
    x->parent = y;
    y->degree++;
    x->marked = false;
}

//堆结点x插入fibonacci heap中
int FibHeapInsert(FibHeap* heap, FibNode* x) {
    if (0 == heap->keyNum) {
        heap->min = x;
    } else {
        FibNodeAdd(x, heap->min);
        x->parent = NULL;
        if (x->key < heap->min->key) {
            heap->min = x;
        }
    }
    heap->keyNum++;
    return heap->keyNum;
}

//将值插入Fibonacci Heap
static int FibHeapInsertKey(FibHeap* heap, int key) {
    FibNode* x = NULL;
    x          = FibHeapNodeMake();
    x->key     = key;
    FibHeapInsert(heap, x);
    return heap->keyNum;
}

//将数组内的值插入Fibonacci Heap
void FibHeapInsertKeys(FibHeap* heap, int keys[], int keyNum) {
    for (int i = 0; i < keyNum; i++) {
        FibHeapInsertKey(heap, keys[i]);
    }
}

//开辟FibHeapConsolidate函数哈希所用空间
static void FibHeapConsMake(FibHeap* heap) {
    int old              = heap->maxNumOfDegree;
    heap->maxNumOfDegree = int(log(heap->keyNum * 1.0) / log(2.0)) + 1;
    if (old < heap->maxNumOfDegree) {
        //因为度为heap->maxNumOfDegree可能被合并,所以要maxNumOfDegree + 1
        heap->cons = (FibNode**)realloc(
            heap->cons, sizeof(FibHeap*) * (heap->maxNumOfDegree + 1));
        if (NULL == heap->cons) {
            puts("Out of Space!");
            exit(1);
        }
    }
}

//合并左右相同度数的二项树
void FibHeapConsolidate(FibHeap* heap) {
    int      D, d;
    FibNode *w = heap->min, *x = NULL, *y = NULL;
    FibHeapConsMake(heap);
    D = heap->maxNumOfDegree + 1;
    for (int i = 0; i < D; i++) {
        *(heap->cons + i) = NULL;
    }

    while (NULL != heap->min) {
        x = FibHeapMinRemove(heap);
        d = x->degree;
        while (NULL != *(heap->cons + d)) {
            y = *(heap->cons + d);
            if (x->key > y->key) {
                swap(x, y);
            }
            FibHeapLink(heap, y, x);
            *(heap->cons + d) = NULL;
            d++;
        }
        *(heap->cons + d) = x;
    }
    heap->min = NULL;

    for (int i = 0; i < D; i++) {
        if (*(heap->cons + i) != NULL) {
            if (NULL == heap->min) {
                heap->min = *(heap->cons + i);
            } else {
                FibNodeAdd(*(heap->cons + i), heap->min);
                if ((*(heap->cons + i))->key < heap->min->key) {
                    heap->min = *(heap->cons + i);
                }
            }
        }
    }
}

//抽取最小结点
FibNode* FibHeapExtractMin(FibHeap* heap) {
    FibNode *x = NULL, *z = heap->min;
    if (z != NULL) {
        while (NULL != z->child) {
            x = z->child;
            FibNodeRemove(x);
            if (x->right == x) {
                z->child = NULL;
            } else {
                z->child = x->right;
            }
            FibNodeAdd(x, z);
            x->parent = NULL;
        }

        FibNodeRemove(z);
        if (z->right == z) {
            heap->min = NULL;
        } else {
            heap->min = z->right;
            FibHeapConsolidate(heap);
        }
        heap->keyNum--;
    }
    return z;
}

//修改度数
void renewDegree(FibNode* parent, int degree) {
    parent->degree -= degree;
    if (parent->parent != NULL) {
        renewDegree(parent->parent, degree);
    }
}

//切断x与父节点y之间的链接，使x成为一个根
static void FibHeapCut(FibHeap* heap, FibNode* x, FibNode* y) {
    FibNodeRemove(x);
    renewDegree(y, x->degree);
    if (x == x->right) {
        y->child = NULL;
    } else {
        y->child = x->right;
    }
    x->parent = NULL;
    x->left = x->right = x;
    x->marked          = false;
    FibNodeAdd(x, heap->min);
}

//级联剪切
static void FibHeapCascadingCut(FibHeap* heap, FibNode* y) {
    FibNode* z = y->parent;
    if (NULL != z) {
        if (y->marked == false) {
            y->marked = true;
        } else {
            FibHeapCut(heap, y, z);
            FibHeapCascadingCut(heap, z);
        }
    }
}

//减小一个关键字
void FibHeapDecrease(FibHeap* heap, FibNode* x, int key) {
    FibNode* y = x->parent;
    if (x->key < key) {
        puts("new key is greater than current key!");
        exit(1);
    }
    x->key = key;

    if (NULL != y && x->key < y->key) {
        //破坏了最小堆性质，需要进行级联剪切操作
        FibHeapCut(heap, x, y);
        FibHeapCascadingCut(heap, y);
    }
    if (x->key < heap->min->key) {
        heap->min = x;
    }
}

//删除结点
int FibHeapDelete(FibHeap* heap, FibNode* x) {
    FibHeapDecrease(heap, x, INT_MIN);
    FibHeapExtractMin(heap);
    return heap->keyNum;
}

//被FibHeapSearch调用
static FibNode* FibNodeSearch(FibNode* x, int key) {
    FibNode *w = x, *y = NULL;
    if (x != NULL) {
        do {
            if (w->key == key) {
                y = w;
                break;
            } else if (NULL != (y = FibNodeSearch(w->child, key))) {
                break;
            }
            w = w->right;
        } while (w != x);
    }
    return y;
}

//堆内搜索关键字
FibNode* FibHeapSearch(FibHeap* heap, int key) {
    return FibNodeSearch(heap->min, key);
}

//被FibHeapDestory调用
static void FibNodeDestory(FibNode* x) {
    FibNode *p = x, *q = NULL;
    while (p != NULL) {
        FibNodeDestory(p->child);
        q = p;
        if (p->left == x) {
            p = NULL;
        } else {
            p = p->left;
        }
        free(q->right);
    }
}

//销毁堆
void FibHeapDestory(FibHeap* heap) {
    FibNodeDestory(heap->min);
    free(heap);
    heap = NULL;
}

//合并堆
FibHeap* FibHeapUnion(FibHeap* heap1, FibHeap *heap2){
    FibHeap *tmp;
    if(heap1 ==NULL){
        return heap2;
    }
    if(heap2 ==NULL){
        return heap1;
    }
    if(heap2->maxNumOfDegree> heap1->maxNumOfDegree){
        tmp = heap1;
        heap1 = heap2;
        heap2 = tmp;
    }
    if(heap1->min ==NULL){
        heap1->min = heap2->min;
        heap1->keyNum = heap2->keyNum;
        free(heap2->cons);
        free(heap2);
    }
    else if(heap2->min==NULL){
        free(heap2->cons);
        free(heap2);
    }
    else{
        FibNodeAdd(heap1->min, heap2->min);
        if(heap1->min->key>heap2->min->key){
            heap1->min = heap2->min;
        }
        heap1->keyNum +=heap2->keyNum;
        free(heap2->cons);
        free(heap2);
    }
    return heap1;
}


int main() {
    path p0(__FILE__);
    auto p  = p0.parent_path().parent_path();
    auto p1 = p0.parent_path().parent_path();
    auto p2 = p0.parent_path().parent_path();
    p2 += "\\output\\time.txt";
    p1 += "\\output\\result.txt";
    p += "\\input\\2_1_input.txt";
    cout << "Loading File from " << p << endl;
    ifstream file;
    file.open(p, ios::in);
    int         i = 0;
    vector<int> input;
    while (i < 500) {
        int number = 0;
        file >> number;
        input.push_back(number);
        i++;
    }

    int key1[50], key2[100], key3[150], key4[200];
    for (i = 0; i < 50; i++) {
        key1[i] = input[i];
    }
    for (; i < 150; i++) {
        key2[i - 50] = input[i];
    }
    for (; i < 300; i++) {
        key3[i - 150] = input[i];
    }
    for (; i < 500; i++) {
        key4[i - 300] = input[i];
    }
    FibHeap *heap1, *heap2, *heap3, *heap4;
    FibNode* x = NULL;
    heap1      = FibHeapMake();
    heap2      = FibHeapMake();
    heap3      = FibHeapMake();
    heap4      = FibHeapMake();
    FibHeapInsertKeys(heap1, key1, 50);
    FibHeapInsertKeys(heap2, key2, 100);
    FibHeapInsertKeys(heap3, key3, 150);
    FibHeapInsertKeys(heap4, key4, 200);
    cout << "Finish building FibHeap" << endl << endl;

    ofstream timefile, outputfile;
    timefile.open(p2, ios::out);
    outputfile.open(p1, ios::out);

    // heap1
    steady_clock::time_point t1 = steady_clock::now();
    outputfile << "H1:" << endl
               << FibHeapInsertKey(heap1, 249) << " "
               << FibHeapInsertKey(heap1, 830) << " ";
    outputfile << heap1->min->key << " ";
    x = FibHeapSearch(heap1, 127);
    FibHeapDelete(heap1, x);
    outputfile << heap1->keyNum << " ";
    x = FibHeapSearch(heap1, 141);
    FibHeapDelete(heap1, x);
    outputfile << heap1->keyNum << " " << heap1->min->key << " ";
    x = FibHeapSearch(heap1, 75);
    FibHeapDecrease(heap1, x, 61);
    outputfile << heap1->min->key << " ";
    x = FibHeapSearch(heap1, 198);
    FibHeapDecrease(heap1, x, 169);
    outputfile << heap1->min->key << " ";
    x = FibHeapExtractMin(heap1);
    outputfile << x->key << " ";
    x = FibHeapExtractMin(heap1);
    outputfile << x->key << " " << endl;
    steady_clock::time_point t2    = steady_clock::now();
    auto                     time1 = t2 - t1;
    timefile << duration<double, milli>(time1).count() << "ms" << endl;

    // heap2
    t1 = steady_clock::now();
    outputfile << "H2: " << endl
               << FibHeapInsertKey(heap2, 816) << " " << heap2->min->key << " "
               << FibHeapInsertKey(heap2, 345) << " ";
    x = FibHeapExtractMin(heap2);
    outputfile << x->key << " ";
    x = FibHeapSearch(heap2, 504);
    outputfile << FibHeapDelete(heap2, x) << " ";
    x = FibHeapSearch(heap2, 203);
    outputfile << FibHeapDelete(heap2, x) << " ";
    x = FibHeapSearch(heap2, 296);
    FibHeapDecrease(heap2, x, 87);
    outputfile << heap2->min->key << " ";
    x = FibHeapSearch(heap2, 278);
    FibHeapDecrease(heap2, x, 258);
    outputfile << heap2->min->key << " " << heap2->min->key << " ";
    x = FibHeapExtractMin(heap2);
    outputfile << x->key << endl;
    t2    = steady_clock::now();
    time1 = t2 - t1;
    timefile << duration<double, milli>(time1).count() << "ms" << endl;

    t1 = steady_clock::now();
    x  = FibHeapExtractMin(heap3);
    outputfile << "H3: " << endl
               << x->key << " " << heap3->min->key << " "
               << FibHeapInsertKey(heap3, 262) << " ";
    x = FibHeapExtractMin(heap3);
    outputfile << x->key << " " << FibHeapInsertKey(heap3, 830) << " "
               << heap3->min->key << " ";
    x = FibHeapSearch(heap3, 134);
    outputfile << FibHeapDelete(heap3, x) << " ";
    x = FibHeapSearch(heap3, 177);
    outputfile << FibHeapDelete(heap3, x) << " ";
    x = FibHeapSearch(heap3, 617);
    FibHeapDecrease(heap3, x, 360);
    outputfile << heap3->min->key << " ";
    x = FibHeapSearch(heap3, 889);
    FibHeapDecrease(heap3, x, 353);
    outputfile << heap3->min->key << endl;
    t2    = steady_clock::now();
    time1 = t2 - t1;
    timefile << duration<double, milli>(time1).count() << "ms" << endl;

    //heap4
    t1 = steady_clock::now();
    x = FibHeapSearch(heap4, 708);
    outputfile<<"H4: "<<endl<<heap4->min->key<<" "<<FibHeapDelete(heap4, x)<<" "<<FibHeapInsertKey(heap4, 281)<<" "<<FibHeapInsertKey(heap4, 347)<<" "<<heap4->min->key<<" ";
    x = FibHeapSearch(heap4, 415);
    outputfile<<FibHeapDelete(heap4, x)<<" ";
    x = FibHeapExtractMin(heap4);
    outputfile<<x->key<<" ";
    x = FibHeapSearch(heap4, 620);
    FibHeapDecrease(heap4, x, 354);
    outputfile<<heap4->min->key<<" ";
    x = FibHeapSearch(heap4, 410);
    FibHeapDecrease(heap4, x, 80);
    outputfile<<heap4->min->key<<" ";
    x = FibHeapExtractMin(heap4);
    outputfile<<x->key<<endl;
    t2 = steady_clock::now();
    time1 = t2 - t1;
    timefile << duration<double, milli>(time1).count() << "ms" << endl;

    //step5
    t1 = steady_clock::now();
    auto heap5 = FibHeapUnion(heap1, heap2);
    heap5 = FibHeapUnion(heap5, heap3);
    heap5 = FibHeapUnion(heap5, heap4);
    cout<<"Heap Union step complete"<<endl;
    t2 = steady_clock::now();
    time1 = t2 - t1;
    timefile << duration<double, milli>(time1).count() << "ms" << endl;

    //step6
    t1 = steady_clock::now();
    x = FibHeapExtractMin(heap5);
    outputfile<<"H5: "<<endl<<x->key<<" "<<heap5->min->key<<" ";
    x = FibHeapSearch(heap5, 800);
    outputfile<<FibHeapDelete(heap5, x)<<" "<<FibHeapInsertKey(heap5, 267)<<" "<<FibHeapInsertKey(heap5, 351)<<" ";
    x = FibHeapExtractMin(heap5);
    outputfile<<x->key<<" ";
    x = FibHeapSearch(heap5, 478);
    FibHeapDecrease(heap5, x, 444);
    outputfile<<heap5->min->key<<" ";
    x = FibHeapSearch(heap5, 559);
    FibHeapDecrease(heap5, x, 456);
    outputfile<<heap5->min->key<<" "<<heap5->min->key<<" ";
    x = FibHeapSearch(heap5, 929);
    outputfile<<FibHeapDelete(heap5, x)<<endl;
    t2 = steady_clock::now();
    time1 = t2 - t1;
    timefile << duration<double, milli>(time1).count() << "ms" << endl;
    return 0;
}