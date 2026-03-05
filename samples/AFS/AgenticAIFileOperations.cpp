#include <cstring>
#include "AgenticAIFileOperations.hpp"

// Base class has only static members. Derived class will implement the virtual functions and manage the context inventory.
int AgenticAIFileOperations::next_fd{0};
int AgenticAIFileOperations::open_files[AgenticAIFileOperations::MAXFILESOPEN] = {0};
AgentContext* AgenticAIFileOperations::context_inventory[AgenticAIFileOperations::MAXFILESOPEN] = {nullptr};