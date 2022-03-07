#include <bits/stdc++.h>
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
  string name;
  bool stop_ind = false;
  long counter = 0;
  int min_wait_msec;
  int max_wait_msec;

  function<void(Sensor *)> notify;

public:
  mutex queue_mutex;
  queue<Packet<long>> packet_queue;

public:
  Sensor(string name, int min_wait_msec = 1, int max_wait_msec = 1000)
      : name(name), min_wait_msec(min_wait_msec), max_wait_msec(max_wait_msec) {
  }

  thread start() { return thread(&Sensor::run, this); }

  void stop() { stop_ind = true; }

  // it is a runnable object
  void run() {
    while (!stop_ind) {
      send_packet(Packet<long>(counter++));
      sleep();
    }
  }

  void connect_to(function<void(Sensor *)> notify) { this->notify = notify; }

  const string &get_name() { return name; }

private:
  void sleep() {
    int sleep_msec =
        min_wait_msec + rand() % (max_wait_msec - min_wait_msec + 1);
    this_thread::sleep_for(1ms * sleep_msec);
  }

  void send_packet(Packet<long> &&packet) {
    cout << "Sensor " << name << " sending packet :" << packet << endl;
    {
      const lock_guard<mutex> lock(queue_mutex);
      packet_queue.push(packet);
    }
    notify(this);
  }
};

class CentralUnit {
private:
  Sensor *main_sensor;
  Sensor *sec_sensor1;
  Sensor *sec_sensor2;

  Sensor *actual;

  bool stop_ind = false;
  long counter = 0;

public:
  mutex cv_mutex;
  condition_variable cv;

public:
  CentralUnit(Sensor *main_sensor, Sensor *sec_sensor1, Sensor *sec_sensor2)
      : main_sensor(main_sensor), sec_sensor1(sec_sensor1),
        sec_sensor2(sec_sensor2) {}

  thread start() { return thread(&CentralUnit::run, this); }

  void stop() { stop_ind = true; }

  // it is a runnable object
  void run() {
    while (!stop_ind) {
      unique_lock<mutex> lk(cv_mutex);
      if (cv_status::no_timeout == cv.wait_for(lk, 1s)) {
        process_packets();
      }
    }
  }

  void notify(Sensor *actual) {
    {
      lock_guard<mutex> lk(cv_mutex);
      this->actual = actual;
    }
    cv.notify_all();
  }

private:
  void process_packets() {
    print_packet(main_sensor);
    print_packet(sec_sensor1);
    print_packet(sec_sensor2);
  }

  void print_packet(Sensor *sensor) {
    const lock_guard<mutex> lock(sensor->queue_mutex);
    while (!sensor->packet_queue.empty()) {
      cout << "Central unit received a packet from sensor "
           << sensor->get_name() << ":" << sensor->packet_queue.front() << "\n";
      sensor->packet_queue.pop();
    }
  }
};

class Simulator {
  Sensor main_sensor{"Main", 100, 1000};
  Sensor sec_sensor1{"Secondary1", 100, 300};
  Sensor sec_sensor2{"Secondary2", 1000, 3000};
  CentralUnit cu{&main_sensor, &sec_sensor1, &sec_sensor2};

  volatile std::sig_atomic_t signal_status;

public:
  void signal_handler(int sigsignal_statusnal) {
    this->signal_status = signal_status;
    cout << "Stopping sensors...\n";
    main_sensor.stop();
    sec_sensor1.stop();
    sec_sensor2.stop();
    cout << "Stopping central unit...\n";
    cu.stop();
  }

  void run() {
    main_sensor.connect_to(function<void(Sensor *)>(
        std::bind(&CentralUnit::notify, &cu, std::placeholders::_1)));
    sec_sensor1.connect_to(function<void(Sensor *)>(
        std::bind(&CentralUnit::notify, &cu, std::placeholders::_1)));
    sec_sensor2.connect_to(function<void(Sensor *)>(
        std::bind(&CentralUnit::notify, &cu, std::placeholders::_1)));
    thread tm = main_sensor.start();
    thread ts1 = sec_sensor1.start();
    thread ts2 = sec_sensor2.start();
    thread tcu = cu.start();

    tm.join();
    ts1.join();
    ts2.join();
    tcu.join();
  }
};

Simulator s;

void signal_handler(int signal) { s.signal_handler(signal); }

int main(int argc, char **argv) {
  std::signal(SIGINT, signal_handler);
  std::signal(SIGABRT, signal_handler);
  std::signal(SIGTERM, signal_handler);
  s.run();
  return 0;
}
