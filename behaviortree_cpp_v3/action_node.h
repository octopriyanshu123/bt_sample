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

#ifndef BEHAVIORTREECORE_ACTIONNODE_H
#define BEHAVIORTREECORE_ACTIONNODE_H

#include <atomic>
#include <thread>
#include <future>
#include <mutex>

#include "leaf_node.h"

namespace BT
{
// IMPORTANT: Actions which returned SUCCESS or FAILURE will not be ticked
// again unless resetStatus() is called first.
// Keep this in mind when writing your custom Control and Decorator nodes.

/**
 * @brief The ActionNodeBase is the base class to use to create any kind of action.
 * A particular derived class is free to override executeTick() as needed.
 *
 */
class ActionNodeBase : public LeafNode
{
public:
  ActionNodeBase(const std::string& name, const NodeConfiguration& config);
  ~ActionNodeBase() override = default;

  virtual NodeType type() const override final
  {
    return NodeType::ACTION;
  }
};

/**
 * @brief The SyncActionNode is an ActionNode that
 * explicitly prevents the status RUNNING and doesn't require
 * an implementation of halt().
 * 
 * 
 * Here's the `SimpleActionNode` code with detailed comments explaining each part:

```cpp
// Constructor for SimpleActionNode
// Initializes the base class SyncActionNode and stores the tick_functor which is a function to be called during tick.
SimpleActionNode::SimpleActionNode(const std::string& name,
                                    SimpleActionNode::TickFunctor tick_functor,
                                    const NodeConfiguration& config)
    : SyncActionNode(name, config),   // Initialize SyncActionNode base class with name and config
      tick_functor_(std::move(tick_functor)) // Store the tick functor, which is the function that defines the action
{
    // Constructor body - nothing additional to do since initialization is handled in the member initializer list.
}

// Method to execute the action during the tick
// It changes the node's status and calls the tick functor to perform the action.
NodeStatus SimpleActionNode::tick()
{
    // Store the current status before attempting to change it
    NodeStatus prev_status = status();

    // If the node is currently IDLE, update its status to RUNNING
    if (prev_status == NodeStatus::IDLE)
    {
        setStatus(NodeStatus::RUNNING);  // Set the node's status to RUNNING, indicating it's now active
        prev_status = NodeStatus::RUNNING;  // Update prev_status to reflect the change
    }

    // Call the tick functor to execute the action and get the new status
    NodeStatus status = tick_functor_(*this);

    // If the new status is different from the previous status, update the node's status
    if (status != prev_status)
    {
        setStatus(status);  // Update the status to the new status
    }

    // Return the current status of the node (after execution of the tick functor)
    return status;
}
```

### Detailed Explanation:

1. **Constructor (`SimpleActionNode`)**:
    - The constructor accepts three parameters: 
      - `name`: A string representing the node's name.
      - `tick_functor`: A function (or callable) that defines the action performed by this node when the `tick()` method is called.
      - `config`: The configuration for the node.
    - The constructor calls the base class `SyncActionNode` constructor to initialize it with the provided `name` and `config`.
    - It then stores the `tick_functor` (the function to be executed during the tick) using `std::move` to efficiently transfer ownership of the functor.

2. **`tick()` Method**:
    - This method is called to execute the action associated with the node.
    - **Status Check**: 
      - The current status of the node is checked using `status()`. The `prev_status` variable holds this value before any changes are made.
      - If the current status is `IDLE`, it updates the status to `RUNNING` because the action is about to be executed. The `prev_status` is also updated to `RUNNING` to reflect this change.
    
    - **Execute the Action**: 
      - The `tick_functor_` is called to perform the node's action. The `tick_functor` is expected to return a `NodeStatus` (either `IDLE`, `RUNNING`, or `SUCCESS`, etc.). This value is stored in the `status` variable.
    
    - **Status Update**: 
      - If the new status returned by the `tick_functor_` is different from `prev_status`, the status of the node is updated using `setStatus(status)` to reflect the new status.
    
    - **Return the Status**: 
      - Finally, the current `status` is returned, indicating the result of the action (whether it is still running, finished, or encountered an issue).

### Key Concepts:

- **`tick_functor_`**: This is a function that encapsulates the logic of the action that the node performs. It is a key part of the `SimpleActionNode` because it defines the behavior of the node when `tick()` is invoked.
  
- **Status Management**: The node’s status is carefully managed during the execution of the tick. Initially, it may be `IDLE`, and once the action begins, it is set to `RUNNING`. After the action finishes, the status could change to something else based on the result of `tick_functor_`.

- **Functor Pattern**: The use of `TickFunctor` allows for flexible, dynamic behavior, where different instances of `SimpleActionNode` can execute different actions based on the passed functor. This makes the node highly reusable for different tasks. 

In this design, the node does not inherently know the details of the action it is performing; it delegates that responsibility to the functor (`tick_functor_`).



 */
class SyncActionNode : public ActionNodeBase
{
public:
  SyncActionNode(const std::string& name, const NodeConfiguration& config);
  ~SyncActionNode() override = default;

  /// throws if the derived class return RUNNING.
  virtual NodeStatus executeTick() override;

  /// You don't need to override this
  virtual void halt() override final
  {}
};

/**
 * @brief The SimpleActionNode provides an easy to use SyncActionNode.
 * The user should simply provide a callback with this signature
 *
 *    BT::NodeStatus functionName(TreeNode&)
 *
 * This avoids the hassle of inheriting from a ActionNode.
 *
 * Using lambdas or std::bind it is easy to pass a pointer to a method.
 * SimpleActionNode is executed synchronously and does not support halting.
 * NodeParameters aren't supported.
 */
class SimpleActionNode : public SyncActionNode
{
public:
  typedef std::function<NodeStatus(TreeNode&)> TickFunctor;

  // You must provide the function to call when tick() is invoked
  SimpleActionNode(const std::string& name, TickFunctor tick_functor,
                   const NodeConfiguration& config);

  ~SimpleActionNode() override = default;

protected:
  virtual NodeStatus tick() override final;

  TickFunctor tick_functor_;
};

/**
 * @brief The AsyncActionNode uses a different thread, where the action will be
 * executed.
 *
 * IMPORTANT: this action is quite hard to implement correctly. Please be sure that you know what you are doing.
 *
 * - In your overriden tick() method, you must check periodically
 *   the result of the method isHaltRequested() and stop your execution accordingly.
 *
 * - in the overriden halt() method, you can do some cleanup, but do not forget to
 *   invoke the base class method AsyncActionNode::halt();
 *
 * - remember, with few exceptions, a halted AsyncAction must return NodeStatus::IDLE.
 *
 * For a complete example, look at __AsyncActionTest__ in action_test_node.h in the folder test.
 *
 * NOTE: when the thread is completed, i.e. the tick() returns its status,
 * a TreeNode::emitStateChanged() will be called.
 * 
 * Here's the `AsyncActionNode` code with detailed explanations added as comments directly within the code:

```cpp
// Constructor for AsyncActionNode
// Initializes the base ActionNodeBase class and sets up the thread to run asynchronously.
AsyncActionNode::AsyncActionNode(const std::string& name, const NodeConfiguration& config)
    : ActionNodeBase(name, config),  // Initialize base class with name and config
      keep_thread_alive_(true),      // Flag to control whether the thread should keep running
      start_action_(false)           // Flag to signal when the action should start
{
    // Create a new thread to run the asynchronous loop
    thread_ = std::thread(&AsyncActionNode::asyncThreadLoop, this); 
}

// Destructor for AsyncActionNode
// Ensures the background thread is properly joined before destruction of the object.
AsyncActionNode::~AsyncActionNode()
{
    // If the thread is still joinable (i.e., it hasn't finished execution)
    if (thread_.joinable())
    {
        // Stop the thread and wait for it to finish execution
        stopAndJoinThread();
    }
}

// Method to wait for the background thread to start the action
void AsyncActionNode::waitStart()
{
    // Lock the mutex to ensure thread safety while modifying start_action_
    std::unique_lock<std::mutex> lock(start_mutex_);

    // Wait until the start_action_ flag is set to true, which signals the thread to start
    while (!start_action_)
    {
        start_signal_.wait(lock); // Block the thread until notified
    }
    
    // Reset the start_action_ flag to false after the thread starts executing
    start_action_ = false;
}

// Method to notify the background thread to start the action
void AsyncActionNode::notifyStart()
{
    // Lock the mutex to ensure thread safety while modifying start_action_
    std::unique_lock<std::mutex> lock(start_mutex_);

    // Set start_action_ flag to true to notify the waiting thread to start
    start_action_ = true;

    // Notify all waiting threads (in case there are multiple threads waiting)
    start_signal_.notify_all();
}

// The background thread loop that handles asynchronous execution
void AsyncActionNode::asyncThreadLoop()
{
    // Continue looping as long as the thread is allowed to run
    while (keep_thread_alive_.load())
    {
        // Wait for the signal to start the action
        waitStart();

        // Check again if the thread should keep running (it might have been signaled to stop)
        if (keep_thread_alive_)
        {
            try {
                // Execute the tick method and update the status
                setStatus(tick());
            }
            catch (std::exception&)
            {
                // If an exception occurs in tick(), print the error and stop the thread
                std::cerr << "\nUncaught exception from the method tick() of an AsyncActionNode: ["
                          << registrationName() << "/" << name() << "]\n" << std::endl;
                
                // Store the exception and stop the thread
                exptr_ = std::current_exception();
                keep_thread_alive_ = false;
            }
        }
    }
}

// The main method to trigger the action in the AsyncActionNode
NodeStatus AsyncActionNode::executeTick()
{
    // If the node is idle, set it to running and notify the background thread to start
    if (status() == NodeStatus::IDLE)
    {
        setStatus(NodeStatus::RUNNING);  // Update status to RUNNING
        notifyStart();  // Notify the background thread to begin execution
    }

    // If an exception was previously stored, rethrow it to propagate the error
    if (exptr_)
    {
        std::rethrow_exception(exptr_);
    }

    // Return the current status of the node
    return status();
}

// Method to stop the background thread and ensure it has finished execution
void AsyncActionNode::stopAndJoinThread()
{
    // Signal the background thread to stop by setting keep_thread_alive_ to false
    keep_thread_alive_.store(false);

    // Notify the waiting thread (if any) to wake up and check the stopping condition
    notifyStart();

    // If the thread is joinable (i.e., it has been started and not yet finished)
    if (thread_.joinable())
    {
        // Wait for the thread to finish execution (join it)
        thread_.join();
    }
}
```

### Detailed Explanation in the Comments:

1. **Constructor**: 
   - Initializes the base class `ActionNodeBase` with `name` and `config`.
   - The flags `keep_thread_alive_` and `start_action_` are set to control the thread's behavior and signal when the action should start.
   - A background thread is created and runs the `asyncThreadLoop` method, which handles the asynchronous task.

2. **Destructor**:
   - Ensures that when the `AsyncActionNode` object is destroyed, the background thread is properly joined, meaning the main thread will wait for the background thread to finish.

3. **waitStart()**:
   - The method uses a `std::mutex` to synchronize the start signal.
   - The thread waits until `start_action_` is set to `true`, signaling it to begin execution. This ensures that the background thread only starts its work when signaled.

4. **notifyStart()**:
   - This method is called by the main thread to signal the background thread to begin executing the action.
   - It sets the `start_action_` flag to `true` and notifies the waiting thread using `notify_all()`.

5. **asyncThreadLoop()**:
   - This is the loop running in the background thread. It keeps running as long as `keep_thread_alive_` is `true`.
   - The loop waits for a signal to start the action (`waitStart()`), then calls `tick()` to execute the action.
   - If an exception is thrown during the execution of `tick()`, it is caught, and the thread is stopped by setting `keep_thread_alive_` to `false`.

6. **executeTick()**:
   - This method is called by the main thread to start the execution of the asynchronous action.
   - If the node is `IDLE`, it updates its status to `RUNNING` and signals the background thread to start.
   - It also rethrows any stored exceptions, ensuring that errors propagate correctly.

7. **stopAndJoinThread()**:
   - This method is responsible for stopping the background thread and ensuring it finishes before the object is destroyed.
   - It sets `keep_thread_alive_` to `false`, signaling the background thread to stop, and calls `notifyStart()` to wake up the thread and let it handle the stop request.
   - If the thread is still running, it is joined, meaning the main thread will wait for the background thread to finish.

### Summary of Key Concepts:

- **Thread Synchronization**: The code uses `std::mutex` and `std::condition_variable` to synchronize the main thread and the background thread. The main thread signals the background thread to start processing, and the background thread waits for that signal.
  
- **Exception Handling**: If an exception occurs in the background thread during the execution of `tick()`, it is caught, stored, and used to stop the thread gracefully.

- **Thread Lifecycle Management**: The background thread is properly managed by ensuring it is joined when the object is destroyed, preventing potential issues like dangling threads.
 
 */
class AsyncActionNode : public ActionNodeBase
{
public:
  AsyncActionNode(const std::string& name, const NodeConfiguration& config) :
    ActionNodeBase(name, config)
  {}

  bool isHaltRequested() const
  {
    return halt_requested_.load();
  }

  // This method spawn a new thread. Do NOT remove the "final" keyword.
  virtual NodeStatus executeTick() override final;

  virtual void halt() override;

private:
  std::exception_ptr exptr_;
  std::atomic_bool halt_requested_;
  std::future<void> thread_handle_;
  std::mutex mutex_;
};

/**
 * @brief The ActionNode is the prefered way to implement asynchronous Actions.
 * It is actually easier to use correctly, when compared with AsyncAction
 *
 * It is particularly useful when your code contains a request-reply pattern,
 * i.e. when the actions sends an asychronous request, then checks periodically
 * if the reply has been received and, eventually, analyze the reply to determine
 * if the result is SUCCESS or FAILURE.
 *
 * -) an action that was in IDLE state will call onStart()
 *
 * -) A RUNNING action will call onRunning()
 *
 * -) if halted, method onHalted() is invoked
 */
class StatefulActionNode : public ActionNodeBase
{
public:
  StatefulActionNode(const std::string& name, const NodeConfiguration& config) :
    ActionNodeBase(name, config)
  {}

  // do not override this method
  NodeStatus tick() override final;
  // do not override this method
  void halt() override final;

  /// method to be called at the beginning.
  /// If it returns RUNNING, this becomes an asychronous node.
  virtual NodeStatus onStart() = 0;

  /// method invoked by a RUNNING action.
  virtual NodeStatus onRunning() = 0;

  /// when the method halt() is called and the action is RUNNING, this method is invoked.
  /// This is a convenient place todo a cleanup, if needed.
  virtual void onHalted() = 0;
};

#ifndef BT_NO_COROUTINES

/**
 * @brief The CoroActionNode class is an a good candidate for asynchronous actions
 * which need to communicate with an external service using an asynch request/reply interface.
 *
 * It is up to the user to decide when to suspend execution of the Action and resume
 * the parent node, invoking the method setStatusRunningAndYield().
 */
class CoroActionNode : public ActionNodeBase
{
public:
  CoroActionNode(const std::string& name, const NodeConfiguration& config);
  virtual ~CoroActionNode() override;

  /// Use this method to return RUNNING and temporary "pause" the Action.
  void setStatusRunningAndYield();

  // This method triggers the TickEngine. Do NOT remove the "final" keyword.
  virtual NodeStatus executeTick() override final;

  /** You may want to override this method. But still, remember to call this
    * implementation too.
    *
    * Example:
    *
    *     void MyAction::halt()
    *     {
    *         // do your stuff here
    *         CoroActionNode::halt();
    *     }
    */
  void halt() override;

protected:
  struct Pimpl;   // The Pimpl idiom
  std::unique_ptr<Pimpl> _p;
};
#endif

}   // namespace BT

#endif
