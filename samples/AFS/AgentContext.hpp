#ifndef AGENT_CONTEXT_H
#define AGENT_CONTEXT_H

#include <cstdlib>
#include <sys/types.h>
#include <cstring>
class AgentContext {
    public: 
        AgentContext() {
            agent_name = "DefaultAgent";
            agent_id = 0;
            agent_context = nullptr;
            state = Idle;
        }
        ~AgentContext() = default;

        enum AgentContextTypes {
            /// @brief Memory context for learning and decision-making processes of the agent.
            MemoryContext,
            /// @brief Conversation context for managing interactions and dialogues.
            ConversationContext,
            /// @brief Retrieval context for handling information retrieval tasks.
            RetrievalContext
        };

        enum AgentStates {
            Idle,
            Creating,
            Ready,
            Deleting,
            Deleted
        };

        private:
            const char* agent_name;
            int agent_id;
            void *agent_context;
            AgentStates state;
            AgentContextTypes context_type;
            const size_t context_size = 1024; // Example size for the agent context

    public: 
        void setState(AgentStates new_state) {
            state = new_state;
        }

        AgentStates getState() const {
            return state;
        }

        int prepareAgentContext() {
            agent_context = malloc(context_size); // Allocate memory for the agent context
            if (agent_context == nullptr) {
                return -1; // Error handling for memory allocation failure
            }
            memset(agent_context, 1, context_size);
            return 0; // Success
        }

        size_t contextSize() const {
            return context_size;
        }

        void *getContextData() const {
            return agent_context;
        }
};
#endif // AGENT_CONTEXT_H