#ifndef TX_RWLOCK_H
#define TX_RWLOCK_H

#include "tx_api.h"

typedef struct TX_RWLOCK_STRUCT
{
    CHAR* tx_rwlock_name;
    ULONG tx_rwlock_rcounter;
    TX_MUTEX tx_rwlock_mtx_rcounter;
    TX_SEMAPHORE tx_rwlock_sem_write;
} TX_RWLOCK;

/**
* Create a read/write lock.
* @param rwlock_ptr Pointer to a r/w lock control block.
* @param name_ptr Pointer to the name of the r/w lock.
* @param priority_inherit Specifies whether or not this r/w lock supports priority inheritance. If this value is TX_INHERIT, then priority inheritance is supported. However, if TX_NO_INHERIT is specified, priority inheritance is not supported by this r/w lock.
* @retval TX_SUCCESS (0x00) Successful r/w lock creation.
* @retval TX_SEMAPHORE_ERROR (0x0C) Invalid internal semaphore pointer. Either the pointer is NULL or the barrier is already created.
* @retval TX_MUTEX_ERROR (0x1C) Invalid internal mutex pointer. Either the pointer is NULL or the mutex is already created.
* @retval TX_INHERIT_ERROR (0x1F) Invalid priority inherit parameter.
* @retval TX_CALLER_ERROR (0x13) Invalid caller of this service.
*/
UINT tx_rwlock_create(TX_RWLOCK* rwlock_ptr, CHAR* name_ptr, UINT inherit);

/**
* Delete a read/write lock.
* @param rwlock_ptr Pointer to a previously created r/w lock.
* @retval TX_SUCCESS(0x00) Successful r/w lock deletion.
* @retval TX_SEMAPHORE_ERROR (0x0C) Invalid internal counting semaphore pointer.
* @retval TX_CALLER_ERROR(0x13) Invalid caller of this service.
*/
UINT tx_rwlock_delete(TX_RWLOCK* rwlock_ptr);

/**
* Obtain access for reading on a read/write lock.
* @param rwlock_ptr Pointer to a previously created r/w lock.
* @param wait_option Defines how the service behaves if the r/w lock is already owned by another thread for writing. Can be TX_NO_WAIT, TX_WAIT_FOREVER or a positive number of ticks to wait for.
* @retval TX_SUCCESS (0x00) Successful r/w lock get operation.
* @retval TX_DELETED (0x01) Internal mutex or counting semaphore was deleted while thread was suspended.
* @retval TX_NOT_AVAILABLE (0x1D) Service was unable to get ownership of the mutex within the specified time to wait.
* @retval TX_NO_INSTANCE (0x0D) Service was unable to retrieve an instance of the counting semaphore (semaphore count is zero within the specified time to wait).
* @retval TX_WAIT_ABORTED (0x1A) Suspension was aborted by another thread, timer, or ISR.
* @retval TX_MUTEX_ERROR (0x1C) Invalid internal mutex pointer.
* @retval TX_SEMAPHORE_ERROR (0x0C) Invalid internal counting semaphore pointer.
* @retval TX_WAIT_ERROR (0x04) A wait option other than TX_NO_WAIT was specified on a call from a non-thread.
* @retval TX_CALLER_ERROR (0x13) Invalid caller of this service.
*/
UINT tx_rwlock_rget(TX_RWLOCK* rwlock_ptr, ULONG wait_option);

/**
* Release access for reading of the read/write lock.
* @param rwlock_ptr Pointer to a previously created r/w lock.
* @retval TX_SUCCESS (0x00) Successful r/w lock release.
* @retval TX_NOT_OWNED (0x1E) R/w lock is not owned by caller.
* @retval TX_MUTEX_ERROR (0x1C) Invalid internal mutex pointer.
* @retval TX_CALLER_ERROR (0x13) Invalid caller of this service.
*/
UINT tx_rwlock_rput(TX_RWLOCK* rwlock_ptr);

/**
* Obtain exclusive ownership for writing on a read/write lock.
* @param rwlock_ptr Pointer to a previously created r/w lock.
* @param wait_option Defines how the service behaves if the r/w lock is already used by another thread for reading or writing. Can be TX_NO_WAIT, TX_WAIT_FOREVER or a positive number of ticks to wait for.
* @retval TX_SUCCESS (0x00) Successful write get of a r/w lock instance.
* @retval TX_DELETED (0x01) Internal mutex or counting semaphore was deleted while thread was suspended.
* @retval TX_NO_INSTANCE (0x0D) Service was unable to get a write access on the r/w lock.
* @retval TX_WAIT_ABORTED (0x1A) Suspension was aborted by another thread, timer, or ISR.
* @retval TX_SEMAPHORE_ERROR (0x0C) Invalid internal counting semaphore pointer.
* @retval TX_WAIT_ERROR (0x04) A wait option other than TX_NO_WAIT was specified on a call from a non-thread.
*/
#ifdef TX_RWLOCK_INLINE_WRITE
#define tx_rwlock_wget(rwlock_ptr, wait_option)  tx_semaphore_get(&((rwlock_ptr)->tx_rwlock_sem_write), wait_option)
#else
UINT tx_rwlock_wget(TX_RWLOCK* rwlock_ptr, ULONG wait_option);
#endif

/**
* Release ownership for writing of the read/write lock.
* @param rwlock_ptr Pointer to a previously created r/w lock.
* @retval TX_SUCCESS(0x00) Successful r/w lock put.
* @retval TX_SEMAPHORE_ERROR (0x0C) Invalid internal counting semaphore pointer.
*/
#ifdef TX_RWLOCK_INLINE_WRITE
#define tx_rwlock_wput(rwlock_ptr)  tx_semaphore_put(&((rwlock_ptr)->tx_rwlock_sem_write))
#else
UINT tx_rwlock_wput(TX_RWLOCK* rwlock_ptr);
#endif

/**
* Prioritize the read/write lock prioritize list for writing access.
* @param rwlock_ptr Pointer to a previously created r/w lock.
* @retval TX_SUCCESS (0x00) Successful semaphore prioritize.
* @retval TX_SEMAPHORE_ERROR (0x0C) Invalid internal counting semaphore pointer.
*/
#ifdef TX_RWLOCK_INLINE_WRITE
#define tx_rwlock_prioritize_write(rwlock_ptr)  tx_semaphore_prioritize(&((rwlock_ptr)->tx_rwlock_sem_write))
#else
UINT tx_rwlock_prioritize_write(TX_RWLOCK* rwlock_ptr);
#endif

#endif
