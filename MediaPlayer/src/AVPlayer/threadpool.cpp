#include "threadpool.h"
#include <string.h>
#include <QDebug>


int ThreadPool::m_maxThreads;

int ThreadPool::m_freeThreads;

int ThreadPool::m_initFlag=-1;

int ThreadPool::m_pushIndex=0;

int ThreadPool::m_readIndex=0;

int ThreadPool::m_size=0;

std::vector<ThreadPool::Threads> ThreadPool::m_threadsQueue;

int ThreadPool::m_maxTasks;

std::vector<ThreadPool::Task> ThreadPool::m_tasksQueue;

std::mutex ThreadPool::m_mutex;

std::condition_variable ThreadPool::m_cond;

bool ThreadPool::init(int threadsNum,int tasksNum)
{
    if(m_initFlag!=-1)
        return true;
    if(threadsNum<=0||tasksNum<=0)
        return false;
    m_maxThreads=threadsNum;
    m_maxTasks=tasksNum;
    m_freeThreads=m_maxThreads;
    m_threadsQueue.resize(m_maxThreads);
    m_tasksQueue.resize(m_maxTasks);
    for(int i=0;i<m_maxThreads;i++) {
        m_threadsQueue[i].isWorking=false;
        m_threadsQueue[i].isTerminate=false;
        std::thread* _thread= new std::thread(threadEventLoop,&m_threadsQueue[i]);
        if(!_thread) {
            return false;
        }
        m_threadsQueue[i].id=_thread->get_id();
        _thread->detach();
    }
    m_initFlag=1;
    return true;
}

bool ThreadPool::addTask(std::function<void(std::shared_ptr<void>)> func,std::shared_ptr<void> par)
{
    std::unique_lock<std::mutex> lock(m_mutex);
    if(m_size>=m_maxTasks)
        return false;
    m_tasksQueue.at(m_pushIndex).func=func;
    m_tasksQueue.at(m_pushIndex).par=par;
    m_size++;
    m_pushIndex=(m_pushIndex+1)%m_maxTasks;
    //qDebug()<<"free threads:"<<m_freeThreads;
    m_cond.notify_one();
}

void ThreadPool::threadEventLoop(void* arg)
{
    auto theThread=reinterpret_cast<Threads*>(arg);
    while(true) {
        std::unique_lock<std::mutex> lock(m_mutex);
        while(!m_size) {
            if(theThread->isTerminate)
                break;
            m_cond.wait(lock);
        }
        if(theThread->isTerminate)
            break;
        Task task = m_tasksQueue[m_readIndex];
        m_tasksQueue[m_readIndex].func=nullptr;
        m_tasksQueue[m_readIndex].par.reset();
        m_readIndex=(m_readIndex+1)%m_maxTasks;
        m_size--;
        m_freeThreads--;
        lock.unlock();
        theThread->isWorking=true;
        //执行任务函数
        task.func(task.par);
        theThread->isWorking=false;
        lock.lock();
        m_freeThreads++;
    }
}

void ThreadPool::releasePool()
{
    std::unique_lock<std::mutex> lock(m_mutex);
    for(int i=0;i<m_maxThreads;i++) {
        m_threadsQueue[i].isTerminate=true;
    }
    m_cond.notify_all();
    lock.unlock();
    //等待线程全部退出
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
}

