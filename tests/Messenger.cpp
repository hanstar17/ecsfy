#include "Messenger.h"

Messenger_Worker::~Messenger_Worker() {
  stop_ = true;
  if (thread_.joinable()) {
    thread_.join();
  }
}

void Messenger_Worker::run() {
  while (!queue_.empty()) {
    auto event = std::move(queue_.front());
    queue_.pop();
    event->dispatch(listeners_[event->id()]);
  }
}

void Messenger_Worker::runAsync() {
  thread_ = std::thread([this]() -> void {
    while (!stop_) {
      std::unique_lock<std::mutex> lk(mut_);
      cv_.wait(lk, [this](void) { return !queue_.empty(); });
      auto event = std::move(queue_.front());
      queue_.pop();
      lk.unlock();
      event->dispatch(listeners_[event->id()]);
    }
  });
}

Messenger::Messenger(size_t workerCount, bool populateWorkerThreads)
    : threaded_(populateWorkerThreads), workers_(threaded_ ? workerCount : 1) {}
