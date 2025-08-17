//task2

#include "csapp.h"
#include <pthread.h>

#define CONNECTION_BUFFER 1024
#define THREAD_COUNT 4

typedef struct StockNode {
    int id;              // 주식 ID
    int available;       // 남은 수량
    int price;           // 가격
    int readers;         // 읽기 중인 스레드 수
    sem_t rmutex;        
    sem_t wmutex;      
    struct StockNode *left, *right;
} StockNode;

static StockNode *root = NULL;
static const char *DATA_FILE = "stock.txt";

// 새로운 주식 노드 생성
static StockNode *newStock(int id, int count, int pr) {
    StockNode *node = malloc(sizeof(StockNode));
    node->id = id;
    node->available = count;
    node->price = pr;
    node->readers = 0;
    node->left = node->right = NULL;
    Sem_init(&node->rmutex, 0, 1);
    Sem_init(&node->wmutex, 0, 1);
    return node;
}

// BST에 삽입
static StockNode *insertStock(StockNode *t, int id, int count, int pr) {
    if (!t) return newStock(id, count, pr);
    if (id < t->id)
        t->left = insertStock(t->left, id, count, pr);
    else if (id > t->id)
        t->right = insertStock(t->right, id, count, pr);
    return t;
}

// 파일에서 주식 데이터 로드
static void loadData() {
    FILE *fp = fopen(DATA_FILE, "r");
    if (!fp) return;
    int id, cnt, pr;
    while (fscanf(fp, "%d %d %d", &id, &cnt, &pr) == 3) {
        root = insertStock(root, id, cnt, pr);
    }
    fclose(fp);
}

// 중위 순회하면서 문자열에 누적
static void serializeTree(StockNode *t, char *out, int *pos) {
    if (!t) return;
    serializeTree(t->left, out, pos);
    // 리더 진입
    P(&t->rmutex);
    if (++t->readers == 1) P(&t->wmutex);
    V(&t->rmutex);
    // 데이터 읽기 
    *pos += sprintf(out + *pos, "%d %d %d\n", t->id, t->available, t->price);
    // 리더 퇴장
    P(&t->rmutex);
    if (--t->readers == 0) V(&t->wmutex);
    V(&t->rmutex);
    serializeTree(t->right, out, pos);
}

// 메모리상의 데이터 파일에 저장
static void saveData() {
    FILE *fp = fopen(DATA_FILE, "w");
    if (!fp) return;
    char buffer[MAXLINE * 100] = "";
    int position = 0;
    serializeTree(root, buffer, &position);
    fprintf(fp, "%s", buffer);
    fclose(fp);
}

static void sigint_handler(int sig) {
    saveData();
    exit(0);
}

// 트리에서 노드 검색
static StockNode *findStock(StockNode *t, int id) {
    if (!t) return NULL;
    if (id < t->id)      return findStock(t->left, id);
    else if (id > t->id) return findStock(t->right, id);
    else                  return t;
}

// buy
static int handleBuy(int id, int qty) {
    StockNode *n = findStock(root, id);
    if (!n) return 0;
    P(&n->wmutex);
    int ok = (n->available >= qty);
    if (ok) n->available -= qty;
    V(&n->wmutex);
    return ok;
}

//sell
static void handleSell(int id, int qty) {
    StockNode *n = findStock(root, id);
    if (!n) return;
    P(&n->wmutex);
    n->available += qty;
    V(&n->wmutex);
}

typedef struct ConnQueue {
    int fds[CONNECTION_BUFFER];
    int head, tail, size;
    sem_t slots;  // 빈 슬롯 개수
    sem_t items;  // 아이템 개수
    sem_t mutex;  
} ConnQueue;

static ConnQueue queue;

// 큐 초기화
static void initQueue(ConnQueue *q) {
    q->head = q->tail = q->size = 0;
    Sem_init(&q->mutex, 0, 1);
    Sem_init(&q->slots, 0, CONNECTION_BUFFER);
    Sem_init(&q->items, 0, 0);
}

// enqueue
static void enqueue(ConnQueue *q, int fd) {
    P(&q->slots);
    P(&q->mutex);
    q->fds[q->tail] = fd;
    q->tail = (q->tail + 1) % CONNECTION_BUFFER;
    q->size++;
    V(&q->mutex);
    V(&q->items);
}

//dequeue
static int dequeue(ConnQueue *q) {
    int fd;
    P(&q->items);
    P(&q->mutex);
    fd = q->fds[q->head];
    q->head = (q->head + 1) % CONNECTION_BUFFER;
    q->size--;
    V(&q->mutex);
    V(&q->slots);
    return fd;
}

// 클라이언트 요청 처리
static void serve(int connfd) {
    rio_t rio;
    Rio_readinitb(&rio, connfd);
    char req[MAXLINE], rsp[MAXLINE * 100];
    while (1) {
        int n = Rio_readlineb(&rio, req, MAXLINE);
        if (n <= 0) break; 
        printf("server received %d bytes on fd %d\n", n, connfd);
        if (!strcmp(req, "show\n")) {
            rsp[0] = '\0';
            serializeTree(root, rsp, &(int){0});
            Rio_writen(connfd, rsp, MAXLINE);
        } else if (!strncmp(req, "buy", 3)) {
            int id, num; sscanf(req + 4, "%d %d", &id, &num);
            if (handleBuy(id, num)) {
                Rio_writen(connfd, "[buy] success\n", MAXLINE);
                saveData();  
            } else {
                Rio_writen(connfd, "Not enough left stocks\n", MAXLINE);
            }
        } else if (!strncmp(req, "sell", 4)) {
            int id, num; sscanf(req + 5, "%d %d", &id, &num);
            handleSell(id, num);
            Rio_writen(connfd, "[sell] success\n", MAXLINE);
            saveData();  
        } else if (!strcmp(req, "exit\n")) {
            break;
        }
    }
    Close(connfd);
    saveData();  
}

//큐에서 fd 꺼내 처리
static void *worker(void *arg) {
    Pthread_detach(pthread_self());
    while (1) {
        int fd = dequeue(&queue);
        serve(fd);
    }
    return NULL;
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        exit(0);
    }
    Signal(SIGINT, sigint_handler); 

    loadData();
    initQueue(&queue);
    for (int i = 0; i < THREAD_COUNT; i++) {
        pthread_t tid;
        Pthread_create(&tid, NULL, worker, NULL);
    }
    int listenfd = Open_listenfd(argv[1]);
    struct sockaddr_storage cli;
    socklen_t alen;
    while (1) {
        alen = sizeof(cli);
        int connfd = Accept(listenfd, (SA *)&cli, &alen);
        enqueue(&queue, connfd);  
        char host[MAXLINE], serv[MAXLINE];
        Getnameinfo((SA *)&cli, alen, host, MAXLINE, serv, MAXLINE, 0);
        printf("Connected to (%s, %s)\n", host, serv);
    }
    return 0;
}
