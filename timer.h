#include <memory>
#include <atomic>
#include <iostream>
#include <chrono>
#include <thread>
#include <ctime>

class Timer
{
public:
    ~Timer();

    void setTime(unsigned t);
    void start();
    void pause();
    void reset();

    void addTime(unsigned t);
    void reduceTime(unsigned t);

    bool isPaused() const;
    void printTimeLeft() const;
    unsigned calculateTimeLeft() const; // in ms

private:
    void runTimer();
    std::chrono::milliseconds calculateChronoTimeLeft() const;

private:
    std::atomic<std::chrono::steady_clock::time_point> _endTime = std::chrono::steady_clock::now();
    std::atomic<bool> _isPaused = false;

    // Needed only for knowing correct time before start and while paused
    std::atomic<std::chrono::milliseconds> _timeLeft;

    std::unique_ptr<std::thread> _worker = nullptr;

    std::condition_variable _timerRunningCondition;
    std::mutex _mtx;
};
