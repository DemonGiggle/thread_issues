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
    static std::atomic<bool> wait_finish;
    static std::atomic<int> thread_inside_count;
    static std::atomic<int> saved_thread_inside_count;

    static std::mutex mtx;

    wait_finish = false;
    thread_inside_count++;
    if (mtx.try_lock())
    {  
        while (thread_inside_count != get_expected_thread_count())
        {  
            // Well, furiosly infinit looping until all the threads runnin in the 
            // kernel are trapped here
        }
        saved_thread_inside_count = thread_inside_count.load();
        wait_finish = true;
        mtx.unlock();
    }
    else
    {
        while (wait_finish == false);
    }

    thread_inside_count--;
    return saved_thread_inside_count.load();
}

void thread_func()
{
    int hammer_count = g_hammer_count;
    while (hammer_count--)
    {
        int t_inside = wait_for_all_entering_ep();
        assert(t_inside == g_thread_count && "Well, you code could not suffer hammering!");
        if (hammer_count % 10000 == 0)
            std::cout << "Passed: " << hammer_count << std::endl;
    }
}

int main()
{
    std::vector<std::thread*> threads;

    for (int i = 0; i < g_thread_count; i++)
    {
        threads.push_back(new std::thread(thread_func));
    }

    std::for_each(threads.begin(), threads.end(), [](std::thread* t) {
            if (t->joinable())
                t->join();
    });

    std::cout << "Finished" << std::endl;
    return 0;
}
