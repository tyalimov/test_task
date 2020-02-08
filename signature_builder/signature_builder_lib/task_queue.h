#pragma once

#include "utils.h"

#include <queue>
#include <mutex>

namespace builder::threading
{
    struct HashTask
    {
        utils::BinaryBlock block;

        void perform();
    };

    class TaskQueue
    {
    private:
        std::queue<HashTask>         m_queue;
        std::condition_variable  m_cv;
        std::mutex               m_mutex;
        bool                     m_no_more_push;
      
    public:

        using SizeType = std::queue<HashTask>::size_type;

        TaskQueue();

        bool tryPop(HashTask& task);
        void push(const HashTask& task);
        void stop();

        [[nodiscard]] bool stopped();
        [[nodiscard]] SizeType size() const;
    };
}
