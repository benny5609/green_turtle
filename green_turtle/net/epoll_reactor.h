#ifndef EPOLL_REACTOR_
#define EPOLL_REACTOR_

#include <vector>
#include "noncopyable.h"

struct epoll_event;
class EventHandler;

class Reactor : Noncopyable
{
  public:
    Reactor();
    virtual ~Reactor();
    bool CreateFd();
    void DestroyFd();
  private:
    Reactor(const Reactor &);
    Reactor &operator=(const Reactor &);
  public:
    bool RegisterEventHandler(EventHandler *h);
    bool UnregisterEventHandler(EventHandler *h);
    // timeout
    void RunOnce(int timeout);
  private:
    int epfd_;
    std::vector<epoll_event> events_;
};

#endif

