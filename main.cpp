/* Algo No.
1. m=1 wait() for grab res, signal() for release res
2. m=2 odd-num grab left res first, and then right res. even-num grab right res, and then left res.
3. m=3 only pick signal both resource.
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
#include <functional>
#include <vector>
#include <thread>
#include <mutex>
#include <typeinfo>
#include <condition_variable>
#include <mutex>

using namespace std;
unsigned int maxNum;

//global
mutex alock;
std::condition_variable sem;
//unique_lock<mutex> locker(alock);
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
    unsigned int getValue(){
        return m;
    }

    //semaphore wait
    void wait(){
        //cout << "before locker semaphore wait"<<endl;
        unique_lock<mutex> locker(alock);
       // cout << "after locker semaphore wait"<<endl;
        while(m==0){
            //use condition_variable stop thread
          //  cout << "(thread id ="<< this_thread::get_id() <<" is waiting)"<<endl;
            sem.wait(locker);
        }
        m--;
        //cout << "semaphore wait"<<endl;
    }
    void wait(int id){
        //cout << "before locker semaphore wait"<<endl;
        unique_lock<mutex> locker(alock);
       // cout << "after locker semaphore wait"<<endl;
        while(m==0){
            //use condition_variable stop thread
            cout << "the philosopher: "<< id+1 << " thread id ="<< this_thread::get_id() <<" is waiting"<<endl;
            sem.wait(locker);
        }
        //alock.lock();
        m--;
        //alock.unlock();
        //cout << "semaphore wait"<<endl;
    }
    void signal() {
      //  cout<<"in singal"<<endl;
        alock.lock();
      //  cout<<"in singal lock"<<endl;
        m++;
        alock.unlock();
        sem.notify_all();
    }
    void signal(int id) {
       // cout<<"in singal"<<endl;
        alock.lock();
      //  cout<<"in singal lock"<<endl;
        m++;
        alock.unlock();
        sem.notify_all();
    }
    void operator = (const unsigned int &t){
        m = t;
    }
    bool operator == (const unsigned int &t){
        return this->m == t;
    }
    unsigned int& operator -(const unsigned int &t){
        this->m = this->m-t;
        return this->m;
    }
};

vector<semaphore> chopsticks;
// limit user number to eat
semaphore user;
// limit one to eat
semaphore single=1;


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
    vector<function<void(int i, int maxNum)>> funcs;
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
    void add(initializer_list<function<void(int i, int maxNum)>> algo){
        for(auto funAlgo=algo.begin(); funAlgo != algo.end(); funAlgo++){
            funcs.push_back(*funAlgo);
        }
    }
    // get algo method
    function<void(int i, int maxNum)> get(algoNum i){
        return funcs[i];
    }

};


// algo
auto funA = [](int id, int maxNum) -> void{
   // std::condition_variable matex;
    int eat=0;
    auto eating = [&]() -> void{
        /*
        while (chopsticks[id]==0){
            unique_lock<mutex> locker(alock);
            alock.lock();
            cout<< "philosopher Num: "<< id+1 << " done eat :"<< eat <<" thread id = "<<this_thread::get_id()<<" is waiting"<<endl;
            alock.unlock();
            sem.wait(locker);
        }
         */
        chopsticks[id].wait();
        /*
        while (chopsticks[(id+1)%maxNum]==0){
            unique_lock<mutex> locker(alock);
            cout<< "philosopher Num: "<< id+1 << " done eat :"<< eat <<" thread id = "<<this_thread::get_id()<<" is waiting"<<endl;
            sem.wait(locker);
        }*/
        chopsticks[(id+1)%maxNum].wait();
        alock.lock();
        cout<< "philosopher Num: "<< id+1 <<" thread id = "<<this_thread::get_id()<<" is eating"<<endl;
        alock.unlock();
        this_thread::sleep_for(chrono::seconds(random()%5));
        eat++;
        alock.lock();
        cout<< "philosopher Num: "<< id+1 <<" thread id = "<<this_thread::get_id()<<" done eating and he eat: " << eat << " times." <<endl;
        alock.unlock();
        chopsticks[id].signal();
        chopsticks[(id+1)%maxNum].signal();
    };
    auto thinking = [&]() -> void{
        alock.lock();
        cout<< "philosopher Num: "<< id+1 << " done eat :"<< eat+1 <<" thread id = "<<this_thread::get_id()<<" is thinking"<<endl;
        alock.unlock();
        this_thread::sleep_for(chrono::seconds(random()%5+5));
        //cout<< "done thinking"<<endl;
    };
    while(eat < 10){
        single.wait(id);
       // cout<< "this is method 1"<<endl;
        eating();
        thinking();
        single.signal(id);
    }
    alock.lock();
    cout << "the philosopher: "<<id+1<< " end his meal"<<endl;
    alock.unlock();
};

auto funB = [](int id, int maxNum) -> void{
    int eat = 0;
    auto grabRightChopstick = [&]() -> void{
        /*
        while (chopsticks[id]==0){
            unique_lock<mutex> locker(alock);
            cout<< "philosopher Num: "<< id+1 << " done eat: "<< eat <<" thread id = "<<this_thread::get_id()<<" is waiting"<<endl;
            sem.wait(locker);
        }*/
        chopsticks[id].wait();
        alock.lock();
        cout<< "philosopher Num: "<< id+1 << " done eat: "<< eat <<" thread id = "<<this_thread::get_id()<<" is grabing right chopstick"<<endl;
        alock.unlock();
        this_thread::sleep_for(chrono::seconds(random()%5));
    };
    auto grabLeftChopstick = [&]() -> void {
       /* while (chopsticks[(id+1)%maxNum]==0){
            unique_lock<mutex> locker(alock);
            cout<< "philosopher Num: "<< id+1 << " done eat: "<< eat <<" thread id = "<<this_thread::get_id()<<" is waiting"<<endl;
            sem.wait(locker);
        }*/
        chopsticks[(id + 1) % maxNum].wait();
        alock.lock();
        cout<< "philosopher Num: "<< id+1 << " done eat: "<< eat <<" thread id = "<<this_thread::get_id()<<" is grabing left chopstick"<<endl;
        alock.unlock();
        this_thread::sleep_for(chrono::seconds(random()%5));
    };
    auto eating = [&]() -> void {
        alock.lock();
        cout<< "philosopher Num: "<< id+1 <<" thread id = "<<this_thread::get_id()<<" is eating "<<endl;
        this_thread::sleep_for(chrono::seconds(random()%5));
        eat = eat +1;
        cout<< "philosopher Num: "<< id+1 <<" thread id = "<<this_thread::get_id()<<" done eating and he eat: " << eat << " times." <<endl;
        alock.unlock();
    };
    auto releaseLeftChopstick = [&]() -> void {
        chopsticks[(id + 1) % maxNum].signal();
        alock.lock();
        cout<< "philosopher Num: "<< id+1 << " done eat: "<< eat <<" thread id = "<<this_thread::get_id()<<" is releasing left chopstick"<<endl;
        this_thread::sleep_for(chrono::seconds(random()%5));
        alock.unlock();
        //sem.notify_all();
    };
    auto releaseRightChopstick = [&]() -> void {
        chopsticks[(id) % maxNum].signal();
        alock.lock();
        cout<< "philosopher Num: "<< id+1 << " done eat: "<< eat <<" thread id = "<<this_thread::get_id()<<" is releasing left chopstick"<<endl;
        this_thread::sleep_for(chrono::seconds(random()%5));
        alock.unlock();
        //sem.notify_all();
    };
    auto thinking = [=]()-> void {
        alock.lock();
        cout<< "philosopher Num: "<< id+1 <<" thread id = "<<this_thread::get_id()<<" is thinking"<<endl;
        std::this_thread::sleep_for(chrono::seconds(random()%5+5));
        alock.unlock();
        //sem.notify_all();
    };
    // the philosopher join meal Number

    while (eat<10){
        user.wait(id);
        if (id%2){
            grabLeftChopstick();
            grabRightChopstick();
            eating();
            releaseLeftChopstick();
            releaseRightChopstick();
            thinking();

        } else{
            grabRightChopstick();
            grabLeftChopstick();
            eating();
            releaseRightChopstick();
            releaseLeftChopstick();
            thinking();
        }
        user.signal();
    }

    cout << "the philosopher: "<<id+1<< " end his meal"<<endl;
};

auto funC = [](int id, int maxNum) -> void {
    int eat = 0;
    auto grabChopsticks = [&]() -> void {
        //
        while (chopsticks[id] == 0 || chopsticks[(id + 1) % maxNum] == 0){
            unique_lock<mutex> locker(alock);
            cout << "philosopher Num: " << id+1 << " done eat :" << eat << " thread id = " << this_thread::get_id() << " is waiting" << endl;
            sem.wait(locker);
        }
        cout << "philosopher Num: " << id +1 << " thread id = " << this_thread::get_id() << " is grabing" << endl;
        chopsticks[id]=0;
        chopsticks[(id + 1) % maxNum]=0;
    };
    auto eating = [&]() -> void {
        alock.lock();
        cout << "philosopher Num: " << id +1 << " thread id = " << this_thread::get_id() << " is eating" << endl;
        eat++;
        chopsticks[id]=1;
        chopsticks[(id + 1) % maxNum]=1;
        cout<< "philosopher Num: "<< id+1 <<" thread id = "<<this_thread::get_id()<<" done eating and he eat: " << eat << " times." <<endl;
        alock.unlock();
        //sem.notify_all();
        std::this_thread::sleep_for(chrono::seconds(random()%5));
    };
    auto thinking = [&]()->void{
        alock.lock();
        cout<< "philosopher Num: "<< id+1 <<" thread id = "<<this_thread::get_id()<<" is thinking"<<endl;
        alock.unlock();
        sem.notify_all();
        std::this_thread::sleep_for(chrono::seconds(random()%5+5));
    };
    while (eat<10){
        grabChopsticks();
        eating();
        thinking();
    }
    cout << "the philosopher: "<<id+1<< " end his meal"<<endl;
};

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
  //      cout<<"chopsticks["<<i<<"]:"<<chopsticks[i].getValue()<<endl;
    }

    // init philosopher algo
    switch (chooseAlgo) {
        case 1:
            for(size_t i=0; i<maxNum;i++){
                phils.emplace_back(thread(philosopher::singleton()->get(method1), i, maxNum));
            }
            break;
        case 2:
            for(size_t i=0; i<maxNum;i++){
                phils.emplace_back(thread(philosopher::singleton()->get(method2), i, maxNum));
            }
            break;
        case 3:
            for(size_t i=0; i<maxNum;i++){
                phils.emplace_back(thread(philosopher::singleton()->get(method3), i, maxNum));
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
