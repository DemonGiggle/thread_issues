#include <iostream>
#include <atomic>
#include <mutex>
#include <thread>
#include <vector>
#include <algorithm>
#include <cassert>

// Well, do not set the thread count exceeds your physical core number
const int g_thread_count = 2;
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
    static std::atomic<bool> closed_door;
    static std::atomic<bool> first_thread;

    while (closed_door);

    thread_inside_count++;
    bool first_thread_expect = false;
    if (std::atomic_compare_exchange_strong(&first_thread, &first_thread_expect, true))
    { 
        while (thread_inside_count != g_thread_count)
        {  
            // Well, furiosly infinit looping until all the threads runnin in the 
            // kernel are trapped here
        }
        closed_door = true;
        saved_thread_inside_count = std::max<int>(saved_thread_inside_count.load(), thread_inside_count.load());
        wait_finish = true;
    }
    else
    {
        while (wait_finish == false);
    }

    int saved = saved_thread_inside_count;
    int prev_count = thread_inside_count.fetch_sub(1);

    if (prev_count == 1)
    {
        // the last one should open the door and initialize
        // value for the next time
        first_thread = false;
        wait_finish = false;
        saved_thread_inside_count = 0;
        closed_door = false;
    }

    return saved;
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
