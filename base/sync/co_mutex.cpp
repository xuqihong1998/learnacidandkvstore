#include "co_mutex.h"

bool CoMutex::tryLock()
{
    return m_mutex.tryLock();
}

void CoMutex::lock()
{
    if(Fiber::GetFiberId() == m_fiberId)
    {
        return;
    }

    while
    //to do 
}