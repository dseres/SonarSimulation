#include <bits/stdc++.h>
#include <chrono>
#include <ctime>
using namespace std;

template <class T> struct Packet {
  T data;
  chrono::time_point<chrono::high_resolution_clock> timestamp =
      chrono::high_resolution_clock::now();

  Packet(T _data) : data(_data) {}

  Packet() = default;
  Packet(const Packet &other) = default;
  Packet(Packet &&other) = default;
  Packet &operator=(const Packet &) = default;
  Packet &operator=(Packet &&) = default;
  ~Packet() = default;
};

class Sensor {
private:
    long counter = 0;
    queue<Packet<long>> packet_queue;
public:
  Sensor() = default;
  Sensor(const Sensor &other) = default;
  Sensor(Sensor &&other) = default;
  Sensor &operator=(const Sensor &) = default;
  Sensor &operator=(Sensor &&) = default;
  ~Sensor() = default;

    //it is a runnable object
  void operator()(){

  }
};

int main(int argc, char **argv) {
  cout << "Helló világ!\n";
  return 0;
}