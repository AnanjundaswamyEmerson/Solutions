#ifndef CONVERSATION_FILE_OPERATIONS_H
#define CONVERSATION_FILE_OPERATIONS_H
#include "AgenticAIFileOperations.hpp"

class ConversationFileOperations: public AgenticAIFileOperations 
{
    public:
        int ai_file_open(const char* path, int flags) override;
        int ai_file_close(int fd) override;
        ssize_t ai_file_read(int fd, void* buffer, size_t size) override;
        ssize_t ai_file_write(int fd, const void* buffer, size_t size) override;
};
#endif // CONVERSATION_FILE_OPERATIONS_H