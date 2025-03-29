//
// Created by jakubszwedowicz on 3/24/25.
//

#ifndef PUBLISHSUBSCRIBEHANDLERS_H
#define PUBLISHSUBSCRIBEHANDLERS_H

#include <utility>
#include <vector>

namespace PacMan {
namespace Utils {

// Forward declarations
template <class Event> class IPublisher;
template <class Event> class ISubscriber;

// Declarations
template <class Event> class ISubscriber {
public:
  virtual ~ISubscriber();
  virtual void callback(const Event& event) = 0;
  virtual void unsubscribe();

private:
  friend class IPublisher<Event>;
  void selfSubscribe(IPublisher<Event> *publisher);

private:
  IPublisher<Event> *m_publisher;
};

template <class Event> class IPublisher {
public:
  virtual ~IPublisher() = default;

  bool subscribe(ISubscriber<Event> *subscriber);
  void unsubscribe(ISubscriber<Event> *subscriber);

protected:
  virtual void publish(const Event &event);

private:
  std::vector<ISubscriber<Event> *> m_subscribers;
};

// Implementations
template <class Event> ISubscriber<Event>::~ISubscriber() { unsubscribe(); }

template <class Event>
void ISubscriber<Event>::selfSubscribe(IPublisher<Event> *publisher) {
  if (m_publisher)
    m_publisher->unsubscribe();
  publisher->subscribe(this);
}

template <class Event> void ISubscriber<Event>::unsubscribe() {
  if (m_publisher) {
    auto pub = std::exchange(m_publisher, nullptr);
    pub->unsubscribe(this);
  }
}

template <class Event>
bool IPublisher<Event>::subscribe(ISubscriber<Event> *subscriber) {
  if (subscriber == nullptr)
    return false;
  subscriber->selfSubscribe(this);
  return true;
}

template <class Event>
void IPublisher<Event>::unsubscribe(ISubscriber<Event> *subscriber) {
  subscriber->unsubscribe(this);
  m_subscribers.erase(subscriber);
}

template <class Event> void IPublisher<Event>::publish(const Event &event) {
  for (auto &subscriber : m_subscribers) {
    subscriber->callback(event);
  }
}

} // namespace Utils
} // namespace PacMan

#endif // PUBLISHSUBSCRIBEHANDLERS_H
