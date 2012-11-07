#define _GLIBCXX_USE_SCHED_YIELD

#include "other.h"
#include <thread>

std::thread* g_managed = NULL;
bool g_finish = false;
int expected_thread_count;

int get_expected_thread_count()
{
    return expected_thread_count;
}

void notify_each_run()
{
    expected_thread_count = MAX_THREAD_COUNT * 2;
}

void manage_func()
{
    while (!g_finish)
    {
        if (expected_thread_count != MAX_THREAD_COUNT)
        {
            expected_thread_count--;
        }
        
        std::this_thread::yield();
    }
}

void initialize()
{
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
