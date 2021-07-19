#include <thread>
#include <vector>
#include <atomic>
#include <mutex>
#include <functional>

#include "threadpool.h"

C_thread_pool::C_thread_pool(unsigned int numThreads) : num_threads(numThreads)
{
    terminate = false;
    for (int i = 0; i < this->num_threads; i++) {
		thread_pool.push_back(std::thread(&C_thread_pool::thread_cycle, this));
	}
}

void C_thread_pool::add_job(std::function<void()> job)
{
	{
        std::unique_lock<std::mutex> lock(this->q_lock);

        this->cur_jobs++;
	    this->queue.push_back(job);
	}
    this->cv.notify_one();
}

int C_thread_pool::current_jobs()
{
    return this->cur_jobs.load();
}

void C_thread_pool::shutdown()
{
    {
        std::unique_lock<std::mutex> lock(this->q_lock);
        this->terminate = true;
    }
    cv.notify_all();

	for (auto &thread : this->thread_pool) {
		thread.join();
	}

    thread_pool.clear();

    this->stopped = true;
}

void C_thread_pool::thread_cycle()
{
    while(1) {
        std::function<void()> job;
        {
            std::unique_lock<std::mutex> lock(this->q_lock);
            this->cv.wait(lock, [this]{ return !queue.empty() || terminate.load(); });

            if(this->terminate) {
                break;
            }

            job = this->queue.back();
            this->cur_jobs--;
            this->queue.pop_back();
        }
        job();
    }
}

C_thread_pool::~C_thread_pool()
{
    if(!this->stopped) {
        this->shutdown();
    }
}