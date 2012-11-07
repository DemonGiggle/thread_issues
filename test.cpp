#include <iostream>
#include <atomic>
#include <mutex>
#include <thread>

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
        while (thread_inside_count != 2)
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
    std::cout << "thread_function: " << std::this_thread::get_id() << std::endl;

    while (true)
    {
        int t_inside = wait_for_all_entering_ep();

        if (t_inside != 2)
        {
            std::cout << "Error detected! : " << t_inside << std::endl;
        }
    }
}

int main()
{
    try 
    {
        std::thread t1(thread_func);
        std::thread t2(thread_func);

        t1.join();
        t2.join();
    }
    catch (std::runtime_error& e)
    {
        std::cout << "Error: " << e.what() << std::endl;
    }

    return 0;
}
