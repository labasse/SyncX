#ifndef TX_TASKQ_H
#define TX_TASKQ_H

#include "tx_api.h"

#define TX_TASKQ_RUNNER_PRIO    0
/* #define TX_DISABLE_NOTIFY_CALLBACKS */

typedef TX_QUEUE TX_TASKQ;

typedef struct TX_TASKQ_ITEM_STRUCT {
    VOID(*task_entry_function)(ULONG);
    ULONG task_input;
    UINT task_priority, task_preemption;
} TX_TASKQ_ITEM;

/**
* @brief Notify application before task start or after its end.
* @param taskq_enter_exit_notify a notify function called for each task start (started==TX_TRUE) or end (started==TX_FALSE). TX_NULL to release notification.
* @retval TX_SUCCESS (0x00) Successful notification registration.
* @retval TX_FEATURE_NOT_ENABLED (0xFF) The system was compiled with notification capabilities disabled.
*/
UINT tx_taskq_enter_exit_notify(VOID(*taskq_enter_exit_notify)(TX_TASKQ_ITEM* item, UINT started));

/**
* @brief Get the memory size needed for the task queue.
* @param max_pending_task Max pending task supported by the task queue.
* @return Memory size needed, in bytes.
* @see tx_taskq_create
*/
#define TX_TASKQ_SIZE(max_pending_task) ((max_pending_task)*sizeof(TX_TASKQ_ITEM))

VOID tx_taskq_runner_entry_function(ULONG task_pool_ptr);

/**
* @brief Create a task queue.
* @param taskq_ptr Pointer to a task queue control block.
* @param name_ptr Pointer to the name of the task queue.
* @param taskq_start Starting address of the task queue. The starting address must be aligned to the size of the ULONG data type.
* @param taskq_size Total number of bytes available for the task queue, can be found using the TX_TASKQ_SIZE macro.
* @retval TX_SUCCESS (0x00) Successful task queue creation.
* @retval TX_QUEUE_ERROR (0x09) Invalid task queue pointer. Either the pointer is NULL or the queue is already created.
* @retval TX_PTR_ERROR (0x03) Invalid starting address of the task queue.
* @retval TX_SIZE_ERROR (0x05) Size of task queue is invalid.
* @retval TX_CALLER_ERROR (0x13) Invalid caller of this service.
* @see TX_TASKQ_SIZE
* @see tx_taskq_delete
*/
#define tx_taskq_create(taskq_ptr, name_ptr, taskq_start, taskq_size) \
    tx_queue_create(taskq_ptr, name_ptr, sizeof(TX_TASKQ_ITEM)/4, taskq_start, taskq_size)

/**
* @brief Delete the task queue.
* @param queue_ptr Pointer to a previously created task queue.
* @retval TX_SUCCESS (0x00) Successful task queue deletion.
* @retval TX_QUEUE_ERROR (0x09) Invalid task queue pointer.
* @retval TX_CALLER_ERROR (0x13) Invalid caller of this service.
*/
#define tx_taskq_delete \
    tx_queue_delete

/**
* @brief Create a thread to run the tasks added to the queue.
* @param taskq_ptr Pointer to a previously created task queue.
* @param thread_ptr Pointer to a thread control block.
* @param name_ptr Pointer to the name of the thread.
* @param stack_start Starting address of the stack's memory area.
* @param stack_size Number bytes in the stack memory area. The thread's stack area must be large enough to handle its worst-case function call nesting and local variable usage.
* @retval TX_SUCCESS (0x00) Successful thread creation.
* @retval TX_THREAD_ERROR (0x0E) Invalid thread control pointer. Either the pointer is NULL or the thread is already created.
* @retval TX_PTR_ERROR (0x03) Invalid starting address of the entry point or the stack area is invalid, usually NULL.
* @retval TX_SIZE_ERROR (0x05) Size of stack area is invalid. Threads must have at least TX_MINIMUM_STACK bytes to execute.
* @retval TX_PRIORITY_ERROR (0x0F) Invalid thread priority, which is a value outside the range of (0 through (TX_MAX_PRIORITIES-1)).
* @retval TX_THRESH_ERROR (0x18) Invalid preemptionthreshold specified. This value must be a valid priority less than or equal to the initial priority of the thread.
* @retval TX_START_ERROR (0x10) Invalid auto-start selection.
* @retval TX_CALLER_ERROR (0x13) Invalid caller of this service.
*/
#define tx_taskq_create_runner(taskq_ptr, thread_ptr, name_ptr, stack_start, stack_size) \
    tx_thread_create(thread_ptr, name_ptr, tx_taskq_runner_entry_function, (ULONG)(taskq_ptr), stack_start, stack_size, TX_TASKQ_RUNNER_PRIO, TX_TASKQ_RUNNER_PRIO, TX_NO_TIME_SLICE, TX_AUTO_START)

/**
* @brief Send a task to the task queue in order to be executed by a runner.
* @param taskq_ptr Pointer to a previously created task queue.
* @param task_entry_function Specifies the initial C function for task execution. The task ends when it returns from this entry function.
* @param task_input A 32-bit value that is passed to the task's entry function when it first executes. The use for this input is determined exclusively by the application.
* @param priority Numerical priority of task. Legal values range from 0 through (TX_MAX_PRIORITES-1), where a value of 0 represents the highest priority.
* @param preemption_treshold Highest priority level (0 through (TX_MAX_PRIORITIES-1)) of disabled preemption. Only priorities higher than this level are allowed to preempt this task. This value must be less than or equal to the specified priority. A value equal to the task priority disables preemption-threshold.
* @param wait_option Defines how the service behaves if the task queue is full. Can be TX_NO_WAIT, TX_WAIT_FOREVER or a positive number of ticks to wait for.
* @retval TX_SUCCESS (0x00) Successful sending of task.
* @retval TX_DELETED (0x01) Message queue was deleted while thread was suspended.
* @retval TX_QUEUE_FULL (0x0B) Service was unable to send task because the queue was full for the duration of the specified time to wait.
* @retval TX_WAIT_ABORTED (0x1A) Suspension was aborted by another thread, timer, or ISR.
* @retval TX_QUEUE_ERROR (0x09) Invalid task queue pointer.
* @retval TX_PTR_ERROR (0x03) Invalid source pointer for task.
* @retval TX_WAIT_ERROR (0x04) A wait option other than TX_NO_WAIT was specified on a call from a nonthread.
*/
#define tx_taskq_send(taskq_ptr, task_entry_function, task_input, priority, preemption_treshold, wait_option) { \
        TX_TASKQ_ITEM task = { task_entry_function, task_input, priority, preemption_treshold };                           \
        tx_queue_send(taskq_ptr, &task, wait_option);                                                                      \
    }

/**
* @brief Send a task to the front of the task queue in order to be executed by a runner.
* @param taskq_ptr Pointer to a previously created task queue.
* @param task_entry_function Specifies the initial C function for task execution. The task ends when it returns from this entry function.
* @param task_input A 32-bit value that is passed to the task's entry function when it first executes. The use for this input is determined exclusively by the application.
* @param priority Numerical priority of task. Legal values range from 0 through (TX_MAX_PRIORITES-1), where a value of 0 represents the highest priority.
* @param preemption_treshold Highest priority level (0 through (TX_MAX_PRIORITIES-1)) of disabled preemption. Only priorities higher than this level are allowed to preempt this task. This value must be less than or equal to the specified priority. A value equal to the task priority disables preemption-threshold.
* @param wait_option Defines how the service behaves if the message queue is full. Can be TX_NO_WAIT, TX_WAIT_FOREVER or a positive number of ticks to wait for.
* @retval TX_SUCCESS (0x00) Successful sending of task.
* @retval TX_DELETED (0x01) Message queue was deleted while thread was suspended.
* @retval TX_QUEUE_FULL (0x0B) Service was unable to send task because the queue was full for the duration of the specified time to wait.
* @retval TX_WAIT_ABORTED (0x1A) Suspension was aborted by another thread, timer, or ISR.
* @retval TX_QUEUE_ERROR (0x09) Invalid task queue pointer.
* @retval TX_PTR_ERROR (0x03) Invalid source pointer for task.
* @retval TX_WAIT_ERROR (0x04) A wait option other than TX_NO_WAIT was specified on a call from a nonthread.
*/
#define tx_taskq_front_send(taskq_ptr, task_entry_function, task_input, priority, preemption_treshold, wait_option) { \
        TX_TASKQ_ITEM task = { task_entry_function, task_input, priority, preemption_treshold };                                 \
        tx_queue_front_send(taskq_ptr, &task, wait_option);                                                                      \
    }

/**
* @brief Empty pending tasks in the task queue.
* @param queue_ptr Pointer to a previously created message queue.
* @retval TX_SUCCESS (0x00) Successful task queue flush.
* @retval TX_QUEUE_ERROR (0x09) Invalid task queue pointer.
*/
#define tx_taskq_flush \
    tx_queue_flush  

#endif
