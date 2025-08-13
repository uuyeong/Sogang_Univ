//task1

#include "csapp.h"

typedef struct stock {
    int ID;
    int left_stock;
    int price;
    int readcnt;           // 현재 읽고 있는 reader 수
    sem_t mutex;
    sem_t rw_mutex;
    struct stock *left;
    struct stock *right;
} Stock;


typedef struct {
    int maxfd;
    int maxi;
    int clientfd[FD_SETSIZE];
    rio_t clientrio[FD_SETSIZE];
    fd_set read_set;
    fd_set ready_set;
    int nready;
} pool_t;

Stock *root = NULL;
char *stock_file = "stock.txt";


Stock *create_node(int id, int quantity, int price) {
    Stock *node = Malloc(sizeof(Stock));
    node->ID         = id;
    node->left_stock = quantity;
    node->price      = price;
    node->readcnt    = 0;
    node->left = node->right = NULL;
    Sem_init(&node->mutex,    0, 1);
    Sem_init(&node->rw_mutex, 0, 1);
    return node;
}


Stock *insert_stock(Stock *node, int id, int qty, int price) {
    if (node == NULL)
        return create_node(id, qty, price);
    if (id < node->ID)
        node->left = insert_stock(node->left, id, qty, price);
    else if (id > node->ID)
        node->right = insert_stock(node->right, id, qty, price);
    return node;
}

void load_stocks() {
    FILE *fp = fopen(stock_file, "r");
    if (!fp) return;

    int id, qty, price;
    while (fscanf(fp, "%d %d %d", &id, &qty, &price) == 3) {
        root = insert_stock(root, id, qty, price);
    }
    fclose(fp);
}

void show_tree_buffered(Stock *node, char *buf) {
    if (!node) return;

    show_tree_buffered(node->left, buf);

    /* → Reader Entry */
    P(&node->mutex);
    node->readcnt++;
    if (node->readcnt == 1)
        P(&node->rw_mutex);
    V(&node->mutex);

    /* ← Critical Section (읽기만 함) */
    char temp[MAXLINE];
    sprintf(temp, "%d %d %d\n",
            node->ID, node->left_stock, node->price);
    strcat(buf, temp);

    /* → Reader Exit */
    P(&node->mutex);
    node->readcnt--;
    if (node->readcnt == 0)
        V(&node->rw_mutex);
    V(&node->mutex);

    show_tree_buffered(node->right, buf);
}


void save_stocks() {
    FILE *fp = fopen(stock_file, "w");
    if (!fp) return;
    char buf[MAXLINE * 100] = "";
    show_tree_buffered(root, buf);
    fprintf(fp, "%s", buf);
    fclose(fp);
}

Stock *find_stock(Stock *node, int id) {
    if (!node) return NULL;
    if (id < node->ID)
        return find_stock(node->left, id);
    else if (id > node->ID)
        return find_stock(node->right, id);
    return node;
}

int buy_stock(int id, int amount) {
    Stock *target = find_stock(root, id);
    if (!target) return 0;

    P(&target->rw_mutex);             // writer lock
    int success = 0;
    if (target->left_stock >= amount) {
        target->left_stock -= amount;
        success = 1;
    }
    V(&target->rw_mutex);             // writer unlock
    return success;
}

void sell_stock(int id, int amount) {
    Stock *target = find_stock(root, id);
    if (!target) return;

    P(&target->rw_mutex);
    target->left_stock += amount;
    V(&target->rw_mutex);
}

void handle_client(int connfd, rio_t *rio, int index, pool_t *p) {
    char buf[MAXLINE];
    int n = Rio_readlineb(rio, buf, MAXLINE);

    if (n <= 0) {
        Close(connfd);
        FD_CLR(connfd, &p->read_set);
        p->clientfd[index] = -1;
        save_stocks();
        return;
    }

    printf("server received %d bytes on fd %d\n", n, connfd);

    if (!strcmp(buf, "show\n")) {
        char show_buf[MAXLINE * 100] = "";
        show_tree_buffered(root, show_buf);
        Rio_writen(connfd, show_buf, MAXLINE);
    } else if (!strncmp(buf, "buy", 3)) {
        int id, amount;
        sscanf(buf + 4, "%d %d", &id, &amount);
        int result = buy_stock(id, amount);
        if(result){ //성공한 경우
            Rio_writen(connfd, "[buy] success\n", MAXLINE);
            save_stocks();
        }
        else{ //실패한 경우
            Rio_writen(connfd, "Not enough left stocks\n", MAXLINE);
            //save_stocks();
        }
        
    } else if (!strncmp(buf, "sell", 4)) {
        int id, amount;
        sscanf(buf + 5, "%d %d", &id, &amount);
        sell_stock(id, amount);
        Rio_writen(connfd, "[sell] success\n", MAXLINE);
        save_stocks();
    } else if (!strcmp(buf, "exit\n")) {
        Close(connfd);
        FD_CLR(connfd, &p->read_set);
        p->clientfd[index] = -1;
        save_stocks();
    }
}

void init_pool(int listenfd, pool_t *p) {
    p->maxfd = listenfd;
    p->maxi = -1;
    FD_ZERO(&p->read_set);
    FD_SET(listenfd, &p->read_set);
    for (int i = 0; i < FD_SETSIZE; i++)
        p->clientfd[i] = -1;
}

void add_client(int connfd, struct sockaddr_storage *clientaddr, socklen_t clientlen, pool_t *p) {
    p->nready--;
    for (int i = 0; i < FD_SETSIZE; i++) {
        if (p->clientfd[i] < 0) {
            p->clientfd[i] = connfd;
            Rio_readinitb(&p->clientrio[i], connfd);
            FD_SET(connfd, &p->read_set);
            if (connfd > p->maxfd) p->maxfd = connfd;
            if (i > p->maxi) p->maxi = i;

            char host[MAXLINE], service[MAXLINE];
            Getnameinfo((SA *)clientaddr, clientlen, host, MAXLINE, service, MAXLINE, 0);
            printf("Connected to (%s, %s)\n", host, service);
            return;
        }
    }
    app_error("too many clients");
}

void check_clients(pool_t *p) {
    for (int i = 0; i <= p->maxi && p->nready > 0; i++) {
        int connfd = p->clientfd[i];
        if (connfd >= 0 && FD_ISSET(connfd, &p->ready_set)) {
            p->nready--;
            handle_client(connfd, &p->clientrio[i], i, p);
        }
    }
}

void free_stocks(Stock *node) {
    if (!node) return;
    free_stocks(node->left);
    free_stocks(node->right);
    free(node);
}

void sigint_handler(int sig) {
    save_stocks();    // 메모리 상 트리를 파일에 덮어쓰기
    exit(0);
}

int main(int argc, char **argv) {
    int listenfd, connfd;
    socklen_t clientlen;
    struct sockaddr_storage clientaddr;
    static pool_t pool;

    Signal(SIGINT, sigint_handler);

    if (argc != 2) {
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        exit(0);
    }

    load_stocks();

    listenfd = Open_listenfd(argv[1]);
    init_pool(listenfd, &pool);

    while (1) {
        pool.ready_set = pool.read_set;
        pool.nready = Select(pool.maxfd + 1, &pool.ready_set, NULL, NULL, NULL);

        if (FD_ISSET(listenfd, &pool.ready_set)) {
            clientlen = sizeof(clientaddr);
            connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);
            add_client(connfd, &clientaddr, clientlen, &pool);
            continue;
        }

        check_clients(&pool);
    }

    free_stocks(root);
    return 0;
}
