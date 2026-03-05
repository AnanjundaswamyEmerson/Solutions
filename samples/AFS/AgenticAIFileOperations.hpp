#ifndef AGENTIC_AI_FILE_OPERATIONS_H
#define AGENTIC_AI_FILE_OPERATIONS_H
#include <cstddef>
#include <sys/types.h>
#include <atomic>
#include "AgentContext.hpp"
#include <cstring>

class AgenticAIFileOperations {
    public:
        virtual int ai_file_open(const char* path, int flags) = 0;
        virtual int ai_file_close(int fd) = 0;
        virtual ssize_t ai_file_read(int fd, void* buffer, size_t size) = 0;
        virtual ssize_t ai_file_write(int fd, const void* buffer, size_t size) = 0;
 
    protected:
        static const int MAXFILESOPEN = 10; // Maximum number of open files
        static int next_fd; // Not thread safe, but sufficient for this example
        static int open_files[MAXFILESOPEN]; // Array to track open file descriptors
        static AgentContext *context_inventory[MAXFILESOPEN]; // Array to hold agent contexts for each open file
    
    static int get_fd()
    {
        int start = next_fd;
        for (int i = 0; i < MAXFILESOPEN; i++)
        {
            int fd = (start + i) % MAXFILESOPEN;
            if (context_inventory[fd] == nullptr)
            {
                next_fd = (fd + 1) % MAXFILESOPEN;
                return fd;
            }
        }
        return -1;  // all slots full
    }

};
#endif // AGENTIC_AI_FILE_OPERATIONS_H