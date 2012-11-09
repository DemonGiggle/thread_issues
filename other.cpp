#define _GLIBCXX_USE_SCHED_YIELD

#include "other.h"
#include <thread>
#include <mutex>
#include <time.h>
#include <stdlib.h>

std::thread* g_managed = NULL;
bool g_finish = false;
int expected_thread_count;
std::mutex g_mtx;

int get_expected_thread_count()
{
    return expected_thread_count;
}

void notify_each_run()
{
    // sometimes, we just random to set the thread count to the expected value
    // to increase the possibility to somewhat racing condition
    if (g_mtx.try_lock())
    {
        if (random() % 88 > 44)
        {
            expected_thread_count = MAX_THREAD_COUNT * 2;
        }
        else
        {
            expected_thread_count = MAX_THREAD_COUNT;
        }
        g_mtx.unlock();
    }
}

void manage_func()
{
    while (!g_finish)
    {
        if (g_mtx.try_lock())
        {
            if (expected_thread_count != MAX_THREAD_COUNT)
            {
                expected_thread_count--;
            }
            g_mtx.unlock();
        }
        
        std::this_thread::yield();
    }
}

void initialize()
{
    srand(time(NULL));
    g_finish = false;
    g_managed = new std::thread(manage_func);
}

void uninitialize()
{
    if (g_managed && g_managed->joinable())
    {
        g_finish = true;

        g_managed->join();
        delete g_managed;
    }
}
