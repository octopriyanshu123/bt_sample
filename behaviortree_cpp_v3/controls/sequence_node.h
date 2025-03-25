/* Copyright (C) 2015-2018 Michele Colledanchise -  All Rights Reserved
 * Copyright (C) 2018-2020 Davide Faconti, Eurecat -  All Rights Reserved
*
*   Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"),
*   to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
*   and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
*   The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*
*   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
*   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
*   WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#pragma once

#include "behaviortree_cpp_v3/control_node.h"

namespace BT
{
/**
 * @brief The SequenceNode is used to tick children in an ordered sequence.
 * If any child returns RUNNING, previous children will NOT be ticked again.
 *
 * - If all the children return SUCCESS, this node returns SUCCESS.
 *
 * - If a child returns RUNNING, this node returns RUNNING.
 *   Loop is NOT restarted, the same running child will be ticked again.
 *
 * - If a child returns FAILURE, stop the loop and return FAILURE.
 *   Restart the loop only if (reset_on_failure == true)
 * 
 * 
 * The provided code defines the `SequenceNode` class for a behavior tree in C++. This class represents a **control node** that is used to manage a sequence of child nodes. The `SequenceNode` executes its child nodes in order, and the node as a whole returns `SUCCESS` only if all child nodes return `SUCCESS`. If any child node returns `FAILURE`, the sequence stops and returns `FAILURE`. The `RUNNING` state indicates that a child node is still in progress.

Here’s an anatomy and detailed explanation of each part of the code:

### **Code Breakdown and Explanation:**

1. **Include Directives (Lines 1-2)**
   ```cpp
   #include "behaviortree_cpp/controls/sequence_node.h"
   #include "behaviortree_cpp/action_node.h"
   ```
   - These lines include necessary header files. 
   - The `sequence_node.h` file defines the `SequenceNode` class, while the `action_node.h` file is included because the `SequenceNode` may work with action nodes or other types of child nodes.

2. **Namespace Declaration (Lines 17-18)**
   ```cpp
   namespace BT
   {
   ```
   - This begins the definition of the `BT` (Behavior Tree) namespace. All the code for the behavior tree framework will be encapsulated within this namespace.

3. **Constructor (`SequenceNode::SequenceNode`) (Lines 21-26)**
   ```cpp
   SequenceNode::SequenceNode(const std::string& name)
       : ControlNode::ControlNode(name, {} )
       , current_child_idx_(0)
   {
       setRegistrationID("Sequence");
   }
   ```
   - The constructor initializes a `SequenceNode` with a given `name`.
   - It calls the constructor of the base class `ControlNode`, passing the `name` and an empty list `{}` as parameters.
     - The `ControlNode` is assumed to be a base class for nodes that can have child nodes (like `SequenceNode`).
   - The member `current_child_idx_` is initialized to 0, indicating the sequence will start from the first child node.
   - The method `setRegistrationID("Sequence")` assigns an ID to the node for registration purposes.

4. **`halt()` Method (Lines 28-32)**
   ```cpp
   void SequenceNode::halt()
   {
       current_child_idx_ = 0;
       ControlNode::halt();
   }
   ```
   - This method is called to halt or reset the `SequenceNode`.
   - It resets the `current_child_idx_` to 0, effectively restarting the sequence.
   - It also calls the base class `halt()` method (likely to halt any child nodes).

5. **`tick()` Method (Lines 34-77)**
   ```cpp
   NodeStatus SequenceNode::tick()
   {
       const size_t children_count = children_nodes_.size();

       setStatus(NodeStatus::RUNNING);

       while (current_child_idx_ < children_count)
       {
           TreeNode* current_child_node = children_nodes_[current_child_idx_];
           const NodeStatus child_status = current_child_node->executeTick();

           switch (child_status)
           {
               case NodeStatus::RUNNING:
               {
                   return child_status;
               }
               case NodeStatus::FAILURE:
               {
                   // Reset on failure
                   haltChildren(0);
                   current_child_idx_ = 0;
                   return child_status;
               }
               case NodeStatus::SUCCESS:
               {
                   current_child_idx_++;
               }
               break;

               case NodeStatus::IDLE:
               {
                   throw LogicError("A child node must never return IDLE");
               }
           }
       }

       // The entire while loop completed. This means that all the children returned SUCCESS.
       if (current_child_idx_ == children_count)
       {
           haltChildren(0);
           current_child_idx_ = 0;
       }
       return NodeStatus::SUCCESS;
   }
   ```
   - **Purpose**: This method is responsible for executing the sequence of child nodes one by one.
   - It sets the node status to `RUNNING` because the sequence node is in progress.
   
   - **While Loop**:
     - The loop continues as long as `current_child_idx_` is less than the number of child nodes.
     - Inside the loop:
       - It retrieves the current child node using `children_nodes_[current_child_idx_]`.
       - It executes the current child node's `executeTick()` method, which returns the child node's status (`NodeStatus`).
       - The status of the child node is handled in a `switch` statement:
         - **`RUNNING`**: If the child node is still running, the sequence node will return this status and wait for the child node to complete. This will cause the `tick()` method to be called again in the next frame.
         - **`FAILURE`**: If any child node returns `FAILURE`, the sequence halts, resets the child nodes using `haltChildren(0)`, and the sequence will reset its index (`current_child_idx_ = 0`). The failure status is returned.
         - **`SUCCESS`**: If a child node returns `SUCCESS`, the sequence moves on to the next child by incrementing `current_child_idx_`.
         - **`IDLE`**: If any child node returns `IDLE`, an exception is thrown because a child node should not be in the `IDLE` state while being ticked.
   
   - **Post Loop**: If all children have returned `SUCCESS`, the sequence has completed successfully. The child nodes are halted, the index is reset, and the node returns `SUCCESS`.

6. **End of Namespace (Line 80)**
   ```cpp
   }
   ```
   - This ends the `BT` namespace.

---

### **Key Concepts in the Code:**

- **Control Node**: The `SequenceNode` is a type of **ControlNode**. Control nodes are responsible for managing the execution flow of other nodes (child nodes). In this case, the `SequenceNode` manages a sequence of child nodes and defines the order in which they are executed.
  
- **`NodeStatus` Enum**: The status of the node is crucial in behavior trees. The possible statuses are:
  - `RUNNING`: The node is actively executing.
  - `SUCCESS`: The node has completed its task successfully.
  - `FAILURE`: The node has failed to complete its task.
  - `IDLE`: The node is not performing any actions and should never return this during active execution.

- **Halting and Resetting**: The `halt()` method is used to reset the state of the node, and `haltChildren()` ensures that all child nodes are halted properly if needed.

- **Sequential Execution**: The `SequenceNode` ensures that all its child nodes are executed sequentially, returning `SUCCESS` only if all child nodes succeed, and returning `FAILURE` as soon as one child fails.

### **Flow of Execution**:
1. The `tick()` method is called on the `SequenceNode`.
2. It starts executing its child nodes in sequence.
3. If a child node returns `RUNNING`, it immediately returns that status and will resume in the next tick.
4. If a child node fails (`FAILURE`), the sequence halts, resets, and returns `FAILURE`.
5. If all children succeed, the sequence returns `SUCCESS`.




 *
 */
class SequenceNode : public ControlNode
{
public:
  SequenceNode(const std::string& name);

  virtual ~SequenceNode() override = default;

  virtual void halt() override;

private:
  size_t current_child_idx_;

  virtual BT::NodeStatus tick() override;
};

}   // namespace BT
