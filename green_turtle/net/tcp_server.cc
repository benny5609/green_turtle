#include <assert.h>
#include "tcp_server.h"
#include "event_loop.h"
#include "tcp_acceptor.h"
#include "tcp_client.h"
#include "timer.h"
#include "timer_queue.h"
#include <system.h>

using namespace green_turtle;
using namespace green_turtle::net;

TcpServer::TcpServer(int expected_size) :
    timer_queue_(new TimerQueue(2048,16))
    ,is_terminal_(false)
    ,thread_count_(1)
    ,expected_size_(expected_size)
{
  System::UpdateTime();
  timer_queue_->Update(System::GetMilliSeconds());
}

TcpServer::~TcpServer()
{
  for(auto thrd : this->threads_)
  {
    delete thrd;
  }
  for(auto loop : this->loops_)
  {
    delete loop;
  }
}

void TcpServer::AddAcceptor(TcpAcceptor *acceptor)
{
  this->handlers_.push_back(acceptor);
  acceptor->set_events(kEventReadable);
}

void TcpServer::AddClient(TcpClient *client)
{
  this->handlers_.push_back(client);
  client->set_events(kEventReadable | kEventWriteable);
}

void TcpServer::SetThreadCount(int count)
{
  assert(count >= 1);
  this->thread_count_ = count;
}

void TcpServer::Terminal()
{
  is_terminal_ = true;
  for(auto loop : this->loops_)
  {
    loop->Ternimal();
  }
}

void TcpServer::ScheduleTimer(Timer *timer_ptr,uint64_t timer_interval,int64_t time_delay)
{
  this->timer_queue_->ScheduleTimer(timer_ptr, timer_interval, time_delay);
}

void TcpServer::CancelTimer(Timer *timer_ptr)
{
  this->timer_queue_->CancelTimer(timer_ptr);
}

void TcpServer::InitEventLoop()
{
  int event_handler_size_per_thread = (expected_size_ + thread_count_ - 1)   / thread_count_;
  for(int idx = 0; idx < thread_count_; ++idx)
  {
    EventLoop *loop = new EventLoop(event_handler_size_per_thread);
    assert(loop);
    loop->SetLoopIndex(idx);
    this->loops_.push_back(loop);
  }

  auto loop_begin = std::begin(this->loops_);
  for(auto handler : this->handlers_)
  {
    EventLoop *loop = *loop_begin;
    loop->AddEventHandler(handler);
    handler->loop_balance(this->loops_);

    loop_begin++;
    if(loop_begin == std::end(this->loops_))
      loop_begin = std::begin(this->loops_);
  }
  this->handlers_.clear();
}

static void Loop(EventLoop *loop)
{
  loop->Loop();
}

void TcpServer::InitThreads()
{
  for(int i = 0; i < thread_count_; ++i)
  {
    std::thread *thrd = new std::thread(Loop, this->loops_[i]);
    this->threads_.push_back(thrd);
  }
}

void TcpServer::Run()
{
  InitEventLoop();
  InitThreads();
  while(!is_terminal_)
  {
    System::UpdateTime();
    size_t message_begin = System::GetMilliSeconds();

    LoopOnce();

    System::UpdateTime();
    size_t timer_begin = System::GetMilliSeconds();
    this->timer_queue_->Update(timer_begin);

    System::UpdateTime();
    size_t process_end = System::GetMilliSeconds();
    //time cost
    size_t cost = process_end - message_begin;
    if(cost < 20)
    {
      System::Yield(20 - cost);
    }
  }
  for(auto& thrd : this->threads_)
  {
    thrd->join();
  }
}
