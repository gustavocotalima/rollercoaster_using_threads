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

void print(queue<int> people, vector<int> train, int m, int status){
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
    if (status==1)
        cout << "EMBARKING\n";
    if (status==2)
        cout << "TRAVELING\n";
    if (status==3)
        cout << "DISEMBARKING\n";

    for (int i = 0; i < m; i+=2)
        cout << " | " << train.at(i);
    cout << " | " << endl;

    for (int i = 1; i < m; i+=2)
        cout << " | " << train.at(i);

    cout << " | \nQueue: ";

    for (int i = 1; i < m; i++){
        cout << people.front() << ", ";
        people.pop();
    }
}

void embark (queue<int> &people, vector<int>&train, int n, int m){
    vector<thread> threads;
    mutex mut;
    for (int i = 0; i < m; i+=2) {
        
        threads.push_back(thread([](queue<int>&people, vector<int>&train, int n, int m, int i, mutex &mut){
            mut.lock();
                train.at(i) = people.front();
                people.pop();
                print(people, train, m, 1);
            mut.unlock();
            this_thread::sleep_for(2s);
            mut.lock();
                train.at(i+1) = people.front();
                people.pop();
                print(people, train, m, 1);
            mut.unlock();
           this_thread::sleep_for(1s);
            
        },ref(people), ref(train), n, m, i, ref(mut)));
    }

    for (auto &a : threads)
        a.join();
}

void disembark (queue<int> &people, vector<int>&train, int n, int m){
    vector<thread> threads;
    mutex mut;
    for (int i = 0; i < m; i+=2) {
        threads.push_back(thread([](queue<int>&people, vector<int>&train, int n, int m, int i, mutex &mut){
            mut.lock();
                people.push(train.at(i));
                train.at(i)=0;
                print(people, train, m, 3);
            mut.unlock();
            this_thread::sleep_for(2s);
            mut.lock();
                people.push(train.at(i+1));
                train.at(i+1)=0;
                print(people, train, m, 3);
            mut.unlock();
            this_thread::sleep_for(1s);
        },ref(people), ref(train), n, m, i, ref(mut)));
    }

    for (auto &a : threads)
        a.join();
}

int main() {
    srand (time(NULL)); 

    int n = rand () % 160 + 40;
    int m = rand () % 16 + 4;
    m += (m%2!=0)?true:false;

    queue<int> people;
    vector<int> train;

    for (int i = 0; i < n; i++)
        people.push(i+1);
    for (int i = 0; i < m; i++)
        train.push_back(0);
    
    while (1){
        embark(people, train, n, m);
        print(people, train, m, 2);
        this_thread::sleep_for(5s);
        disembark(people, train, n, m);
    }

    return 0;
}

