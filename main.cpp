#include "timer.h"
using namespace std::chrono_literals;

void timePrinter(const Timer &timer)
{
    while (true)
        if (timer.calculateTimeLeft() > 0 && timer.calculateTimeLeft() % 100 == 0)
        {
            timer.printTimeLeft();
            std::this_thread::sleep_for(10ms);
        }
}

int main()
{

    auto st = std::chrono::steady_clock::now();

    { // Scope needed for correct timer destruction
        Timer t;
        std::thread printerThread(timePrinter, std::ref(t));

        t.setTime(7);
        t.start();

        std::this_thread::sleep_for(1s);

        t.addTime(2);
        std::cout << "2s added" << std::endl;
        std::this_thread::sleep_for(1s);

        t.reduceTime(3);
        std::cout << "3s reduced" << std::endl;
        std::this_thread::sleep_for(1s);

        t.pause();
        std::this_thread::sleep_for(1s);

        t.start();
        std::this_thread::sleep_for(1.5s);
        t.reset();
        std::cout << "Reset" << std::endl;

        printerThread.detach();
    }
    auto elapsed = std::chrono::steady_clock::now() - st;
    std::cout << "waited for " << std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count() << " ms\n";
}