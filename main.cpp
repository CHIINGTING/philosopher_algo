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
using namespace std;
unsigned int maxNum;

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
        m--;
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
};
vector<semaphore> chopsticks;
enum algoNum{
    method1=0,
    method2=1,
    method3=2
};
// define philosopher
class philosopher{
private:
    size_t philNum=0;
    vector<function<void()>> funcs;
    philosopher() {};
    philosopher(const philosopher& phil){
        funcs = phil.funcs;
    }
    philosopher(size_t i) {
        funcs.push_back(funA);
        funcs.push_back(funB);
        funcs.push_back(funC);
        philNum = i;
        cout << "in philosopher(size_t i): "<< i << endl;
    };
    ~philosopher(){}
public:
    // create singleton object
    static philosopher *instance;
    static philosopher *singleton(int i){
        if(instance){
            instance->philNum = i;
            return instance;
        }
        instance = new philosopher(i);
        return instance;
    }
    //init function algo
    void add(initializer_list<function<void()>> algo){
        for(auto funAlgo=algo.begin(); funAlgo != algo.end(); funAlgo++){
            funcs.push_back(*funAlgo);
        }
    }
    // add new algo
    void add(function<void()> algo){
        funcs.push_back(algo);
    }
    // get algo method
    function<void()> get(algoNum i){
        return funcs[i];
    }
    function<void()> funA = []() -> void{
        /*  auto eating = []() -> void {
              unique_lock<mutex> locker(alock);
              uint64_t current_id =(uint64_t) this_thread::get_id();
              chopsticks[current_id % maxNum]
          };*/
        unique_lock<mutex> locker(alock);
        cout << "Num: "<< instance->philNum <<" thread ID: "<<this_thread::get_id()<<endl;
    };
    function<void()> funB = []() -> void{};
    function<void()> funC = []() -> void{};
};



philosopher *philosopher::instance;
int main(){
    // 加入3種演算法
    //philosopher::singleton()->add({funA, funB, funC});
    cout<< "plz insert the method number:";
    unsigned int  chooseAlgo;
    cin >> chooseAlgo;
    cout<< "plz insert the philosopher number:";
    cin >> maxNum;

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
                phils.emplace_back(philosopher::singleton(i)->get(method1));
            }
            break;
        case 2:
            for(size_t i=0; i<maxNum;i++){
                phils.emplace_back(philosopher::singleton(i)->get(method2));
            }
            break;
        case 3:
            for(size_t i=0; i<maxNum;i++){
                phils.emplace_back(philosopher::singleton(i)->get(method3));
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
