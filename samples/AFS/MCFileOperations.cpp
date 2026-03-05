#include <cerrno>
#include "AgentContext.hpp"
#include "MCFileOperations.hpp"

// Implementation of the MCFileOperations class that inherits from AgenticAIFileOperations

/// @brief Open file to store agent context in memory.
/// @param path Path of the context repository.
/// @param flags READ/WRITE flags for the file operation.
/// @return File descriptor on success, negative error code on failure.
int MCFileOperations::ai_file_open(const char* path, int flags)
{
    int fd = get_fd();
    if (fd < 0) {
        return -ENFILE; // Maximum number of open files reached
    }

    AgentContext* context = new AgentContext();
    context->setState(AgentContext::Creating);
    
    if (context->prepareAgentContext() != 0) {
        delete context;
        return -ENOMEM; // Could not allocate agent context
    }
    context->setState(AgentContext::Ready);
    context_inventory[fd] = context; // Store the context in the inventory
    return fd; // Success, return a file descriptor or handle as needed
}

/// @brief Close the context file.
/// @param fd File descriptor of the file to close.
/// @return 0 on success, negative error code on failure.
int MCFileOperations::ai_file_close(int fd)
{
    // Implementation for closing a file
    if (fd < 0 || fd >= MAXFILESOPEN) {
        return -EBADFD; // Invalid file descriptor
    }
    AgentContext* context = context_inventory[fd];
    if (context->getState() != AgentContext::Ready) {
        return -EBADFD; // File is not in a state that can be closed
    }

    delete context;
    context_inventory[fd] = nullptr; // Clear the context from the inventory
    return 0;
}

/// @brief Read data from the memory context file.
/// @param fd File descriptor of the file to read from.
/// @param buffer Buffer to store the read data.
/// @param size Size of the buffer.
/// @return Number of bytes read on success, negative error code on failure.
ssize_t MCFileOperations::ai_file_read(int fd, void* buffer, size_t size) 
{
    if (fd < 0 || fd >= MAXFILESOPEN) {
        return -EBADFD; // Invalid file descriptor
    }
    AgentContext* context = context_inventory[fd];
    if (context->getState() != AgentContext::Ready) {
        return ENODATA; // File is not in a state that can be read
    }
    if (size < context->contextSize()) {
        return -E2BIG; // Buffer size is too small for the context
    }
    memcpy(buffer, context->getContextData(), context->contextSize());
    return context->contextSize();
}

/// @brief Write data to the memory context file.
/// @param fd File descriptor of the file to write to.
/// @param buffer Buffer containing the data to write.
/// @param size Size of the data to write.
/// @return Number of bytes written on success, negative error code on failure.
ssize_t MCFileOperations::ai_file_write(int fd, const void* buffer, size_t size) {
    if (fd < 0 || fd >= MAXFILESOPEN) {
        return -EBADFD; // Invalid file descriptor
    }
    AgentContext* context = context_inventory[fd];
    if (context->getState() != AgentContext::Ready) {
        return ENODATA; // File is not in a state that can be written to
    }
    if (size > context->contextSize()) {
        return -EFBIG; // Data size exceeds the context size
    }
    memcpy(context->getContextData(), buffer, size);
    return size;
}