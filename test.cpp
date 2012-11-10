#include <iostream>
#include <atomic>
#include <mutex>
#include <thread>
#include <vector>
#include <algorithm>
#include <cassert>
#include <condition_variable>

#include "other.h"

// Well, do not set the thread count exceeds your physical core number
const int g_thread_count = MAX_THREAD_COUNT;
const int g_hammer_count = 1000000;

/**
 * I hope everyone come inside, and everyone know
 * how many had been here
 */
int wait_for_all_entering_ep()
{
    static std::atomic<int> in_count(0);
    static bool is_door1_open( true );
    static bool is_door2_open( false );

    static int result = 0;

    while ( !is_door1_open );

    if ( in_count++ == 0 )
    {
        while ( in_count != get_expected_thread_count() );
        result = in_count.load();
        is_door1_open = false;
        is_door2_open = true;
    }
    else
    {
        while ( !is_door2_open );
    }

    if ( --in_count == 0 )
    {
        is_door2_open = false;
        is_door1_open = true;
    }

    return result;
}

void thread_func()
{
    int hammer_count = g_hammer_count;
    while (hammer_count--)
    {
        notify_each_run();

        int t_inside = wait_for_all_entering_ep();
        assert(t_inside == g_thread_count && "Well, you code could not suffer hammering!");
        if (hammer_count % 10000 == 0)
            std::cout << "Passed: " << hammer_count << std::endl;
    }
}

int main()
{
    std::vector<std::thread*> threads;

    initialize();

    for (int i = 0; i < g_thread_count; i++)
    {
        threads.push_back(new std::thread(thread_func));
    }

    std::for_each(threads.begin(), threads.end(), [](std::thread* t) {
            if (t->joinable())
                t->join();
    });

    uninitialize();

    std::cout << "Finished" << std::endl;
    return 0;
}
