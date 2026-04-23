#include <iostream>
#include <thread>

using std::cout;
using std::cin;
using std::string;
using std::thread;
using std::exit;
using std::endl;

void T1(){
    cout << "Hello multi-threading! ff to exit" << endl;
}

int main(){
    string Terminal;
    thread t1 (T1);
    cout << "";
    cin >> Terminal;
    t1.join();
    if ("ff" == Terminal){
        exit(255);
    }
    return 0;
