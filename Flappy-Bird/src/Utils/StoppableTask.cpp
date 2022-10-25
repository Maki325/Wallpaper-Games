#include "pch.h"
#include "StoppableTask.h"

namespace WallpaperAPI {

  StoppableTask::StoppableTask() :
    m_futureObj(m_exitSignal.get_future()) {}

  StoppableTask::StoppableTask(StoppableTask&& obj) noexcept : m_exitSignal(std::move(obj.m_exitSignal)), m_futureObj(std::move(obj.m_futureObj)) {}

  StoppableTask& StoppableTask::operator=(StoppableTask&& obj) noexcept
  {
    std::cout << "Move Assignment is called" << std::endl;
    m_exitSignal = std::move(obj.m_exitSignal);
    m_futureObj = std::move(obj.m_futureObj);
    return *this;
  }

  bool StoppableTask::StopRequested()
  {
    return m_futureObj.wait_for(std::chrono::milliseconds(0)) != std::future_status::timeout;
  }

  void StoppableTask::Stop()
  {
    m_exitSignal.set_value(true);
  }

  void StoppableTask::Run(StoppableTask& task)
  {
    task.Run();
  }
}
