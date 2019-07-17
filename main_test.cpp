//
// Created by fanqiang on 2019/7/17.
//

#include <cstdio>
#include <iostream>

using namespace std;
class Test {
public:
    char* a;
    Test() {
        a = "200\0";
    }
};

void getTest(Test **pTest){
    *pTest = new Test();
}

int main(int argc, char **argv) {
    Test *test;
    getTest(&test);
    cout << (*test->a) <<endl;
}

