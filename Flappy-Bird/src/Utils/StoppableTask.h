#pragma once

namespace WallpaperAPI
{
  class StoppableTask
  { // https://thispointer.com/c11-how-to-stop-or-terminate-a-thread/
  public:
    StoppableTask();
    StoppableTask(StoppableTask&& obj) noexcept;
    StoppableTask& operator=(StoppableTask&& obj) noexcept;
    virtual void Run() = 0;
    void operator()()
    {
      Run();
    }
    void Stop();

    static void Run(StoppableTask& task);

  protected:
    bool StopRequested();

  private:

    std::promise<bool> m_exitSignal;
    std::future<bool> m_futureObj;
  };
}
