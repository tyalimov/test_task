#pragma once

#include "utils.h"

#include <queue>
#include <mutex>

// TODO: Везде проверить ссылки

namespace builder::threading
{
    struct Task
    {
        utils::BinaryBlock block;

        explicit Task(const utils::BinaryBlock& b)
            : block(b)
        {}

        Task() = default;

        void perform();
    };

    class TaskQueue
    {
    private:
        std::queue<Task>         m_queue;
        std::condition_variable  m_cv;
        std::mutex               m_mutex;
        bool                     m_no_more_push;
      
    public:

        using SizeType = std::queue<Task>::size_type;

        TaskQueue();

        bool tryPop(Task& task);
        bool tryPop(std::vector<Task>& tasks, uint64_t count);
        void push(const Task& task);
        void push(const std::vector<Task>& tasks);
        void stop();

        [[nodiscard]] bool stopped();
        [[nodiscard]] SizeType size() const;
    };
}
