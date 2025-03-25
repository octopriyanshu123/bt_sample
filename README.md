# BT_ros1

Behavior Tree example for ROS 1

# Support ROS version

* melodic
* noetic

# Build up environment

1. git clone the repo.
```
mkdir -p ~/bt_ros1_ws/src
cd ~/bt_ros1_ws/src
git clone https://github.com/Adlink-ROS/BT_ros1.git
```

2. Install dependencies
```
cd ~/bt_ros1_ws
rosdep install --from-paths src --ignore-src -r -y
```

3. Build
```
catkin_make
# If you want to build with OpenVINO support
catkin_make --cmake-args -DBUILD_OPENVINO=ON
```

# Usage

We will run Gazebo with NeuronBot2 and show a simple BT example.
NeuronBot2 supports [melodic](https://github.com/Adlink-ROS/neuronbot2/tree/melodic-devel) & [noetic](https://github.com/Adlink-ROS/neuronbot2/tree/noetic-devel).
Choose the version based on your ROS 1 environment.

The BT example (refer to [bt_nav_mememan_interrupt.xml](bt_xml/bt_nav_mememan_interrupt.xml)) will make NeuronBot2 move between Goal_a and Goal_b.
If receiving `/interrupt_event`, which is `gohome`, then NeuronBot2 will move to Goal_c.

* Open 1st terminal and run mememan world. (ROS 1 environment)
```
source ~/neuronbot2_ros1_ws/devel/setup.bash
export GAZEBO_MODEL_PATH=~/neuronbot2_ros1_ws/src/neuronbot2/neuronbot2_gazebo/models
roslaunch neuronbot2_gazebo neuronbot2_world.launch world_model:=mememan_world.model
```
* Open 2nd terminal and run navigation. (ROS 1 environment)
```
source ~/neuronbot2_ros1_ws/devel/setup.bash
roslaunch neuronbot2_nav bringup.launch map_name:=$HOME/neuronbot2_ros1_ws/src/neuronbot2/neuronbot2_nav/maps/mememan.yaml open_rviz:=true
```
* Open 3rd termainal and run BT. (ROS 1 environment) 
```
source ~/bt_ros1_ws/devel/setup.bash
roslaunch bt_sample bt_sample.launch
```
* Open 4th terminal and pub interrupt event. (ROS 1 environment)
```
rostopic pub /interrupt_event std_msgs/String "gohome"
```

# Note
If you want to get the coordinate for navigation, you can run navigation and listen to the topic /goal_pose or open rviz to monitor tf.

The position and orientation should be put into BT file and the orientation value we use here is Quaternion.

Record X, Y in positoin and Z, W in orientation.
![](readme_resource/inspect_rviz.png)

Open xml file in bt_xml and modify robot checkpoint with " X ; Y ; Z ; W " format in SetBlackboard.
![](readme_resource/bt_modify.png)

If you only have Euler Angle and want to transfer to Quaternion, please refer to online tools, such as https://quaternions.online/



Action
AlwaysFailure
AlwaysSuccess
Script
SetBlackBoard
Sleep

Here are notes on the specific nodes used in a behavior tree:

### 1. **Action**
   - **Definition**: An action node is where actual behavior or tasks are performed. It represents a concrete action that the AI character or agent takes. For instance, it could involve moving, attacking, interacting, or any action in the game.
   - **Usage**: Executes a specific task and returns either success or failure. Commonly used in tasks like picking up an item, patrolling an area, etc.
   - **Result**: 
     - **Success**: The action completed successfully.
     - **Failure**: The action failed, possibly due to a precondition not being met.

### 2. **AlwaysFailure**
   - **Definition**: This node always returns a **Failure** result, no matter what. It's a specialized node that is used to artificially cause a failure in the tree. It has no effect other than returning failure.
   - **Usage**: Useful in scenarios where you need to fail a branch of the tree immediately, regardless of other conditions.
   - **Result**: Always returns **Failure**.

### 3. **AlwaysSuccess**
   - **Definition**: This node always returns a **Success** result, regardless of the conditions or inputs.
   - **Usage**: Used to artificially ensure that a branch succeeds, often to simplify testing or ensure that part of the tree completes successfully even if other conditions are not met.
   - **Result**: Always returns **Success**.

### 4. **Script**
   - **Definition**: A script node allows the insertion of custom code or scripts to be executed as part of the behavior tree. It can be used to execute complex logic or call external functions that are not easily represented in the standard behavior tree nodes.
   - **Usage**: Useful for cases where complex or unique behavior is needed that cannot be easily encapsulated in the standard nodes.
   - **Result**: The outcome of a script node depends on the script logic; it could return **Success**, **Failure**, or any custom result defined in the script.

### 5. **SetBlackBoard**
   - **Definition**: The SetBlackBoard node is used to update or modify the values in the **Blackboard**, a shared memory space where the behavior tree stores data (such as positions, states, or other relevant information).
   - **Usage**: Typically used to set or update variables that are being shared across different parts of the behavior tree. For example, setting the target position or the health status of an entity.
   - **Result**: This node doesn't return success or failure per se; instead, it simply updates the blackboard values.

### 6. **Sleep**
   - **Definition**: The Sleep node pauses the behavior tree for a specified amount of time. During this time, no other actions or decisions are made until the sleep duration has passed.
   - **Usage**: It is commonly used to introduce delays between actions, such as waiting before the next task or behavior can occur (e.g., pausing between movements or actions).
   - **Result**: The node typically returns **Success** after the sleep time has elapsed, indicating the pause is complete and the tree can continue.

These nodes form the foundation of many behavior trees and are used to create decision-making processes in AI characters, game agents, or robots.

Condition
ScriptCondition

Here are the notes on the **Condition** and **ScriptCondition** nodes in a behavior tree:

### 1. **Condition**
   - **Definition**: A **Condition** node evaluates a specific condition and returns either **Success** or **Failure** based on whether that condition is true or false.
   - **Usage**: It is typically used to check whether certain requirements or states are met before proceeding with further actions in the behavior tree. This can involve checking variables, health, proximity to an object, or other criteria.
   - **Example**: 
     - Is the AI's health above a certain threshold?
     - Is the player in range for an attack?
     - Has the AI reached the destination?
   - **Result**: 
     - **Success**: The condition is met (true).
     - **Failure**: The condition is not met (false).

### 2. **ScriptCondition**
   - **Definition**: A **ScriptCondition** node allows you to insert custom scripts to evaluate a condition. Instead of using a predefined set of conditions (like health checks or proximity checks), this node runs a script that returns either **Success** or **Failure** based on custom logic.
   - **Usage**: Used when the built-in conditions are not sufficient, and you need to define your own condition logic. For example, you might want to check if a complex condition holds, like if an AI is in a "stealth" state or if certain events have occurred that require a more advanced check.
   - **Example**: 
     - Checking if an AI has successfully completed a series of tasks (like finding a key and unlocking a door).
     - Custom logic to check whether the AI should change its behavior based on environmental factors.
   - **Result**: The result of the script determines the outcome:
     - **Success**: The condition defined in the script is true.
     - **Failure**: The condition defined in the script is false.

### Summary
- **Condition** nodes are used for simple, predefined checks that return **Success** or **Failure** based on the evaluation of a condition.
- **ScriptCondition** nodes are used for more complex, custom checks where a script is executed to return **Success** or **Failure** based on custom logic.

Both types of nodes are essential for ensuring that an AI's behavior tree can respond to dynamic, real-time conditions in a game or simulation.

Control
AsyncFallback
AsyncSequence
ReactiveFallback
ReactiveSequence
Sequence
SequenceWithMemory
Fallback

Here are the notes on the **Control** nodes in a behavior tree:

### 1. **AsyncFallback**
   - **Definition**: An **AsyncFallback** node is a type of control node that behaves similarly to a regular fallback (also known as a **Selector**), but it is designed to handle asynchronous tasks.
   - **Usage**: It attempts each child node in sequence, and if a child node fails, it continues trying the next child. However, it also allows the tree to run tasks asynchronously, meaning the node doesn't block the entire tree while waiting for a child task to complete. This is useful in situations where actions take time, such as waiting for a network request, animation, or pathfinding to finish.
   - **Result**: 
     - **Success**: If any of the children nodes return **Success**.
     - **Failure**: If all children return **Failure**.
   
### 2. **AsyncSequence**
   - **Definition**: Similar to **AsyncFallback**, the **AsyncSequence** node is a control node that handles asynchronous execution, but it requires that all child nodes succeed in order for it to succeed, much like a regular sequence node.
   - **Usage**: The **AsyncSequence** executes each child node in order, but allows the tree to process other tasks while waiting for any child nodes that may take time (like waiting for an AI to finish an animation or an event to complete). If all child nodes succeed, the **AsyncSequence** returns **Success**. If any child node fails, it returns **Failure**.
   - **Result**: 
     - **Success**: If all children return **Success**.
     - **Failure**: If any child returns **Failure**.

### 3. **ReactiveFallback**
   - **Definition**: A **ReactiveFallback** node is similar to the **Fallback** node but responds reactively based on the status of the children. It continuously reevaluates the children when necessary, reacting to changes dynamically.
   - **Usage**: This node can be used when you want the tree to react to changes in real-time. For example, if the AI detects a new threat, it will choose the appropriate child node based on the current situation, continuing to check its conditions and respond accordingly.
   - **Result**: 
     - **Success**: If any child node returns **Success**.
     - **Failure**: If all children return **Failure**.

### 4. **ReactiveSequence**
   - **Definition**: The **ReactiveSequence** node is similar to the **Sequence** node but reacts to changes in the environment or context dynamically. It reevaluates its children nodes based on changes and conditions.
   - **Usage**: This node is useful in situations where you want to ensure that all conditions are met sequentially but need to react to changes in the environment. For example, an AI might start a sequence of tasks but adjust its actions based on real-time updates (e.g., react to an unexpected event in the environment).
   - **Result**: 
     - **Success**: If all child nodes return **Success** in order.
     - **Failure**: If any child node returns **Failure**.

### 5. **Sequence**
   - **Definition**: A **Sequence** node is a control node that executes its children in order. It returns **Success** only if all child nodes succeed. If any child node fails, it immediately returns **Failure** and does not continue to the remaining children.
   - **Usage**: Commonly used when you need a set of tasks to occur in a specific order. For example, an AI might follow a sequence where it checks if an enemy is in range, aims, and then fires.
   - **Result**: 
     - **Success**: If all child nodes return **Success**.
     - **Failure**: If any child node returns **Failure**.

### 6. **SequenceWithMemory**
   - **Definition**: A **SequenceWithMemory** node is a variation of the **Sequence** node that "remembers" the result of each child node after it has been evaluated. This means that if a child node fails, the sequence won't reattempt it in the future unless it explicitly gets reset or updated.
   - **Usage**: Useful when you want the tree to continue working without re-evaluating certain conditions or tasks that have already been checked. For example, once an AI has checked a condition (like whether a door is locked), it won't recheck it every time the sequence is re-entered.
   - **Result**: 
     - **Success**: If all child nodes return **Success** (considering memory).
     - **Failure**: If any child node returns **Failure** (considering memory).

### 7. **Fallback (Selector)**
   - **Definition**: A **Fallback** node (also known as a **Selector**) is a control node that evaluates its child nodes in order and returns **Success** as soon as one of its children succeeds. If all children fail, it returns **Failure**.
   - **Usage**: This node is typically used for decision-making where the AI has multiple options, and it should choose the first one that works. For instance, if an AI character is trying to find a way to move forward and encounters an obstacle, it might first try to walk, then jump, and then finally climb.
   - **Result**: 
     - **Success**: If any child node returns **Success**.
     - **Failure**: If all child nodes return **Failure**.

---

### Summary

- **AsyncFallback**: Sequential fallback with asynchronous execution.
- **AsyncSequence**: Sequential behavior with asynchronous execution.
- **ReactiveFallback**: A fallback node that reacts to changes dynamically.
- **ReactiveSequence**: A sequence node that reacts to environmental changes.
- **Sequence**: A control node that runs child nodes in order, requiring all to succeed.
- **SequenceWithMemory**: A sequence node that "remembers" past results to avoid rechecking.
- **Fallback (Selector)**: A control node that runs child nodes in order, succeeding if any child succeeds.

These control nodes help manage the flow of tasks in a behavior tree, allowing for more dynamic and efficient decision-making, especially when dealing with asynchronous tasks, memory, and real-time reactions to the environment.

ifThenElse
Parallel
ParallelAll
WhileDoElse

Here are the notes for the additional Behavior Tree decorators: **ifThenElse**, **Parallel**, **ParallelAll**, and **WhileDoElse**.

### 1. **ifThenElse**
   - **Function:** Executes a child node based on a condition. If the condition is true, it runs one node (the "then" branch); if false, it runs another (the "else" branch).
   - **Usage:** This is a conditional control structure, similar to an if-else statement in programming. It allows for branching behavior based on specific conditions.
   - **Example:** If the character is low on health, run the "Retreat" behavior (then branch), else run "Attack" (else branch).
   
   **Behavior:**
   - **True condition:** Executes the "then" branch.
   - **False condition:** Executes the "else" branch.

---

### 2. **Parallel**
   - **Function:** Executes multiple child nodes simultaneously, but the node fails if any child node fails (i.e., the failure condition is triggered if any child node fails).
   - **Usage:** This is useful when you need multiple actions or behaviors to happen concurrently and you want the entire task to fail if any part of it fails.
   - **Example:** A character might perform several tasks simultaneously, like "Moving to target," "Attacking enemy," and "Watching for nearby threats," and if any of these tasks fail, the whole parallel task fails.

   **Behavior:**
   - **Success:** If at least one child node succeeds.
   - **Failure:** If any child node fails.

---

### 3. **ParallelAll**
   - **Function:** Executes multiple child nodes simultaneously, and all of them must succeed for the parallel node to succeed.
   - **Usage:** This decorator is used when all child nodes need to succeed before proceeding. It is useful in scenarios where every task must complete successfully before moving forward.
   - **Example:** If a character must both "attack" and "move to target" to succeed in a combat scenario, and both tasks must succeed before moving on to the next behavior.

   **Behavior:**
   - **Success:** If **all** child nodes succeed.
   - **Failure:** If **any** child node fails.

---

### 4. **WhileDoElse**
   - **Function:** Similar to a "while loop" in programming, it executes a child node repeatedly while a condition is true. If the condition becomes false, it executes another "else" node.
   - **Usage:** This decorator is used to keep performing actions as long as a condition holds true, and switch to a different behavior once the condition is no longer met.
   - **Example:** A character might continue "searching for an enemy" while the condition "enemy not found" is true. Once the condition becomes false (enemy found), it runs the "Attack" behavior as the "else" branch.

   **Behavior:**
   - **True condition:** Repeatedly runs the "do" node (until the condition is false).
   - **False condition:** Executes the "else" node.

---

### Summary of Key Differences:

- **ifThenElse**: Conditional branching behavior. Executes "then" branch if true, and "else" branch if false.
- **Parallel**: Executes multiple nodes at the same time, fails if any child fails.
- **ParallelAll**: Executes multiple nodes simultaneously, succeeds only if all children succeed.
- **WhileDoElse**: Repeats actions while a condition is true, then switches to another behavior once the condition becomes false.

These decorators allow for complex behaviors and are crucial for AI systems that require decision-making, concurrency, or repetitive actions.

Switch2
Switch3
Switch4
Switch5
Switch6


Here are the notes on the **Switch** decorators **Switch2**, **Switch3**, **Switch4**, **Switch5**, and **Switch6** used in Behavior Trees. These decorators allow for conditional branching based on an internal state, often simplifying decision-making by offering multiple paths.

### 1. **Switch2**
   - **Function:** Switches between two possible child nodes based on a condition.
   - **Usage:** It’s used when there are two possible paths to take, depending on a certain condition or state. If the condition is true, it executes one child node, and if false, it executes the other.
   - **Example:** A character might switch between "Patrol" and "Guard" behaviors depending on whether they are at a waypoint or have spotted an enemy.

   **Behavior:**
   - **Condition true:** Executes the first child node.
   - **Condition false:** Executes the second child node.

---

### 2. **Switch3**
   - **Function:** Switches between three possible child nodes, depending on the condition.
   - **Usage:** This is used when there are three possible behaviors to choose from, based on the condition. A value or condition determines which of the three branches to execute.
   - **Example:** A character might switch between three actions based on conditions:
     - If near an enemy: "Attack."
     - If low on health: "Retreat."
     - If idle: "Patrol."

   **Behavior:**
   - **Condition value 1:** Executes the first child node.
   - **Condition value 2:** Executes the second child node.
   - **Condition value 3:** Executes the third child node.

---

### 3. **Switch4**
   - **Function:** Switches between four child nodes based on a condition.
   - **Usage:** This is used when you have four distinct paths based on different conditions or states. It’s useful for more complex decision-making processes.
   - **Example:** A character could choose between:
     - "Idle" if nothing is happening.
     - "Move to target" if a target is detected.
     - "Attack" if the target is within range.
     - "Retreat" if health is too low.

   **Behavior:**
   - **Condition value 1:** Executes the first child node.
   - **Condition value 2:** Executes the second child node.
   - **Condition value 3:** Executes the third child node.
   - **Condition value 4:** Executes the fourth child node.

---

### 4. **Switch5**
   - **Function:** Switches between five possible child nodes based on a condition.
   - **Usage:** This is useful when there are more complex states or behaviors that need to be handled. It provides five different paths of execution.
   - **Example:** A more advanced AI might switch between:
     - "Patrol" if the area is clear.
     - "Alert" if suspicious activity is detected.
     - "Attack" if an enemy is detected.
     - "Retreat" if health is low.
     - "Rest" if the character is tired.

   **Behavior:**
   - **Condition value 1:** Executes the first child node.
   - **Condition value 2:** Executes the second child node.
   - **Condition value 3:** Executes the third child node.
   - **Condition value 4:** Executes the fourth child node.
   - **Condition value 5:** Executes the fifth child node.

---

### 5. **Switch6**
   - **Function:** Switches between six possible child nodes based on a condition.
   - **Usage:** This decorator is used when there are six potential actions or states that the AI might switch between. It’s helpful for highly complex AI with multiple behaviors.
   - **Example:** A character could switch between:
     - "Idle" if nothing is happening.
     - "Move to waypoint" if the destination is set.
     - "Patrol" if they are assigned a patrol route.
     - "Attack" if an enemy is within range.
     - "Defend" if an ally is under attack.
     - "Retreat" if health is critically low.

   **Behavior:**
   - **Condition value 1:** Executes the first child node.
   - **Condition value 2:** Executes the second child node.
   - **Condition value 3:** Executes the third child node.
   - **Condition value 4:** Executes the fourth child node.
   - **Condition value 5:** Executes the fifth child node.
   - **Condition value 6:** Executes the sixth child node.

---

### Summary:

- **Switch2**: Chooses between two branches based on a condition.
- **Switch3**: Chooses between three branches based on the condition.
- **Switch4**: Chooses between four branches based on the condition.
- **Switch5**: Chooses between five branches based on the condition.
- **Switch6**: Chooses between six branches based on the condition.

These **Switch** decorators provide a convenient way to implement multiple branching behaviors in a Behavior Tree, especially when the AI needs to handle various conditions or states, each requiring a different behavior.



Decorator
Delay
ForceFailure
ForceSuccess
Inverter
KeepRunningUntilFailure
LoopDouble
LoopString
Precondition
Repeat
RetryUnilSuccessful
RunOnce
Timeout


Certainly! Below are notes on various **Behavior Tree (BT)** decorators commonly used in game AI, robotics, and other decision-making systems. These decorators modify the behavior of nodes and influence how actions are executed within a tree.

### 1. **Delay**
   - **Function:** Adds a delay before the decorated node is executed.
   - **Usage:** Can be used to create pauses in actions or to prevent nodes from executing immediately. Often used in systems that need to wait for a specific condition before proceeding.
   - **Example:** A character might wait for 2 seconds before taking an action.

---

### 2. **ForceFailure**
   - **Function:** Forces the child node to fail, regardless of the child’s internal status.
   - **Usage:** Used to override the result of a node. It is useful when you want to explicitly cause failure in certain conditions, regardless of the success or failure of the child node.
   - **Example:** Used when you want to stop a task, even if it's usually successful.

---

### 3. **ForceSuccess**
   - **Function:** Forces the child node to succeed, regardless of the child’s internal status.
   - **Usage:** Can be used when you want a node to succeed regardless of the actual outcome of the child node. Often useful in handling specific states or forcing a positive result in a task.
   - **Example:** You can force a "search" task to appear successful even if no target is found.

---

### 4. **Inverter**
   - **Function:** Inverts the result of the child node.
   - **Usage:** If the child node would normally succeed, it will fail, and if it would normally fail, it will succeed. It is used to reverse the logic of the node.
   - **Example:** Used when you need to implement a condition like "if not X," where "X" is a condition that would normally succeed.

---

### 5. **KeepRunningUntilFailure**
   - **Function:** Continually executes the child node until it fails.
   - **Usage:** Keeps the child node running until a failure occurs. It is useful for repeating actions or processes until a failure condition is met (e.g., keep attacking until an enemy dies).
   - **Example:** A character might continue to attack until their health drops below a certain threshold.

---

### 6. **LoopDouble**
   - **Function:** Executes the child node twice before continuing.
   - **Usage:** A form of loop decorator that forces the child node to repeat its execution twice before continuing with the next node.
   - **Example:** Can be used for tasks where a behavior needs to be repeated exactly two times.

---

### 7. **LoopString**
   - **Function:** Loops through the child node a number of times specified by a string value.
   - **Usage:** The string defines how many times the node should be repeated, useful for repeating a task a dynamic number of times.
   - **Example:** You can repeat an action a specific number of times based on a variable condition.

---

### 8. **Precondition**
   - **Function:** Checks a condition before executing the child node.
   - **Usage:** Used to ensure that certain conditions are met before executing a task. If the condition is false, the child node will not run.
   - **Example:** A task like "Attack" might have a precondition that the target must be within range.

---

### 9. **Repeat**
   - **Function:** Repeats the child node a specific number of times.
   - **Usage:** Forces the execution of the child node multiple times before moving to the next node.
   - **Example:** A character might repeat a "search" action three times, checking different areas for a target.

---

### 10. **RetryUntilSuccessful**
   - **Function:** Keeps retrying the child node until it succeeds.
   - **Usage:** Useful for tasks where the system should keep trying until it gets a successful result, such as attempting to find a path until one is found.
   - **Example:** Trying to open a door repeatedly until it is successfully opened.

---

### 11. **RunOnce**
   - **Function:** Executes the child node only once.
   - **Usage:** Ensures that a particular task or action is only performed once, regardless of whether it is successful or not.
   - **Example:** A character might perform an action like "pickup item" just once, after which the action is ignored in subsequent evaluations.

---

### 12. **Timeout**
   - **Function:** If the child node takes longer than a set time limit, it is considered a failure.
   - **Usage:** Useful for limiting the execution time of tasks or actions. If a task takes too long, it will be failed and the system can move to another behavior.
   - **Example:** A "search for enemy" node might be given a timeout if it takes longer than a set threshold to find the target.

---

These decorators modify the behavior of BT nodes and help add more control, decision-making, and flexibility to the AI systems or agents using the behavior tree structure.


Subtree





The `BT::*` namespace in Behavior Trees (BT) typically refers to classes and functions that are part of the Behavior Tree library, specifically **BehaviorTree.CPP** (a popular implementation in C++). There are many different options and types that are provided within the `BT::*` namespace to handle the different aspects of creating and managing behavior trees.

Here are the main categories of options or classes within the `BT::*` namespace in BehaviorTree.CPP:

### 1. **Nodes**
   - **Action Nodes**: Perform an action or a task (e.g., `BT::ActionNodeBase`, `BT::AsyncActionNode`).
   - **Condition Nodes**: Check a condition and return `SUCCESS` or `FAILURE` based on the condition (e.g., `BT::ConditionNode`).
   - **Control Flow Nodes**: Control the sequence of execution of other nodes (e.g., `BT::Sequence`, `BT::Fallback`, `BT::DecoratorNode`).
   - **Composite Nodes**: Nodes that can contain multiple child nodes and control their execution (e.g., `BT::Sequence`, `BT::Selector`, `BT::Parallel`).
   - **Decorator Nodes**: Modify or add extra behavior to a node (e.g., `BT::Repeat`, `BT::Timeout`, `BT::ForceSuccess`, `BT::ForceFailure`).

### 2. **Node Types**
   These are specific node types defined within the `BT::*` namespace, including:

   - **`BT::ActionNodeBase`**: The base class for action nodes that represent actions to be executed.
   - **`BT::ConditionNode`**: The base class for condition nodes that are used to check if some condition is met.
   - **`BT::DecoratorNode`**: A node that decorates another node, typically modifying its behavior (e.g., `BT::ForceSuccess`, `BT::Repeat`, `BT::Inverter`).
   - **`BT::ControlNode`**: The base class for control flow nodes like `Sequence`, `Fallback`, etc.
   - **`BT::Sequence`**: A composite node that runs its children sequentially, failing immediately when one of them fails.
   - **`BT::Fallback`**: A composite node that runs its children sequentially, succeeding when one child succeeds.
   - **`BT::Parallel`**: A node that runs all its children in parallel and has specific criteria to determine success or failure.
   - **`BT::AsyncActionNode`**: Action nodes that are executed asynchronously, allowing non-blocking behavior.
   - **`BT::AsyncFallback`**: A control node that tries its children asynchronously, skipping to the next child if one fails.
   - **`BT::AsyncSequence`**: A control node that processes its children asynchronously in sequence.

### 3. **Ports and Inputs/Outputs**
   - **`BT::InputPort`**: Represents input ports where data can be provided to nodes.
   - **`BT::OutputPort`**: Represents output ports where nodes can send data to other nodes.
   - **`BT::PortsList`**: A list of ports used by nodes for data exchange.

### 4. **Node Statuses**
   - **`BT::NodeStatus::SUCCESS`**: The status indicating that the node has succeeded.
   - **`BT::NodeStatus::FAILURE`**: The status indicating that the node has failed.
   - **`BT::NodeStatus::RUNNING`**: The status indicating that the node is still running and not yet finished.

### 5. **Utility Classes**
   - **`BT::Tree`**: Represents the behavior tree itself.
   - **`BT::TreeNode`**: A base class for all the nodes in the behavior tree.
   - **`BT::BehaviorTreeFactory`**: A factory class used to create behavior trees from XML or programmatically.
   - **`BT::RuntimeError`**: Used to throw errors during runtime in behavior trees.

### 6. **Decorators**
   Decorators modify the behavior of their children by adding extra functionality, such as retrying, inverting the result, or timing out:
   - **`BT::Repeat`**: Repeats the execution of its child until a certain condition is met.
   - **`BT::Timeout`**: Executes the child node until it times out.
   - **`BT::ForceSuccess`**: Forces the child node to always return `SUCCESS`.
   - **`BT::ForceFailure`**: Forces the child node to always return `FAILURE`.
   - **`BT::Inverter`**: Inverts the result of its child node (e.g., `SUCCESS` becomes `FAILURE` and vice versa).
   - **`BT::KeepRunningUntilFailure`**: Keeps running the child node until it fails.
   - **`BT::Delay`**: Introduces a delay before the child node is executed.

### 7. **Behavior Tree Execution**
   - **`BT::BehaviorTree`**: This class represents an entire behavior tree and provides methods to tick the root node, execute it, and manage status updates.

---

### **Main Options in BT::* Namespace:**
To summarize the available types of options in `BT::*`:

- **Nodes**: Action, Condition, Control Flow, Composite, and Decorator nodes.
- **Control Flow**: Sequence, Fallback, Parallel, AsyncSequence, AsyncFallback, ReactiveSequence, ReactiveFallback.
- **Node Types**: Sequence, Fallback, ConditionNode, ActionNode, AsyncNode, and more.
- **Statuses**: SUCCESS, FAILURE, RUNNING.

---

### **How Many Options Are There?**
While there are many classes and types in the `BT::*` namespace, the number can vary slightly depending on the version of the Behavior Tree library and custom nodes you add. Below is an estimate of the core categories:
- **Node Types**: Around 15 to 20 core node types.
- **Control Flow Nodes**: Around 6 types (Sequence, Fallback, Parallel, etc.).
- **Decorator Nodes**: Around 10 common types (Repeat, ForceSuccess, Timeout, etc.).

### **Key Takeaway**:
Behavior Tree frameworks are highly extensible, and the number of available node types or classes (`BT::*`) depends on the specific implementation, but the categories I mentioned above cover most of the commonly used types in the BehaviorTree.CPP library. You can always extend it further by defining custom nodes specific to your use case.