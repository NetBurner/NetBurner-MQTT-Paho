#pragma once

#include <HiResTimer.h>

class NBMQTTCountdown
{
public:
	NBMQTTCountdown()
    {
		init();
    }

	NBMQTTCountdown(int ms)
    {
		init();
		countdown_ms(ms);
    }

    ~NBMQTTCountdown() {
    	timer->releaseTimer();
    }

    bool expired()
    {
    	return (left_ms() == 0);
    }


    void countdown_ms(int ms)
    {
    	startTime = static_cast<int>((TimeTick/TICKS_PER_SECOND) * 1000); // convert to ms
    	delay = ms;
    }


    void countdown(int seconds)
    {
    	countdown_ms(seconds * 1000);
    }


    int left_ms()
    {
    	int time = static_cast<int>((TimeTick/TICKS_PER_SECOND) * 1000); // convert to ms
    	return ((time-startTime) > delay) ? 0 : delay - (time-startTime);
    }

private:

    void init()
    {
        timer = HiResTimer::getHiResTimer();
    	delay = 0;
        if(timer->readTime() == 0) { // Has background timer been started?
        	startTime = 0;
        	timer->init();
        	timer->start();

        }
    }


    HiResTimer *timer;
    int delay;
    int startTime;
};
