Inspired by *https://arxiv.org/pdf/2512.05470*, this is a sample AI file system implementation 
to highlight a few concepts in object oriented programming using C++. 

*AgenticAIFileOperations* provides the expected file operations for every agent type to implement.
*MCFileOperations* and *ConversationFileOperations" provides concrete file operations. 
*AgentContext* maintains the agent state machine and context.
*main* is a test driver for *MCFileOperations*.

Possible extensions:
- Implement all the agentic types with true capabilities.
- AgentContext can be abstracted to remove specific agent type (currently MCFileOperations).
- Add unit tests for individual agent operations.