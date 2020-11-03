#ifndef FUNCTIONS_QUEUE_H
#define FUNCTIONS_QUEUE_H

#include <deque>
#include <functional>

namespace Oggy
{
    
class FunctionsQueue
{
public:
    FunctionsQueue(const std::function<bool()>& canProcessNextFunction) :
        m_canProcessNextFunction(canProcessNextFunction)
    {}

    FunctionsQueue(const FunctionsQueue& other) = delete;
    
    inline void doOrAppend(const std::function<void()>& fun) {
        m_canProcessNextFunction() ? fun() : m_queue.push_back(fun);
    }
    
    inline void processMaxFunctions() {
        while (!m_queue.empty()){
            m_queue.front()();
            m_queue.pop_front();
            if (!m_canProcessNextFunction()) break;
        }
    }

    inline void clear() {
        m_queue.clear();
    }

    inline auto canProcess() const { return m_canProcessNextFunction(); }
    
private:
    std::function<bool()> m_canProcessNextFunction;
    std::deque<std::function<void()>> m_queue;
};
    
}

#endif
