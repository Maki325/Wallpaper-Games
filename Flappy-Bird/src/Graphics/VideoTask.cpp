#include "pch.h"
#include "VideoTask.h"
#include "../Core/Application.h"
#include "../Core/Event.h"

#include "opencv2/core.hpp"
#include "opencv2/video.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/videoio.hpp"

namespace WallpaperAPI
{
  VideoTask::VideoTask(std::string&& videoPath) : m_videoPath(videoPath), m_seconds(-1) {}
  VideoTask::VideoTask(std::string&& videoPath, double seconds)
    : m_videoPath(videoPath), m_seconds(seconds) {}

  void VideoTask::Run()
  {
    // https://gist.github.com/itsrifat/66b253db2736b23f247c
    cv::VideoCapture cap(m_videoPath);
    if (!cap.isOpened()) {
      std::cout << "Couldn't open video \"" << m_videoPath << "\"!" << std::endl;
      Application::PostEvent({ Event::Type::EVENT_TYPE_QUIT, 0 });
      return;
    }

    double fps = cap.get(cv::VideoCaptureProperties::CAP_PROP_FPS);

    int width = cap.get(cv::VideoCaptureProperties::CAP_PROP_FRAME_WIDTH),
      height = cap.get(cv::VideoCaptureProperties::CAP_PROP_FRAME_HEIGHT);

    cv::Mat frame;
    for (
      int frameNum = 0;
      frameNum < cap.get(cv::VideoCaptureProperties::CAP_PROP_FRAME_COUNT)
        && !StopRequested();
      frameNum++
    ) {
      cap >> frame;
      VideoFrame* videoFrame = new VideoFrame();
      videoFrame->width = width;
      videoFrame->height = height;
      videoFrame->data = new unsigned char[width * height * frame.channels()];

      cv::flip(frame, frame, 0);
      if (frame.isContinuous()) {
        memcpy(videoFrame->data, frame.data, width * height * frame.channels());
      }
      else {
        for (int i = 0; i < height; i++)
        {
          memcpy(&videoFrame->data[i * width * frame.channels()], &(frame.data[i * frame.step]), width * frame.channels());
        }
      }

      Application::PostEvent({Event::Type::EVENT_TYPE_RENDER, videoFrame });

      std::this_thread::sleep_for(std::chrono::milliseconds((int)(1000 / fps)));

      if (frameNum == 0) {
        // Application::PostEvent({ Event::Type::EVENT_TYPE_QUIT, 0 });
        // cv::imshow("Test", frame);
        // cv::waitKey();
        // break;
      }
      if (m_seconds != -1 && m_seconds <= frameNum / fps) {
        break;
      }
    }

    Application::PostEvent({ Event::Type::EVENT_TYPE_QUIT, 0 });
  }
}
