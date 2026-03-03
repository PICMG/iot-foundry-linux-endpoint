/**
 * @file msgqueue.h
 * @brief Simple message queue implementation using pthread
 * Replacement for Zephyr K_MSGQ for Linux
 */
#ifndef MSGQUEUE_H
#define MSGQUEUE_H

#include <pthread.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>

struct msgqueue {
    void *buffer;
    size_t msg_size;
    size_t max_msgs;
    size_t count;
    size_t read_idx;
    size_t write_idx;
    pthread_mutex_t mutex;
    pthread_cond_t cond_not_empty;
    pthread_cond_t cond_not_full;
};

/**
 * Initialize a message queue
 * @param q Pointer to the message queue structure
 * @param buffer Pointer to buffer storage
 * @param msg_size Size of each message
 * @param max_msgs Maximum number of messages
 * @return 0 on success, -1 on error
 */
static inline int msgqueue_init(struct msgqueue *q, void *buffer, size_t msg_size, size_t max_msgs)
{
    q->buffer = buffer;
    q->msg_size = msg_size;
    q->max_msgs = max_msgs;
    q->count = 0;
    q->read_idx = 0;
    q->write_idx = 0;
    
    if (pthread_mutex_init(&q->mutex, NULL) != 0) {
        return -1;
    }
    if (pthread_cond_init(&q->cond_not_empty, NULL) != 0) {
        pthread_mutex_destroy(&q->mutex);
        return -1;
    }
    if (pthread_cond_init(&q->cond_not_full, NULL) != 0) {
        pthread_mutex_destroy(&q->mutex);
        pthread_cond_destroy(&q->cond_not_empty);
        return -1;
    }
    
    return 0;
}

/**
 * Put a message into the queue (non-blocking)
 * @param q Pointer to the message queue
 * @param msg Pointer to message to enqueue
 * @return 0 on success, -ENOMEM if queue is full
 */
static inline int msgqueue_put(struct msgqueue *q, const void *msg)
{
    pthread_mutex_lock(&q->mutex);
    
    if (q->count >= q->max_msgs) {
        pthread_mutex_unlock(&q->mutex);
        return -ENOMEM;
    }
    
    char *dest = (char *)q->buffer + (q->write_idx * q->msg_size);
    memcpy(dest, msg, q->msg_size);
    q->write_idx = (q->write_idx + 1) % q->max_msgs;
    q->count++;
    
    pthread_cond_signal(&q->cond_not_empty);
    pthread_mutex_unlock(&q->mutex);
    
    return 0;
}

/**
 * Try to get a message from the queue (non-blocking)
 * @param q Pointer to the message queue
 * @param msg Pointer to buffer to receive message
 * @return 0 on success, -1 if queue is empty
 */
static inline int msgqueue_tryget(struct msgqueue *q, void *msg)
{
    pthread_mutex_lock(&q->mutex);
    
    if (q->count == 0) {
        pthread_mutex_unlock(&q->mutex);
        return -1;
    }
    
    char *src = (char *)q->buffer + (q->read_idx * q->msg_size);
    memcpy(msg, src, q->msg_size);
    q->read_idx = (q->read_idx + 1) % q->max_msgs;
    q->count--;
    
    pthread_cond_signal(&q->cond_not_full);
    pthread_mutex_unlock(&q->mutex);
    
    return 0;
}

/**
 * Get a message from the queue (blocking)
 * @param q Pointer to the message queue
 * @param msg Pointer to buffer to receive message
 * @return 0 on success
 */
static inline int msgqueue_get(struct msgqueue *q, void *msg)
{
    pthread_mutex_lock(&q->mutex);
    
    while (q->count == 0) {
        pthread_cond_wait(&q->cond_not_empty, &q->mutex);
    }
    
    char *src = (char *)q->buffer + (q->read_idx * q->msg_size);
    memcpy(msg, src, q->msg_size);
    q->read_idx = (q->read_idx + 1) % q->max_msgs;
    q->count--;
    
    pthread_cond_signal(&q->cond_not_full);
    pthread_mutex_unlock(&q->mutex);
    
    return 0;
}

/**
 * Destroy a message queue
 * @param q Pointer to the message queue
 */
static inline void msgqueue_destroy(struct msgqueue *q)
{
    pthread_mutex_destroy(&q->mutex);
    pthread_cond_destroy(&q->cond_not_empty);
    pthread_cond_destroy(&q->cond_not_full);
}

#endif /* MSGQUEUE_H */
