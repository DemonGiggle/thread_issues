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
    static std::atomic<int> num_room_1(0);
    static std::atomic<int> num_room_2(0);
    int num_thread = 0;

    ++num_room_1;

    while(num_room_1 != get_expected_thread_count());
    num_thread = num_room_1;

    ++num_room_2;
    while(num_room_2 != num_thread);

    --num_room_1;
    while(num_room_1 != 0);

    --num_room_2;
    while(num_room_2 != 0);

    return num_thread;
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
