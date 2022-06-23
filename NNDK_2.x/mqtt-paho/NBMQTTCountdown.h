#pragma once

#include <ucos.h>

class NBMQTTCountdown
{
private:
    DWORD doneTick;
    DWORD startTick;


public:
	NBMQTTCountdown()
    {
       doneTick = 0;
       startTick = 0;
    }

	NBMQTTCountdown(int ms)
    {
		countdown_ms(ms);
    }

    ~NBMQTTCountdown() {
    }

    bool expired()
    {
    	return (TimeTick>=doneTick);
    }

    void countdown_ms(int ms)
    {
    	startTick = TimeTick;
        DWORD ms_per_Tick = ( 1000 / TICKS_PER_SECOND );
        DWORD interval = ( ms / ms_per_Tick );
        if( interval < 1 ){ interval = 1; }
        doneTick = startTick + interval;
    }

    void countdown(int seconds)
    {
    	countdown_ms(seconds * 1000);
    }

    int left_ms()
    {
        uint32_t now = TimeTick;
        if( now >= doneTick ){ return 0; }
        return (( doneTick - now ) * ( 1000 / TICKS_PER_SECOND ));
    }

private:
    void init()
    {
        doneTick = 0;
        startTick = 0;
    }
};
