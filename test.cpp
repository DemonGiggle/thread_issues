#include <iostream>
#include <atomic>
#include <mutex>
#include <thread>
#include <vector>
#include <algorithm>
#include <cassert>

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
    static std::atomic<bool> is_ready(false);
    static std::atomic<int>  in_count(0);
    static std::atomic<int>  out_count(0);

    static int result_count = 0;

    while(out_count != 0);

    if(in_count++ == 0)
    {
        while(in_count != get_expected_thread_count());

        result_count = in_count;

        is_ready = true;
    }
    else
    {
        while(!is_ready);
    }

    if(in_count == ++out_count)
    {
        is_ready  = false;
        in_count  = 0;
        out_count = 0;
    }

    return result_count;
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
