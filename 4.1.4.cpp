#include <iostream>
#include <thread>
#include <vector>
#include <queue>  
#include <chrono>
#include <mutex>

#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

using namespace std;

void imprimir(queue<int> fila, vector<int> carrinho, int m, int status){
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
    if (status==1)
        cout << "EMBARCANDO\n";
    if (status==2)
        cout << "PERCORRENDO\n";
    if (status==3)
        cout << "DESEMBARCANDO\n";

    for (int i = 0; i < m; i+=2)
        cout << " | " << carrinho.at(i);
    cout << " | " << endl;

    for (int i = 1; i < m; i+=2)
        cout << " | " << carrinho.at(i);

    cout << " | \nprox fila: ";

    for (int i = 1; i < m; i++){
        cout << fila.front() << ", ";
        fila.pop();
    }
}

void embarque (queue<int> &fila, vector<int>&carrinho, int n, int m){
    vector<thread> threads;
    mutex mut;
    for (int i = 0; i < m; i+=2) {
        
        threads.push_back(thread([](queue<int>&fila, vector<int>&carrinho, int n, int m, int i, mutex &mut){
            mut.lock();
                carrinho.at(i) = fila.front();
                fila.pop();
                imprimir(fila, carrinho, m, 1);
            mut.unlock();
            this_thread::sleep_for(2s);
            mut.lock();
                carrinho.at(i+1) = fila.front();
                fila.pop();
                imprimir(fila, carrinho, m, 1);
            mut.unlock();
           this_thread::sleep_for(1s);
            
        },ref(fila), ref(carrinho), n, m, i, ref(mut)));
    }

    for (auto &a : threads)
        a.join();
}

void desembarque (queue<int> &fila, vector<int>&carrinho, int n, int m){
    vector<thread> threads;
    mutex mut;
    for (int i = 0; i < m; i+=2) {
        threads.push_back(thread([](queue<int>&fila, vector<int>&carrinho, int n, int m, int i, mutex &mut){
            mut.lock();
                fila.push(carrinho.at(i));
                carrinho.at(i)=0;
                imprimir(fila, carrinho, m, 3);
            mut.unlock();
            this_thread::sleep_for(2s);
            mut.lock();
                fila.push(carrinho.at(i+1));
                carrinho.at(i+1)=0;
                imprimir(fila, carrinho, m, 3);
            mut.unlock();
            this_thread::sleep_for(1s);
        },ref(fila), ref(carrinho), n, m, i, ref(mut)));
    }

    for (auto &a : threads)
        a.join();
}

int main() {
    srand (time(NULL)); 

    int n = rand () % 160 + 40;
    int m = rand () % 16 + 4;
    m += (m%2!=0)?true:false;

    queue<int> fila;
    vector<int> carrinho;

    for (int i = 0; i < n; i++)
        fila.push(i+1);
    for (int i = 0; i < m; i++)
        carrinho.push_back(0);
    
    while (true!=false){
        embarque(fila, carrinho, n, m);
        imprimir(fila, carrinho, m, 2);
        this_thread::sleep_for(5s);
        desembarque(fila, carrinho, n, m);
    }

    return 0;
}

