#pragma once
#include <atomic>
#include <condition_variable>
#include <functional>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <unordered_map>
#include <vector>

// message type id support
using Messenger_OpaqueMessageTypeId = const void*;

template <class T>
struct Messenger_MessageTypeId {
  using OpaqueType = Messenger_OpaqueMessageTypeId;
  static constexpr OpaqueType opaque() {
    return static_cast<Messenger_OpaqueMessageTypeId>(&ptr);
  }

  static const T* const ptr;
};

template <class T>
const T* const Messenger_MessageTypeId<T>::ptr = nullptr;

// event
struct Messenger_EventBase {
 public:
  virtual ~Messenger_EventBase() = default;
  virtual Messenger_OpaqueMessageTypeId id() = 0;
};

template <class T>
struct Messenger_Event : public Messenger_EventBase {
 public:
  Messenger_Event(const T& message);
  virtual Messenger_OpaqueMessageTypeId id() override;

  T message_;
};

// dispatcher
template <class T>
class Messenger_DispatcherBase {
 public:
  virtual ~Messenger_DispatcherBase() = default;
  virtual void dispatch(Messenger_EventBase&) = 0;
};

template <class T>
class Messenger_Dispatcher : public Messenger_EventBase {
 public:
  virtual void dispatch(Messenger_EventBase&) override;
};


// worker
class Messenger_Worker {
 public:
  Messenger_Worker() = default;
  ~Messenger_Worker();

  template <class MessageType>
  void registerListener(std::function<void(const MessageType&)> callback);

  template <class MessageType>
  void sendMessage(const MessageType&);

  void run();

  void runAsync();

 private:
  std::queue<std::unique_ptr<Messenger_EventBase>> queue_;
  std::unordered_map<Messenger_OpaqueMessageTypeId, std::vector<void*>> listeners_;

  std::thread thread_;
  std::mutex mut_;
  std::condition_variable cv_;
  std::atomic<bool> stop_;
};

// messenger
class Messenger {
  using Worker = Messenger_Worker;
  using MessageTypeId = Messenger_OpaqueMessageTypeId;

 public:
  Messenger(size_t workerCount, bool populateWorkerThreads);

  template <class MessageType>
  void registerMessageType(size_t assignedWorkerIndex = 0);

  template <class MessageType>
  void registerListener(std::function<void(const MessageType&)> callback);

  template <class MessageType>
  void sendMessage(const MessageType& message);

 private:
  bool isThreaded() const;

  Worker& getWorker(size_t index);

  const bool threaded_;
  std::vector<Worker> workers_;
  std::unordered_map<MessageTypeId, size_t> messageWorkerIndices_;

  template <class MessageType>
  MessageTypeId getMessageTypeId();
};

/// INLINE FUNCTION DEFINITIONS ///

// event
template <class MessageType>
Messenger_Event<MessageType>::Messenger_Event(const MessageType& message)
    : message_(message) {}

template <class MessageType>
Messenger_OpaqueMessageTypeId Messenger_Event<MessageType>::id() {
  return Messenger_MessageTypeId<MessageType>::opaque();
}

// dispatcher
template <class MessageType>
void Messenger_Dispatcher<MessageType>::dispatch(Messenger_EventBase& event) {
  for (auto& listener : listeners) {
    listener(static_cast<Messenger_Event<MessageType>(event).message);
  }
}

// worker
template <class MessageType>
void Messenger_Worker::registerListener(std::function<void(const MessageType&)> callback) {
  listeners_[Messenger_MessageTypeId<MessageType>::opaque()].push_back(
      reinterpret_cast<void*>(handler));
}

template <class MessageType>
void Messenger_Worker::sendMessage(const MessageType& msg) {
  queue_.emplace(std::make_unique<Messenger_Event<MessageType>>(msg));
}

// messenger
template <class MessageType>
void Messenger::registerMessageType(size_t assignedWorkerIndex) {
  messageWorkerIndices_[getMessageTypeId<MessageType>()] = assignedWorkerIndex;
}

template <class MessageType>
void Messenger::registerListener(void (*handler)(const MessageType&)) {
  Worker& worker = getWorker(messageWorkerIndices_[getMessageTypeId<MessageType>()]);
  worker.registerListener<MessageType>(handler);
}

template <class MessageType>
void Messenger::sendMessage(const MessageType& msg) {
  Worker& worker = getWorker(messageWorkerIndices_[getMessageTypeId<MessageType>()]);
  worker.sendMessage(msg);
  if (!isThreaded()) {
    worker.run();
  }
}

template <class MessageType>
Messenger::MessageTypeId Messenger::getMessageTypeId() {
  return Messenger_MessageTypeId<MessageType>::opaque();
}

inline bool Messenger::isThreaded() const {
  return threaded_;
}

inline Messenger::Worker& Messenger::getWorker(size_t index) {
  return isThreaded() ? workers_[index] : workers_[0];
}
