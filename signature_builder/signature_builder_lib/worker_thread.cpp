#include "worker_thread.h"

namespace builder::threading
{
    WorkerThread::WorkerThread
    (
        const std::shared_ptr<std::mutex>&               file_iterator_mutex,
        const std::shared_ptr<filesys::FileIterator>&    file_iterator,
        const std::shared_ptr<std::vector<std::string>>& hashes
    )
        : m_file_iterator_mutex(file_iterator_mutex)
        , m_file_iterator(file_iterator)
        , m_hashes(hashes)
    {
    }

    void WorkerThread::run()
    {
        // TODO: implement
        // Считать N блоков (также запомнить инфу о количестве считанного)
        // Посчитать хеши
        // Записать в таблицу
        // Повторить
    }

    void WorkerThreadFunction
    (
        const std::shared_ptr<std::mutex>&               file_iterator_mutex,
        const std::shared_ptr<filesys::FileIterator>&    file_iterator,
        const std::shared_ptr<std::vector<std::string>>& hashes
    )
    {
        WorkerThread{ file_iterator_mutex, file_iterator, hashes }.run();
    }
}
