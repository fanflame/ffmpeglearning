#include <iostream>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}
using namespace std;

int main() {
    cout << "Hello, World!" << std::endl;
    cout << __VERSION__ << endl;
    cout << avcodec_configuration() << endl;

    return 0;
}