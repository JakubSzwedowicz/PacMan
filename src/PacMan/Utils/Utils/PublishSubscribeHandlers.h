//
// Created by jakubszwedowicz on 3/24/25.
//

#ifndef PUBLISHSUBSCRIBEHANDLERS_H
#define PUBLISHSUBSCRIBEHANDLERS_H

#include <algorithm>
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
  virtual void callback(const Event &event) = 0;
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

/**
 * @brief This function removes a subscriber from the ones receiving events from
 * this publisher. It's quite slow however at O(N) time complexity
 * @param subscriber Object that wants to subscribe for events published by this
 * publisher
 */
template <class Event>
void IPublisher<Event>::unsubscribe(ISubscriber<Event> *subscriber) {
  auto it = std::find(m_subscribers.begin(), m_subscribers.end(), subscriber);
  if (it != m_subscribers.end()) {
    subscriber->unsubscribe();
    m_subscribers.erase(it);
  }
}

template <class Event> void IPublisher<Event>::publish(const Event &event) {
  for (auto &subscriber : m_subscribers) {
    subscriber->callback(event);
  }
}

} // namespace Utils
} // namespace PacMan

#endif // PUBLISHSUBSCRIBEHANDLERS_H
