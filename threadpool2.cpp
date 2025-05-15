#include <iostream>
#include <pthread.h>
#include <vector>
#include <queue>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <thread> 
#include <curl/curl.h>

using namespace std;
class DownLoad{
    public:
    DownLoad(const std::string &url,const std::string &savepath):url(url),savepath(savepath);
    {}

    void execute()
    {
    CURL* curl;
    FILE* fp;
    CURLcode res;

    curl_global_init(CURL_GLOBAL_DEFAULT);  // 初始化 libcurl

    curl = curl_easy_init();
    if(curl)
    {
        fp = fopen(savepath.c_str(),"wb");
         if (!fp) {
            std::cerr << "Failed to open file: " << save_path << std::endl;
            return;
        }

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);

        res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
    std::cerr << "Download failed: " << curl_easy_strerror(res) << std::endl;
    } else {
    std::cout << "Download finished: " << url << std::endl;
    }
        fclose(fp);
        curl_easy_cleanup(curl);

    }
    curl_global_cleanup();
}

    private:
    std::string url;
    std::string savepath;

    static size_t write_data(void* ptr, size_t size, size_t nmemb, FILE* stream) {
        size_t written = fwrite(ptr, size, nmemb, stream);
        return written;
}
}
class Threadpool
{
    public:
    //static mutex printMutex; 
    Threadpool(size_t p_num):stop(false),pendingTasks(0)
    {
        for(int i = 0;i < p_num;i++)
        {
            //pthread_t thread;
            workers.emplace_back(&Threadpool::workerFunction, this);
        }
    }
    ~Threadpool()
    {
        {
        unique_lock<std::mutex> lock(queueMutex);
        stop = true;
        condition.notify_all();
        }
        for(std::thread &worker : workers)
        {
            worker.join();
            //pthread_join(worker,nullptr);
        }
    }

    template <class F>
    void enqueue(F&& f)
    {
        unique_lock<std::mutex> lock(queueMutex);
        if(stop)
        {
            throw std::runtime_error("ThreadPool has been stopped");
        }
        ++pendingTasks;
        taskQueue.emplace(forward<F>(f));
    
        condition.notify_one();
    }

    void wait()
{
    unique_lock lock(queueMutex);
    condition.wait(lock, [this] {
        return taskQueue.empty() && pendingTasks == 0;
    });
}

    private:
    void workerFunction()
    {
        //Threadpool* pool = static_cast<Threadpool*>(arg);
        while(true)
        {
            function<void()> task;
            {
             unique_lock<mutex> lock(queueMutex);
             condition.wait(lock, [this] {
                return stop || !taskQueue.empty();
                });
                
                if (stop && taskQueue.empty())
                return;
                
                task = move(taskQueue.front());
                taskQueue.pop();

            }
            task();
            --pendingTasks;
            if (pendingTasks == 0 && taskQueue.empty())
            {
                condition.notify_all();
            }
        }
        //return;

    }
    private:
vector<thread> workers; // 工作线程
queue<function<void()>> taskQueue; // 任务队列
mutex queueMutex; // 保护任务队列的互斥锁
condition_variable condition; // 条件变量，用于线程同步
atomic<bool> stop; // 标志，表示是否停止线程池
// static mutex printMutex; 
atomic<int> pendingTasks; // 记录未完成的任务数量

};
mutex Threadpool::printMutex;

void downloadTask(int id, const string& url, const string& savepath) {
    DownLoad downloader(url, savepath);
    downloader.execute();
}
    int main()
    {
        int p_num = 11;
        Threadpool pool(p_num);
    
        for (int i = 1; i < p_num; i++)
        {
            pool.enqueue([i,downloadTasks])
            {
                 downloadTask(i, downloadTasks[i].first, downloadTasks[i].second);
            }
        }
    
        pool.wait();
        cout << "所有任务完成，程序退出。" << endl;
        return 0;
    }
