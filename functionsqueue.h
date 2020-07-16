#include <deque>
#include <functional>

class FunctionsQueue
{
public:
    FunctionsQueue(const std::function<bool()>& canProcessNextFunction) :
        m_canProcessNextFunction(canProcessNextFunction)
    {}
    
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
    
private:
    std::function<bool()> m_canProcessNextFunction;
    std::deque<std::function<void()>> m_queue;
};
