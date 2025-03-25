In Behavior Trees (BT), **decorators** are special types of nodes that modify the behavior or outcome of their child nodes. They are used to change or control how the child node behaves based on certain conditions, without altering the child node itself. Decorators typically wrap a node and allow for additional logic, such as preconditions, limiting the number of times a node can run, or reversing its result.

Decorators can be thought of as control flow mechanisms that influence the execution of a behavior tree. They don't typically perform tasks themselves but instead modify the behavior of their child nodes.

### Common Types of Decorators in Behavior Trees

Here are some of the most common types of decorators and how they work:

#### 1. **Successive (Success) Decorator**
   - **Purpose**: Always forces the child node to return a **`SUCCESS`** status, no matter what the child node’s outcome is.
   - **Behavior**: This decorator modifies the status returned by the child node, overriding it to always be `SUCCESS`.
   - **Example Use Case**: You might use this to ensure that a certain action always succeeds, even if the child node fails.
   
   **Example:**
   ```cpp
   // If child fails, still returns success.
   SuccessDecorator(SUCCESS) -> Child Node (FAILURE)
   ```

#### 2. **Failure Decorator**
   - **Purpose**: Always forces the child node to return a **`FAILURE`** status, regardless of the child node's result.
   - **Behavior**: This decorator modifies the status returned by the child node, overriding it to always be `FAILURE`.
   - **Example Use Case**: This is used when you want to ensure that a certain condition always results in failure, no matter the child node’s behavior.
   
   **Example:**
   ```cpp
   // Regardless of the child node result, force failure.
   FailureDecorator(FAILURE) -> Child Node (SUCCESS)
   ```

#### 3. **Invert Decorator**
   - **Purpose**: Inverts the status of the child node.
   - **Behavior**: If the child node returns `SUCCESS`, the decorator will make it return `FAILURE`, and if the child node returns `FAILURE`, it will make it return `SUCCESS`. `RUNNING` status remains unchanged.
   - **Example Use Case**: This can be used when you want to reverse the outcome of a child node’s result.
   
   **Example:**
   ```cpp
   // Inverts the result of the child node.
   InvertDecorator(SUCCESS) -> Child Node (FAILURE)
   ```

#### 4. **Repeat Decorator**
   - **Purpose**: Repeats the execution of a child node multiple times.
   - **Behavior**: The decorator forces the child node to keep executing until a certain condition is met, like a fixed number of repetitions or a specific result.
   - **Example Use Case**: Repeating an action until success or failure or until a certain number of iterations have been completed.
   
   **Example:**
   ```cpp
   // Repeat the child node until success or failure.
   RepeatDecorator(Repeat until success) -> Child Node
   ```

#### 5. **Limit Decorator**
   - **Purpose**: Limits the number of times a child node can be executed.
   - **Behavior**: This decorator allows you to set a maximum number of times the child node will execute. After reaching the maximum count, the decorator stops the child node.
   - **Example Use Case**: Useful when you want a node to be tried only a certain number of times, such as retrying an action up to three times before failing.

   **Example:**
   ```cpp
   // Limit how many times a child node can execute.
   LimitDecorator(Max 3 Attempts) -> Child Node
   ```

#### 6. **Wait Decorator**
   - **Purpose**: Introduces a delay or waits for a condition before allowing the child node to proceed.
   - **Behavior**: This can be used to delay the execution of a child node, ensuring that some condition is met before continuing with the behavior.
   - **Example Use Case**: This is used in scenarios like waiting for a certain period before trying an action again.

   **Example:**
   ```cpp
   // Wait for a fixed period of time before continuing.
   WaitDecorator(Wait 5 seconds) -> Child Node
   ```

### How Decorators Work in Behavior Trees

- **Structure**: In a behavior tree, decorators are typically leaf nodes that only have one child node. They can be attached to a single node (usually a leaf or action node) and modify its behavior.
- **Execution**: When a behavior tree is ticked, the decorator will first execute its own logic and then pass control to its child node. The decorator then manipulates the result of the child node's execution according to its specific behavior (success, failure, inversion, etc.).

### Example of Using Decorators in a Behavior Tree

Here is a simple example of a behavior tree where a decorator modifies the behavior of an action node:

```cpp
#include "behaviortree_cpp/behavior_tree.h"

using namespace BT;

// Define an action node that returns success or failure
class ActionNode : public SyncActionNode
{
public:
    ActionNode(const std::string& name) : SyncActionNode(name) {}
    
    NodeStatus tick() override
    {
        std::cout << "ActionNode ticked!" << std::endl;
        return NodeStatus::FAILURE; // Simulating failure
    }
};

// Define a decorator node
class SuccessDecorator : public DecoratorNode
{
public:
    SuccessDecorator(const std::string& name, Node* child) : DecoratorNode(name, child) {}

    NodeStatus tick() override
    {
        NodeStatus status = child_->tick();  // Execute child node
        if (status == NodeStatus::FAILURE)
        {
            std::cout << "Child failed, but SuccessDecorator forces SUCCESS!" << std::endl;
            return NodeStatus::SUCCESS;  // Force success
        }
        return status;  // Otherwise, return the child's status
    }
};

int main()
{
    // Constructing a behavior tree
    ActionNode* action = new ActionNode("Action");
    SuccessDecorator* decorator = new SuccessDecorator("SuccessDecorator", action);

    // Create behavior tree and tick it
    TreeNode* tree = decorator;
    tree->tick();  // This will print "ActionNode ticked!" and "Child failed, but SuccessDecorator forces SUCCESS!"
    
    return 0;
}
```

### Summary

- **Decorators** in behavior trees are used to modify or control the execution of child nodes.
- They are typically **leaf nodes** that wrap other nodes and add logic to **invert results**, **force success or failure**, **limit repetitions**, **delay execution**, and more.
- They are extremely useful for adding extra layers of logic to behavior trees without modifying the child nodes directly.

Decorators provide flexibility, allowing you to control the flow and behavior of your BT without adding complexity to the action or control nodes themselves.