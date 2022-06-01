#ifndef TX_BARRIER_H
#define TX_BARRIER_H

#include "tx_api.h"

typedef struct TX_BARRIER_STRUCT
{
    CHAR* tx_barrier_name;
    ULONG tx_barrier_counter;
    ULONG tx_barrier_raise_count;
    TX_MUTEX tx_barrier_mtx_counter;
    TX_SEMAPHORE tx_barrier_sem_raise;
} TX_BARRIER;

/**
* Create a barrier.
* @param barrier_ptr Pointer to a barrier control block.
* @param name_ptr Pointer to the name of the barrier.
* @param raise_count Number of call to tx_barrier_wait required to raise the barrier.
* @param priority_inherit Specifies whether or not this barrier supports priority inheritance. If this value is TX_INHERIT, then priority inheritance is supported. However, if TX_NO_INHERIT is specified, priority inheritance is not supported by this barrier.
* @retval TX_SUCCESS (0x00) Successful barrier creation.
* @retval TX_SEMAPHORE_ERROR (0x0C) Invalid internal semaphore pointer. Either the pointer is NULL or the barrier is already created.
* @retval TX_MUTEX_ERROR (0x1C) Invalid internal mutex pointer. Either the pointer is NULL or the mutex is already created.
* @retval TX_INHERIT_ERROR (0x1F) Invalid priority inherit parameter.
* @retval TX_CALLER_ERROR (0x13) Invalid caller of this service.
*/
UINT tx_barrier_create(TX_BARRIER* barrier_ptr, CHAR* name_ptr, UINT raise_count, UINT inherit);

/**
* Delete a barrier.
* @param barrier_ptr Pointer to a previously created barrier.
* @retval TX_SUCCESS(0x00) Successful barrier deletion.
* @retval TX_SEMAPHORE_ERROR (0x0C) Invalid internal counting semaphore pointer.
* @retval TX_CALLER_ERROR(0x13) Invalid caller of this service.
*/
UINT tx_barrier_delete(TX_BARRIER* barrier_ptr);

/**
* Synchronize the current thread with the other thread using the barrier.
* @param barrier_ptr Pointer to a previously created barrier.
* @param wait_option Defines how the service behaves if no all thread reached the barrier. Can be TX_NO_WAIT, TX_WAIT_FOREVER or a positive number of ticks to wait for.
* @retval TX_SUCCESS (0x00) Successful retrieval of a semaphore instance.
* @retval TX_DELETED (0x01) Internal mutex or counting semaphore was deleted while thread was suspended.
* @retval TX_NO_INSTANCE (0x0D) Barrier is not raised within the specified time to wait. This thread still being counted, it must not retry.
* @retval TX_WAIT_ABORTED (0x1A) Suspension was aborted by another thread, timer, or ISR. Can be Done by calling tx_barrier_reset.
* @retval TX_MUTEX_ERROR (0x1C) Invalid internal mutex pointer.
* @retval TX_SEMAPHORE_ERROR (0x0C) Invalid internal counting semaphore pointer.
* @retval TX_WAIT_ERROR (0x04) A wait option other than TX_NO_WAIT was specified on a call from a non-thread.
*/
UINT tx_barrier_wait(TX_BARRIER* barrier_ptr, ULONG wait_option);

/**
* Reset the barrier, all waiting operation is aborted.
* @param barrier_ptr Pointer to a previously created barrier.
* @retval TX_SUCCESS (0x00) Successful retrieval of a semaphore instance.
*/
UINT tx_barrier_reset(TX_BARRIER* barrier_ptr);

#endif
