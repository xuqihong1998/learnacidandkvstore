/*
Create by xqh on 2023/10/24
*/
#include CO_MUTWEX_H
#define CO_MUTWEX_H

#include <semaphore.h>
#include <pthread.h>
#include <atomic>
#include <memory>
#include <queue>
/*
RAII 包装互斥锁
*/
template<class T>
class RAIILock
{
public:
    RAIILock(T& mutex): m_mutex(mutex)
    {
        m_mutex.lock();
        m_locked = true;
    }
    ~RAIILock()
    {
        unlock();
    }
    void lock()
    {
        if(!m_locked)
        {
            m_mutex.lock();
            m_locked = true;
        }
    }
    bool tryLock()
    {
        if(!m_locked)
        {
            m_locked = mutex.tryLock();
        }
        return m_locked;
    }
    void unlock()
    {
        if(m_locked)
        {
            m_mutex.unlock();
            m_locked = false;
        }
    }
};
/*
自旋锁
*/
class SpinLock
{
public:
    using Lock = RAIILock<SpinLock>;
    SpinLock()
    {
        pthread_spin_init(&m_mutex,0);
    }
    ~SpinLock()
    {
        pthread_spin_destory(&m_mutex);
    }
    void lock()
    {
        pthread_spin_lock(&m_mutex);
    }
    void unlock()
    {
        pthread_spin_unlock(&m_mutex);
    }
private:
    pthread_spinlock_t m_mutex;
};

/*
互斥量
*/
class Mutex
{
public:
    using Lock = RAIILock<Mutex>;
    Mutex()
    {
        pthread_mutex_init(&m_mutex);
    }
    ~Mutex()
    {
        pthread_mutex_destroy(&m_mutex);
    }
    void lock()
    {
        pthread_mutex_lock(&m_mutex);
    }
    void unlock()
    {
        pthread_mutex_unlock(&m_mutex);
    }
    bool tryLock()
    {
        return !pthread_mutex_trylock(&m_mutex);
    }
private:
    pthread_mutex_t m_mutex;
};

/*
协程锁
*/
class CoMutex 
{
public:
    using Lock = RAIILock<CoMutex>;
    
    void lock();
    void unlock();
    bool tryLock();
private:
    //协程所持有的锁
    RAIILock m_mutex;
    //保护等待队列的锁
    RAIILock m_gaurd;
    //持有锁的协程id
    uint64_t m_fiberId = 0;
    //协程等待队列
    std::queue<std::shared_ptr<Fiber>> m_waitQueue;
};

/*
读锁RAII包装
*/
template<class T>
class ReadRAIILock
{
public:
    ReadRAIILock(T& mutex):m_mutex(mutex)
    {
        m_mutex.rlock();
        m_locked = true;
    }
    ~ReadRAIILock()
    {
        unlock();
    }
    void lock()
    {
        if(!m_locked)
        {
            m_mutex.rlock();
            m_locked = true;
        }
    }
    void unlock()
    {
        if(m_locked)
        {
            m_mutex.unlock();
            m_locked = false;
        }
    }
private:
    T& m_mutex;
    bool m_locked = false;
};
/*
写锁RAII包装
*/
template<class T>
class WriteRAIILock
{
public:
    WriteRAIILock(T& mutex):m_mutex(mutex)
    {
        m_mutex.wlock();
        m_locked = true;
    }
    ~WriteRAIILock()
    {
        unlock();
    }
    void lock()
    {
        if(!m_locked)
        {
            m_mutex.wlock();
            m_locked = true;
        }
    }
    void unlock()
    {
        if(m_locked)
        {
            m_mutex.unlock();
            m_locked = false;
        }
    }
private:
    T& m_mutex;
    bool m_locked = false;
};
class RWMutex
{
public:
    using ReadLock = ReadRAIILock<RWMutex>;
    using WriteLock = WriteRAIILock<RWMutex>;
    RWMutex()
    {
        pthread_rwlock_init(&m_rwmutex,nullptr);
    }
    ～RWMutex()
    {
        pthread_rwlock_destroy(&m_rwmutex);
    }
    void rlock()
    {
        pthread_rwlock_rdlock(&m_rwmutex);
    }
    void wlock()
    {
        pthread_rwlock_wrlock(&m_rwmutex);
    }
    void unlock()
    {
        pthread_rwlock_unlock(&m_rwmutex);
    }
private:
    pthread_rwlock_t m_rwmutex;
};

#endif