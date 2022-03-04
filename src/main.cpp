#include <bits/stdc++.h>
#include <ostream>
#include <thread>
using namespace std;

template <class T> struct Packet {
  T data;
  chrono::time_point<chrono::high_resolution_clock> timestamp =
      chrono::high_resolution_clock::now();

  Packet(T _data) : data(_data) {}

  friend ostream &operator<<(ostream &os, const Packet &p) {
    auto t_c = chrono::high_resolution_clock::to_time_t(p.timestamp);
    return os << "Packet{ data = " << p.data
              << "; timestamp = " << put_time(std::localtime(&t_c), "%F %T %z")
              << "}";
  }
};

class Sensor {
private:
  bool stop_ind = false;
  long counter = 0;
  mutex queue_mutex;
  queue<Packet<long>> packet_queue;

public:
  // it is a runnable object
  void operator()() {
    while (!stop_ind || counter<=10) {
      this_thread::sleep_for(1ms * (1 + rand() % 1000));
      auto packet = Packet<long>(counter++);
      cout<<"Sending packet :"<<packet<<endl;
      const lock_guard<mutex> lock(queue_mutex);
      packet_queue.push(packet);
    }
  }
};

class SonarPU {
private:
  Sensor main_sensor;
  Sensor sensor2;
  Sensor sensor3;
};

int main(int argc, char **argv) {
  cout << "Helló világ!\n";
  auto ts = chrono::high_resolution_clock::now();
  auto t_c = chrono::high_resolution_clock::to_time_t(ts);
  cout << put_time(std::localtime(&t_c), "%c") << endl;

  Sensor main_sensor;
  Sensor sensor2;
  Sensor sensor3;

  // thread tm(main_sensor);
  // thread t2(sensor2);
  // thread t3(&sensor3);

  // tm.join();
  // t2.join();
  // t3.join();

  return 0;
}