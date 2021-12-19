/* Algo No.
1. m=1 wait() for grab res, signal() for release res
2. m=2 odd-num grab left res first, and then right res. even-num grab right res, and then left res.
3. m=3 only pick up both resource.
*/
/* Algo detail
start 1-5 sec
eating 1-5 sec
think 5-10 sec
 */
/* process No.
n = 3-11 create philosopher
 */
#include <iostream>
#include <pthread.h>
#include <functional>
#include <vector>
#include <thread>
#include <mutex>
#include <typeinfo>
#include <condition_variable>
#include <random>
using namespace std;
unsigned int maxNum;

//global
mutex alock;
condition_variable sem;
//define semaphore
class semaphore{
private:
    unsigned int m;//semaphore res
public:
    semaphore()= default;
    semaphore(unsigned int x):m(x){}
    semaphore(const semaphore& sema){
        m=sema.m;
    }
    //semaphore wait
    void down(){
        unique_lock<mutex> locker(alock);
        while(m==0){
            //use condition_variable stop thread
            cout << "(thread id ="<<&this_thread::get_id<<"is waiting)"<<endl;
            sem.wait(locker);
        }
        alock.lock();
        m--;
        alock.unlock();
    }
    void up() {
        alock.lock();
        m++;
        sem.notify_all();
        alock.unlock();
    }
    void operator = (const unsigned int &t){
        m = t;
    }
    bool operator == (const unsigned int &t){
        return m==t;
    }
};
vector<semaphore> chopsticks;


// algo method No.
enum algoNum{
    method1=0,
    method2=1,
    method3=2
};


// define philosopher

class philosopher{
private:
    size_t philNum=0;
    vector<function<void(int i, int maxNum, semaphore &user)>> funcs;
    philosopher() {};
    philosopher(const philosopher& phil){
        funcs = phil.funcs;
    }
    ~philosopher(){}
public:
    // create singleton object
    static philosopher *instance;
    static philosopher *singleton(){
        if(instance){
            return instance;
        }
        instance = new philosopher();
        return instance;
    }
    //init function algo
    void add(initializer_list<function<void(int i, int maxNum, semaphore &user)>> algo){
        for(auto funAlgo=algo.begin(); funAlgo != algo.end(); funAlgo++){
            funcs.push_back(*funAlgo);
        }
    }
    // add new algo
    void add(function<void(int i, int maxNum, semaphore &user)> algo){
        funcs.push_back(algo);
    }
    // get algo method
    function<void(int i, int maxNum, semaphore &user)> get(algoNum i){
        return funcs[i];
    }

};


// algo
auto funA = [](int i, int maxNum, semaphore &user) -> void{

};
auto funB = [](int i, int maxNum, semaphore &user) -> void{
    auto grabRightChopstick = [&](int id,int eat) -> void{
        alock.lock();
        chopsticks[id].down();
        cout<< "philosopher Num: "<< id << " done eat :"<< eat+1 <<" thread id = "<<this_thread::get_id()<<"is grabing right chopstick"<<endl;
        alock.unlock();
        this_thread::sleep_for(chrono::seconds(random()%5));
    };
    auto grabLeftChopstick = [&](int id,int eat) -> void {
        alock.lock();
        chopsticks[(id+1)%maxNum].down();
        cout<< "philosopher Num: "<< id << " done eat :"<< eat+1 <<" thread id = "<<this_thread::get_id()<<"is grabing right chopstick"<<endl;
        alock.unlock();
        this_thread::sleep_for(chrono::seconds(random()%5));
    };
    auto eating = [=](int id,int eat) -> void {
        unique_lock<mutex> locker(alock);
        alock.lock();
        cout<< "philosopher Num: "<< i << " done eat :"<< eat+1 <<" thread id = "<<this_thread::get_id()<<"is waiting"<<endl;
        chopsticks[(id+1)%maxNum].up();
        chopsticks[(id)%maxNum].up();
        alock.unlock();
        this_thread::sleep_for(chrono::seconds(random()%5));
    };
    auto thinking = [=](){
        alock.lock();
        sem.notify_all();
        cout<< "philosopher Num: "<< i <<" thread id = "<<this_thread::get_id()<<"is thinking"<<endl;
        alock.unlock();
        std::this_thread::sleep_for(chrono::seconds(random()%5+5));
    };
    user.down();
    int eat = 0;
    int id = i;
    while (eat<10){

        if (id%2){
            grabLeftChopstick(i,eat);
            grabRightChopstick(i,eat);
            eating(i,eat);
            thinking();

        } else{
            grabRightChopstick(i,eat);
            grabLeftChopstick(i,eat);
            eating(i,eat);
            thinking();
        }

    }
};

auto funC = [](int i, int maxNum, semaphore &user) -> void {
    int eat= 0;
    auto eating = [&]() -> void {
        unique_lock<mutex> locker(alock);

        while (chopsticks[i]==0 || chopsticks[(i+1)%maxNum]==0){
            cout<< "philosopher Num: "<< i << " done eat :"<< eat+1 <<" thread id = "<<this_thread::get_id()<<"is waiting"<<endl;
            sem.wait(locker);
        }
        alock.lock();
        chopsticks[i].down();
        chopsticks[(i+1)%maxNum].down();
        cout << "philosopher Num: "<< i << " eat number: "<< eat+1 <<" thread id = "<<this_thread::get_id()<<" is eating"<<endl;
        eat++;
        std::this_thread::sleep_for(chrono::seconds(random()%5));
        chopsticks[i].up();
        chopsticks[(i+1)%maxNum].up();
        sem.notify_all();
        alock.unlock();
    };
    auto thinking = [=](){
        alock.lock();
        sem.notify_all();
        cout<< "philosopher Num: "<< i <<" thread id = "<<this_thread::get_id()<<"is thinking"<<endl;
        alock.unlock();
        chrono::microseconds s(20000);
        this_thread::sleep_for(s);
        std::this_thread::sleep_for(chrono::seconds(random()%5+5));
    };
    while (eat<10){
        thinking();
        eating();
    }
};


static semaphore user;
philosopher *philosopher::instance;
int main(){
    // 加入3種演算法
    philosopher::singleton()->add({funA, funB, funC});
    cout<< "plz insert the method number:";
    unsigned int  chooseAlgo;
    cin >> chooseAlgo;
    cout<< "plz insert the philosopher number:";
    cin >> maxNum;
    // philosopher user
    user = maxNum-1;
    //philosopher list
    vector<thread> phils;

    //init chopsticks
    for(size_t i=0; i<maxNum; i++){
        chopsticks.emplace_back(1);
    }

    // init philosopher algo
    switch (chooseAlgo) {
        case 1:
            for(size_t i=0; i<maxNum;i++){
                phils.emplace_back(thread(philosopher::singleton()->get(method1), i, maxNum, user));
            }
            break;
        case 2:
            for(size_t i=0; i<maxNum;i++){
                phils.emplace_back(philosopher::singleton()->get(method2), i, maxNum, user);
            }
            break;
        case 3:
            for(size_t i=0; i<maxNum;i++){
                phils.emplace_back(philosopher::singleton()->get(method3), i, maxNum, user);
            }
            break;
        default:
            break;
    }

    // wait all thread done
    for(auto phil = phils.begin();phil != phils.end();phil++){
        phil->join();
    }
    return 0;
}
