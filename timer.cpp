#include "timer.h"
using namespace std::chrono_literals;

void Timer::start()
{
    if (_isPaused)
    {
        _isPaused = false;
        _timerRunningCondition.notify_one();
        return;
    }

    if (_timeLeft.load() == 0ms)
    {
        std::cout << "Set time first" << std::endl;
        return;
    }

    _worker = std::make_unique<std::thread>(&Timer::runTimer, this);
}

Timer::~Timer()
{
    if (_worker && _worker->joinable())
        _worker->join();
}

std::chrono::milliseconds Timer::calculateChronoTimeLeft() const
{
    auto n = std::chrono::steady_clock::now();

    if (n >= _endTime.load())
        return 0ms;
    return std::chrono::duration_cast<std::chrono::milliseconds>(_endTime.load() - n);
}

unsigned Timer::calculateTimeLeft() const
{
    return calculateChronoTimeLeft().count();
}

void Timer::setTime(unsigned t)
{
    if (calculateTimeLeft() > 0 && !_isPaused)
        std::cout << "Already set" << std::endl;
    _timeLeft = std::chrono::seconds(t);
}

void Timer::pause()
{
    if (_isPaused)
    {
        std::cout << "Already paused" << std::endl;
        return;
    }
    _isPaused = true;
    _timerRunningCondition.notify_one();
}

void Timer::reset()
{
    _endTime = std::chrono::steady_clock::now();
    _timerRunningCondition.notify_one();
}

void Timer::runTimer()
{
    _endTime = std::chrono::steady_clock::now() + _timeLeft.load();

    std::unique_lock<std::mutex> lock(_mtx);

    while (std::chrono::steady_clock::now() < _endTime.load())
    {
        // Wait for:
        // 1. Time goes off
        // 2. Pause
        // 3. If user reduces time to <= 0 (clock::now() > _endTime)
        // 4. User resets timer
        _timerRunningCondition.wait_until(lock, _endTime.load());
        if (isPaused())
        {
            auto pauseStartTime = std::chrono::steady_clock::now();
            _timeLeft = calculateChronoTimeLeft();
            _timerRunningCondition.wait(lock);
            auto pauseEndTime = std::chrono::steady_clock::now();

            _endTime = _endTime.load() + (pauseEndTime - pauseStartTime);
        }
    }
}

bool Timer::isPaused() const
{
    return _isPaused.load();
}

void Timer::printTimeLeft() const
{
    auto tl = isPaused() ? _timeLeft.load().count() : calculateTimeLeft();
    if (isPaused())
        std::cout << "Paused. ";
    std::cout << "Time Left: "<< tl / 1000 << "." << (tl / 100) % 10 << "s" << std::endl;
}

void Timer::addTime(unsigned t)
{
    _endTime = _endTime.load() + std::chrono::seconds(t);
}

void Timer::reduceTime(unsigned t)
{
    _endTime = _endTime.load() - std::chrono::seconds(t);
    _timerRunningCondition.notify_one();
}