#ifndef QUEUE_H
#define QUEUE_H

#define QUEUE_BUFFER_SIZE 20

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

typedef char            int8;
typedef unsigned char   uint8;
typedef unsigned char   bool;
// typedef unsigned int	uint16;

typedef struct {
    uint8 buffer[QUEUE_BUFFER_SIZE + 1];
    int head;
    int tail;
} Queue_T;

/*
 * init_queue - 初始化队列
 *
 * @q: 目标队列
 */
void init_queue(Queue_T *q);
/*
 * get_queue_size - 队列长度
 *
 * @q: 目标队列
 */
int get_queue_size(Queue_T *q);
/*
 * is_queue_empty - 查询队列是否为空
 *
 * @q: 目标队列
 * return: TRUE-空
 */
bool is_queue_empty(Queue_T *q);
/*
 * is_queue_full - 查询队列是否full
 *
 * @q: 目标队列
 * return: TRUE-满
 */
bool is_queue_full(Queue_T *q);
/*
 * enqueue - 将数据插入队尾
 *
 * @q: 目标队列
 * @data: 数据
 * return: TRUE-成功插入队列
 */
bool enqueue(Queue_T *q, uint8 data);
/*
 * dequeue - 队首数据出队
 *
 * @q: 目标队列
 * @re[OUT]: 输出数据
 * return: TRUE-成功出队
 */
void dequeue(Queue_T *q, uint8 *re);

uint8 check_QH(Queue_T *q);

void clear_queue(Queue_T *q);
#endif
