#include <stdio.h>
#include <math.h>

#define EPS 1e-6

long long binSearch(const long long * arr, long long len, long long el) {
    long long left{0}, right{len-1};
    long long middle{0};
    while (left <= right) {
        middle = (left + right) / 2;
        if (arr[middle] > el) right = middle - 1;
        else if (arr[middle] < el) left = middle + 1;
        else return middle;
    }
    return -1;
}

int valid(double x) {
    return x>5.25;
}

//struct Data {
//    int x, y;
//};
//
//typedef double MyDouble;
//
//double func(const struct Data * d) {
//    return d->x;
//}

double search(double a, double b, int (*validation_function) (double)) {
    double middle{0};
    long long direction{0};
    direction = (*validation_function)(a) ? 0 : 1;
    // printf("abs(a - b): %lf\n", abs(a-b));
    // printf("direction: %lld\n", direction);
    while (abs(a - b) > EPS) {
        middle = (a + b)/2;
        // printf("middle: %lf\n", middle);
        if ((*validation_function)(middle)) {
            if (!(*validation_function)(middle - EPS)) {
                return middle;
            }
            direction ? b = middle - EPS : a = middle + EPS;
            // printf("a = %lf, b=%lf\n", a, b);
        } else {
            direction ? a = middle + EPS : b = middle - EPS;
            // printf("a = %lf, b=%lf\n", a, b);
        }
    }
    return middle;
}


int main() {
    double x = search(-100, 300, &valid);
    printf("x = %lf\n", x);
    return 0;
}
