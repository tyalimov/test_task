﻿#pragma once

#include <mutex>
#include <atomic>
#include <tuple>

#include "file_mapper.h"
#include "barrier.h"

#ifdef _DEBUG
#   define LOG(msg)                                                       \
    {                                                                     \
        std::lock_guard<std::mutex> lock(m_console_mutex);                \
        std::cout << "Thread [" << std::this_thread::get_id() << "] - ";  \
        std::cout << msg << std::endl;                                    \
    }
#else
#   define LOG(msg)
#endif

namespace builder::threading
{
    class Worker
    {
        
    private:

        Barrier&                 m_barrier;
        uint64_t&                m_current_limit;
        uint64_t&                m_current_block_id;
        filesys::FileMapper&     m_input_mapper;
        filesys::FileMapper&     m_output_mapper;

        std::mutex& m_block_id_mutex;
        std::mutex& m_console_mutex;

        // TODO: bool --> enum
        [[nodiscard]] std::tuple<bool, uint64_t> getNextBlockId() const noexcept;

    public:

        // TODO: Проверить все конструкторы на консистентность
        Worker
        (
            Barrier&                 barrier,
            uint64_t&                current_limit,
            uint64_t&                current_block_id,
            filesys::FileMapper&     input_mapper,
            filesys::FileMapper&     output_mapper,
            std::mutex&              block_id_mutex,
            std::mutex&              console_mutex
        );

        void run();
    };
}
