#include <thread>
#include <vector>
#include <atomic>
#include <mutex>
#include <functional>

class C_thread_pool
{
public:
    C_thread_pool(unsigned int numThreads);
    ~C_thread_pool();
    void add_job(std::function<void()> job);
    int current_jobs();
    void shutdown();
    
    const unsigned int num_threads;
private:
    void thread_cycle();

    std::atomic<bool> terminate;
    std::condition_variable cv;
    std::vector<std::thread> thread_pool;
    std::mutex q_lock;
    std::vector<std::function<void()>> queue;
    std::atomic<int> cur_jobs;
    bool stopped = false;
};