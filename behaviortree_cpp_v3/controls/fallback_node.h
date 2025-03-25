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
 * @brief The FallbackNode is used to try different strategies,
 * until one succeeds.
 * If any child returns RUNNING, previous children will NOT be ticked again.
 *
 * - If all the children return FAILURE, this node returns FAILURE.
 *
 * - If a child returns RUNNING, this node returns RUNNING.
 *
 * - If a child returns SUCCESS, stop the loop and return SUCCESS.
 *
 * 
 * 
 * The provided code defines the `FallbackNode` class for a behavior tree in C++. This class is another type of **control node** in the behavior tree framework. The `FallbackNode` executes its children in order but stops as soon as one of its children succeeds. If a child fails, it moves on to the next one.

### **Code Breakdown and Explanation:**

1. **Include Directives (Lines 1-2)**
   ```cpp
   #include "behaviortree_cpp/controls/fallback_node.h"
   #include "behaviortree_cpp/action_node.h"
   ```
   - These lines include the necessary header files for the `FallbackNode` class and the `action_node.h` header, which might be used by child nodes.

2. **Namespace Declaration (Lines 16-17)**
   ```cpp
   namespace BT
   {
   ```
   - This begins the `BT` (Behavior Tree) namespace, under which the behavior tree framework's classes, including `FallbackNode`, are defined.

3. **Constructor (`FallbackNode::FallbackNode`) (Lines 19-23)**
   ```cpp
   FallbackNode::FallbackNode(const std::string& name)
       : ControlNode::ControlNode(name, {} )
       , current_child_idx_(0)
   {
       setRegistrationID("Fallback");
   }
   ```
   - The constructor initializes a `FallbackNode` object with a given `name`.
   - It calls the constructor of the `ControlNode` base class, passing the `name` and an empty list `{}`.
     - The `ControlNode` base class is responsible for managing child nodes for control flow.
   - The member variable `current_child_idx_` is initialized to 0, meaning the fallback node starts from the first child.
   - The `setRegistrationID("Fallback")` call assigns a unique registration ID for the node type.

4. **`tick()` Method (Lines 26-69)**
   ```cpp
   NodeStatus FallbackNode::tick()
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
               case NodeStatus::SUCCESS:
               {
                   haltChildren(0);
                   current_child_idx_ = 0;
                   return child_status;
               }
               case NodeStatus::FAILURE:
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

       // The entire while loop completed. This means that all the children returned FAILURE.
       if (current_child_idx_ == children_count)
       {
           haltChildren(0);
           current_child_idx_ = 0;
       }

       return NodeStatus::FAILURE;
   }
   ```
   - **Purpose**: The `tick()` method is responsible for executing the fallback node's behavior.
   - The node sets its own status to `RUNNING` to indicate that it is actively processing.
   
   - **While Loop**:
     - The loop iterates through each child node in the `children_nodes_` vector.
     - Inside the loop:
       - The current child node is selected with `children_nodes_[current_child_idx_]`.
       - The child node's `executeTick()` method is called to get the child's current status.
       - The status of the child node is handled using a `switch` statement:
         - **`RUNNING`**: If the child node is still running, the fallback node returns `RUNNING` and will be called again in the next tick to continue waiting for the child node to finish.
         - **`SUCCESS`**: If a child node succeeds, the sequence is considered successful, the node halts all child nodes using `haltChildren(0)`, resets the `current_child_idx_`, and returns `SUCCESS`.
         - **`FAILURE`**: If the child node fails, the fallback node moves to the next child by incrementing `current_child_idx_`.
         - **`IDLE`**: An error is thrown because no child node should return `IDLE` during active execution.
   
   - **Post Loop**:
     - After all child nodes have been processed, if all child nodes returned `FAILURE`, the fallback node will return `FAILURE`.
     - It halts all child nodes, resets the index, and returns `FAILURE`.

5. **`halt()` Method (Lines 72-75)**
   ```cpp
   void FallbackNode::halt()
   {
       current_child_idx_ = 0;
       ControlNode::halt();
   }
   ```
   - This method is called to halt or reset the `FallbackNode`.
   - It resets the `current_child_idx_` to 0, effectively restarting the fallback node's child node execution from the first child.
   - It also calls the base class `halt()` method to halt any child nodes as needed.

6. **End of Namespace (Line 78)**
   ```cpp
   }
   ```
   - This ends the `BT` namespace.

---

### **Key Concepts in the Code:**

- **Control Node**: The `FallbackNode` is derived from a **ControlNode**, which is used for nodes that manage the flow of execution across multiple child nodes. Control nodes typically decide the order of execution and handle how the child nodes interact with each other.

- **`NodeStatus` Enum**: This enumeration defines the possible states for a node:
  - `RUNNING`: The node is actively processing.
  - `SUCCESS`: The node has successfully completed its task.
  - `FAILURE`: The node has failed in its task.
  - `IDLE`: The node is not actively processing but is not in a failure or success state.

- **Fallback Node Behavior**:
  - A fallback node will try to execute each child node in order, returning `SUCCESS` if any child node succeeds. If a child node fails, it moves on to the next one.
  - The node will return `FAILURE` only when all child nodes have failed.
  - The fallback node will return `RUNNING` if any child node is still executing (i.e., still in the `RUNNING` state).

- **Halting and Resetting**: The `halt()` method ensures that the fallback node and its child nodes can be reset if needed. This is useful for when the behavior tree needs to stop the current behavior and potentially reattempt the sequence or fallback.

### **Flow of Execution**:
1. The `tick()` method is called on the `FallbackNode`.
2. The node starts processing child nodes from the beginning (index 0).
3. If a child node is still running, the `RUNNING` status is returned.
4. If a child node succeeds, the fallback node immediately stops and returns `SUCCESS`.
5. If a child node fails, the fallback node continues with the next child node.
6. If all children fail, the node returns `FAILURE`.




 */
class FallbackNode : public ControlNode
{
public:
  FallbackNode(const std::string& name);

  virtual ~FallbackNode() override = default;

  virtual void halt() override;

private:
  size_t current_child_idx_;

  virtual BT::NodeStatus tick() override;
};

}   // namespace BT
