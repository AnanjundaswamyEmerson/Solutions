#include "AgenticAIFileOperations.hpp"
#include "MCFileOperations.hpp"
#include <iostream>
#include <algorithm>

int main() {
    // Driver code to demonstrate the usage of MCFileOperations
    MCFileOperations mcFileOps;
    const char* path = "agent_context.dat"; // Path for the context repository
    int flags = 0; // Example flags, adjust as needed
    int fd = mcFileOps.ai_file_open(path, flags);
    if (fd < 0) {
        std::cerr << "Failed to open file: " << strerror(-fd) << std::endl;
        return 1;
    }
    std::cout << "File opened successfully with file descriptor: " << fd << std::endl;

    // Example buffer for reading
    char buffer[1024];
    ssize_t bytesRead = mcFileOps.ai_file_read(fd, buffer, sizeof(buffer));
    if (bytesRead < 0) {
        std::cerr << "Failed to read from file: " << strerror(-bytesRead) << std::endl;
        mcFileOps.ai_file_close(fd);
        return 1;
    }
    std::cout << "Successfully read " << bytesRead << " bytes from file." << std::endl;

    // Modify and write context
    std::fill(buffer, buffer + bytesRead, 2); // Example modification, adjust as needed
    if (mcFileOps.ai_file_write(fd, buffer, bytesRead) < 0) {
        std::cerr << "Failed to write to file: " << strerror(-bytesRead) << std::endl;
        mcFileOps.ai_file_close(fd);
        return 1;
    }

    // Read back the modified context and verify
    bytesRead = mcFileOps.ai_file_read(fd, buffer, sizeof(buffer));
    if (bytesRead < 0) {
        std::cerr << "Failed to read from file: " << strerror(-bytesRead) << std::endl;
        mcFileOps.ai_file_close(fd);
        return 1;
    }

    bool ok = std::all_of(buffer, buffer + bytesRead, [](char c) { return c == 2; });
    if (ok) {
        std::cout << "Context modified and verified successfully." << std::endl;
    } else {
        std::cerr << "Context verification failed." << std::endl;
    }
    mcFileOps.ai_file_close(fd);
}