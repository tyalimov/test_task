#pragma once

#include <mutex>
#include <atomic>

#include "file_mapper.h"

#ifdef _DEBUG
#   define LOG(msg) { std::lock_guard<std::mutex> lock(m_console_mutex); std::cout << msg << std::endl; }
#else
#   define LOG(msg)
#endif

namespace builder::threading
{
    class Worker
    {
        
    private:
        std::mutex&           m_console_mutex;
        std::atomic_uint64_t& m_current_block_id;
        filesys::FileMapper&  m_input_mapper;
        filesys::FileMapper&  m_output_mapper;

    public:

        // TODO: Проверить все конструкторы на консистентность
        Worker
        (
            std::mutex&           console_mutex,
            std::atomic_uint64_t& current_block_id,
            filesys::FileMapper&  input_mapper,
            filesys::FileMapper&  output_mapper
        );

        void run();
    };
}
