#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "list.h"
#include "hash.h" 
#include "bitmap.h"  
#include <stdint.h>

#ifndef hash_entry
#define hash_entry(HASH_ELEM, STRUCT, MEMBER) \
    ((STRUCT *) ((uint8_t *) (HASH_ELEM) - offsetof(STRUCT, MEMBER)))
#endif

#define MAX_STRUCTS 10


//구조체 선언
typedef enum { LIST, HASH_TABLE, BITMAP } DataType;

typedef struct {
    char name[32];
    DataType type;
    void *data;
} DataStruct;


DataStruct list_struct[MAX_STRUCTS];
int l_cnt = 0;

struct list_item *insertion_order[1000];
int insertion_count = 0;


DataStruct hash_struct[MAX_STRUCTS];
int h_cnt = 0;

struct hash_item {
    int data;
    struct hash_elem elem;
};


DataStruct bitmap_struct[MAX_STRUCTS];
int b_cnt = 0;


// 문자열을 소문자로 변환
void to_lowercase(char *str) {
    for (int i = 0; str[i]; i++) {
        str[i] = tolower(str[i]);
    }
}




// -----------------------LIST--------------------------------------------

// 리스트의 모든 요소를 출력
void list_dump(const struct list *list) {
    struct list_elem *current = list->head.next;
    while (current != &list->tail) {
        struct list_item *item = list_entry(current, struct list_item, elem);
        printf("%d ", item->data);
        current = current->next;
    }
    printf("\n");
}

// 리스트 비교 함수
bool compare_items_list(const struct list_elem *a, const struct list_elem *b, void *aux) {
    struct list_item *item_a = list_entry(a, struct list_item, elem);
    struct list_item *item_b = list_entry(b, struct list_item, elem);
    
    return item_a->data < item_b->data;
}

// 리스트 생성
void create_list(const char *name) {
    if (l_cnt >= MAX_STRUCTS) return;
    list_struct[l_cnt].data = malloc(sizeof(struct list));
    if (list_struct[l_cnt].data == NULL) {
        printf("Memory allocation failed!\n");
        return;
    }
    list_init(list_struct[l_cnt].data);
    strcpy(list_struct[l_cnt].name, name);
    list_struct[l_cnt].type = LIST;
    l_cnt++;
}

// 이름으로 리스트 구조체 포인터 반환
struct list* get_list_by_name(const char *name) {
    for (int i = 0; i < l_cnt; i++) {
        if (strcmp(list_struct[i].name, name) == 0) {
            return list_struct[i].data;
        }
    }
    return NULL;
}

// 리스트 앞에 값 추가
void list_push_front_by_name(const char *name, int value) {
    struct list *lst = get_list_by_name(name);
    if (lst) {
        struct list_item *item = malloc(sizeof(struct list_item));
        item->data = value;
        list_push_front(lst, &item->elem);
    } else {
        printf("List not found!\n");
    }
}

// 리스트 뒤에 값 추가
void list_push_back_by_name(const char *name, int value) {
    struct list *lst = get_list_by_name(name);
    if (lst) {
        struct list_item *item = malloc(sizeof(struct list_item));
        item->data = value;
        list_push_back(lst, &item->elem);
    } else {
        printf("List not found!\n");
    }
}

// 리스트 앞에서 요소 제거
void list_pop_front_by_name(const char *name) {
    struct list *lst = get_list_by_name(name);
    if (lst && !list_empty(lst)) {
        struct list_elem *elem = list_pop_front(lst);
        free(list_entry(elem, struct list_item, elem));
    } else {
        printf("List is empty or not found!\n");
    }
}

// 리스트 뒤에서 요소 제거
void list_pop_back_by_name(const char *name) {
    struct list *lst = get_list_by_name(name);
    if (lst && !list_empty(lst)) {
        struct list_elem *elem = list_pop_back(lst);
        free(list_entry(elem, struct list_item, elem));
    } else {
        printf("List is empty or not found!\n");
    }
}

// 리스트 맨 앞 값 출력
void list_front_by_name(const char *name) {
    struct list *lst = get_list_by_name(name);
    if (lst && !list_empty(lst)) {
        struct list_elem *elem = list_front(lst);
        struct list_item *item = list_entry(elem, struct list_item, elem);
        printf("%d\n", item->data);
    } else {
        printf("List is empty or not found!\n");
    }
}

// 리스트 맨 뒤 값 출력
void list_back_by_name(const char *name) {
    struct list *lst = get_list_by_name(name);
    if (lst && !list_empty(lst)) {
        struct list_elem *elem = list_back(lst);
        struct list_item *item = list_entry(elem, struct list_item, elem);
        printf("%d\n", item->data);
    } else {
        printf("List is empty or not found!\n");
    }
}

// 리스트에 정렬된 순서로 삽입
void list_insert_ordered_by_name(const char *name, int value) {
    struct list *lst = get_list_by_name(name);
    if (lst) {
        struct list_item *item = malloc(sizeof(struct list_item));
        item->data = value;
        list_insert_ordered(lst, &item->elem, compare_items_list, NULL);
    } else {
        printf("List not found!\n");
    }
}

// 리스트가 비었는지 확인
void list_empty_by_name(const char *name) {
    struct list *lst = get_list_by_name(name);
    if (lst) {
        printf("%s\n", list_empty(lst) ? "true" : "false");
    } else {
        printf("List not found!\n");
    }
}

// 리스트 크기 출력
void list_size_by_name(const char *name) {
    struct list *lst = get_list_by_name(name);
    if (lst) {
        printf("%zu\n", list_size(lst));
    } else {
        printf("List not found!\n");
    }
}

// 리스트의 특정 위치에 값 삽입
void list_insert_at(struct list *lst, int idx, int value) {
    struct list_elem *e = list_nth(lst, idx); 
    struct list_item *item = malloc(sizeof(struct list_item));
    item->data = value;

    if (idx < 0 || idx > list_size(lst)) {
        printf("Invalid index!\n");
        free(item);
        return;
    }

    if (idx == list_size(lst)) {
        list_push_back(lst, &item->elem);
    } else {
        list_insert(e, &item->elem);
    }
}

// 리스트에서 인덱스로 요소 제거
void list_remove_by_idx(const char *name, int idx) {
    struct list *lst = get_list_by_name(name);
    if (lst) {
        struct list_elem *e = list_nth(lst, idx);
        if (e != NULL) {
            struct list_item *item = list_entry(e, struct list_item, elem);
            list_remove(e);
            free(item);
        } else {
            printf("Invalid index!\n");
        }
    } else {
        printf("List not found!\n");
    }
}

// 리스트 뒤집기
void list_reverse_by_name(const char *name) {
    struct list *lst = get_list_by_name(name);
    if (lst) {
        list_reverse(lst);
    } else {
        printf("List not found!\n");
    }
}

// 리스트 무작위 섞기
void list_shuffle_by_name(const char *name) {
    struct list *lst = get_list_by_name(name);
    if (lst) {
        list_shuffle(lst);
    } else {
        printf("List not found!\n");
    }
}

// 리스트 정렬
void list_sort_by_name(const char *name) {
    struct list *lst = get_list_by_name(name);
    if (lst) {
        list_sort(lst, compare_items_list, NULL);
    } else {
        printf("List not found!\n");
    }
}

// 다른 리스트에서 일부 요소를 잘라와 삽입
void list_splice_by_name(const char *name, int idx, const char *name2, int start_idx, int end_idx) {
    struct list *lst = get_list_by_name(name);
    struct list *lst2 = get_list_by_name(name2);
    
    if (lst && lst2) {
        // list2에서 start_idx와 end_idx에 해당하는 요소들 찾기
        struct list_elem *e = list_begin(lst2);
        for (int i = 0; i < start_idx; i++) {
            e = list_next(e);
        }
        struct list_elem *start = e;
        
        for (int i = start_idx; i < end_idx; i++) {
            e = list_next(e);
        }
        struct list_elem *end = e;
        
        if (idx == list_size(lst)) {
            list_splice(list_tail(lst), start, end);
        } else {
            struct list_elem *insert_point = list_nth(lst, idx);
            list_splice(insert_point, start, end);
        }
    } else {
        printf("List not found!\n");
    }
}

// 리스트에서 두 인덱스의 요소를 교환
void list_swap_by_idx(const char *name, int idx1, int idx2) {
    struct list *lst = get_list_by_name(name);
    if (lst) {
        struct list_elem *e1 = list_nth(lst, idx1);
        struct list_elem *e2 = list_nth(lst, idx2);

        list_swap(e1, e2);
    } else {
        printf("List not found!\n");
    }
}

// 중복 요소 제거
void list_unique_by_name(const char *name, const char *duplicate_name) {
    struct list *lst = get_list_by_name(name); 
    struct list *duplicates = duplicate_name ? get_list_by_name(duplicate_name) : NULL;

    if (lst == NULL) {
        printf("List not found!\n");
        return;
    }

    if (duplicates) { 
        list_unique(lst, duplicates, compare_items_list, NULL);
    } else {
        list_unique(lst, NULL, compare_items_list, NULL);
    }
}





// -----------------------HASH--------------------------------------------



// 이름으로 해시 테이블 포인터 반환
struct hash* get_hash_by_name(const char *name) {
    for (int i = 0; i < h_cnt; i++) {
        if (strcmp(hash_struct[i].name, name) == 0) {
            return (struct hash *)hash_struct[i].data;
        }
    }
    return NULL;
}

// 해시 테이블 요소 출력
void hash_dump_by_name(const char *name) {
    struct hash *h = get_hash_by_name(name);
    if (!h) return;

    struct hash_iterator it;
    hash_first(&it, h);
    while (hash_next(&it)) {
        struct hash_item *item = hash_entry(hash_cur(&it), struct hash_item, elem);
        printf("%d ", item->data);
    }
    printf("\n");
}

// 해시 함수 정의
unsigned my_hash_func(const struct hash_elem *e, void *aux) {
    struct hash_item *item = hash_entry(e, struct hash_item, elem);
    return hash_int(item->data);
}

// 해시 equal 비교 함수
bool hash_equal_check(const struct hash_elem *a, const struct hash_elem *b, void *aux) {
    struct hash_item *item_a = hash_entry(a, struct hash_item, elem);
    struct hash_item *item_b = hash_entry(b, struct hash_item, elem);
    return item_a->data != item_b->data; 
}

// 해시 요소 메모리 해제 함수
void free_hash_elem(struct hash_elem *e, void *aux) {
    struct hash_item *item = hash_entry(e, struct hash_item, elem);
    free(item);
}

// 해시 테이블 생성
void create_hashtable(const char *name) {
    if (h_cnt >= MAX_STRUCTS) return;

    struct hash *h = malloc(sizeof(struct hash));
    if (!h) return;

    hash_init(h, my_hash_func, hash_equal_check, NULL);
    strcpy(hash_struct[h_cnt].name, name);
    hash_struct[h_cnt].type = HASH_TABLE;
    hash_struct[h_cnt].data = h;
    h_cnt++;
}

// 해시 테이블에 값 삽입
void hash_insert_by_name(const char *name, int value) {
    struct hash *h = get_hash_by_name(name);
    if (!h) return;

    struct hash_item *item = malloc(sizeof(struct hash_item));
    item->data = value;

    struct hash_elem *old = hash_insert(h, &item->elem);
    if (old != NULL) {
        free(item);
        return;
    }
}

// 해시 테이블 요소를 정렬하여 출력
void hash_dump(struct hash *h) {
    struct hash_iterator it;
    hash_first(&it, h);

    struct hash_item *items[1000];
    int count = 0;

    while (hash_next(&it)) {
        struct hash_item *item = hash_entry(hash_cur(&it), struct hash_item, elem);
        items[count++] = item;
    }

    for (int i = 0; i < count - 1; ++i) {
        for (int j = i + 1; j < count; ++j) {
            if (items[i]->data > items[j]->data) {
                struct hash_item *tmp = items[i];
                items[i] = items[j];
                items[j] = tmp;
            }
        }
    }

    for (int i = 0; i < count; i++) {
        printf("%d ", items[i]->data);
    }
    printf("\n");
}

// 해시 테이블 요소 제곱 처리
void square(struct hash_elem *e, void *aux) {
    struct hash_item *item = hash_entry(e, struct hash_item, elem);
    item->data = item->data * item->data;
}

// 해시 테이블 요소 세제곱 처리
void triple(struct hash_elem *e, void *aux) {
    struct hash_item *item = hash_entry(e, struct hash_item, elem);
    item->data = item->data * item->data * item->data;
}

// 해시 테이블에 연산 적용 (square, triple)
void hash_apply_by_name(const char *name, const char *operation) {
    struct hash *h = get_hash_by_name(name);
    if (!h) return;

    if (strcmp(operation, "square") == 0) {
        hash_apply(h, square);
    } else if (strcmp(operation, "triple") == 0) {
        hash_apply(h, triple);
    }
}

// 해시 테이블에서 특정 값 삭제
void hash_delete_by_name(const char *name, int value) {
    struct hash *h = get_hash_by_name(name);
    if (!h) return;

    struct hash_item temp;
    temp.data = value;

    struct hash_elem *e = hash_delete(h, &temp.elem);
    if (e != NULL) {
        struct hash_item *item = hash_entry(e, struct hash_item, elem);
        free(item);
    }
}

// 해시 테이블이 비었는지 확인
void hash_empty_by_name(const char *name) {
    struct hash *h = get_hash_by_name(name);
    if (!h) return;

    printf("%s\n", hash_empty(h) ? "true" : "false");
}

// 해시 테이블 크기 출력
void hash_size_by_name(const char *name) {
    struct hash *h = get_hash_by_name(name);
    if (!h) return;

    printf("%zu\n", hash_size(h));
}

// 해시 테이블 초기화
void hash_clear_by_name(const char *name) {
    struct hash *h = get_hash_by_name(name);
    if (!h) return;

    hash_clear(h, free_hash_elem);
}

// 해시 테이블에서 값 검색
void hash_find_by_name(const char *name, int value) {
    struct hash *h = get_hash_by_name(name);
    if (!h) return;

    struct hash_item temp;
    temp.data = value;

    struct hash_elem *e = hash_find(h, &temp.elem);
    if (e != NULL) {
        struct hash_item *found = hash_entry(e, struct hash_item, elem);
        printf("%d\n", found->data);
    }
}

// 해시 테이블 값 교체
void hash_replace_by_name(const char *name, int value) {
    struct hash *h = get_hash_by_name(name);
    if (!h) return;

    struct hash_item *item = malloc(sizeof(struct hash_item));
    item->data = value;

    struct hash_elem *old = hash_replace(h, &item->elem);
    if (old != NULL) {
        struct hash_item *old_item = hash_entry(old, struct hash_item, elem);
        free(old_item);
    }
}






// -----------------------BITMAP--------------------------------------------



// 이름으로 비트맵 구조체 포인터 반환
struct bitmap* get_bitmap_by_name(const char *name) {
    for (int i = 0; i < b_cnt; i++) {
        if (strcmp(bitmap_struct[i].name, name) == 0 && bitmap_struct[i].type == BITMAP) {
            return bitmap_struct[i].data;
        }
    }
    return NULL;
}

// 비트맵을 0과 1로 출력
void bitmap_dump_by_name(const char *name) {
    struct bitmap *bm = get_bitmap_by_name(name);
    if (!bm) return;

    size_t size = bitmap_size(bm);
    for (size_t i = 0; i < size; i++) {
        printf("%d", bitmap_test(bm, i) ? 1 : 0);
    }
    printf("\n");
}

// 비트맵 생성
void create_bitmap(const char *name, int bit_cnt) {
    if (b_cnt >= MAX_STRUCTS) return;

    bitmap_struct[b_cnt].data = bitmap_create(bit_cnt);
    if (bitmap_struct[b_cnt].data == NULL) {
        printf("Memory allocation failed!\n");
        return;
    }

    strcpy(bitmap_struct[b_cnt].name, name);
    bitmap_struct[b_cnt].type = BITMAP;
    b_cnt++;
}


// 특정 위치 비트 마크
void bitmap_mark_by_name(const char *name, size_t idx) {
    struct bitmap *bm = get_bitmap_by_name(name);
    if (bm && idx < bitmap_size(bm)) {
        bitmap_mark(bm, idx);
    }
}

// 특정 위치 비트 리셋
void bitmap_reset_by_name(const char *name, size_t idx) {
    struct bitmap *bm = get_bitmap_by_name(name);
    if (bm) bitmap_reset(bm, idx);
}

// 구간 전체가 true인지 확인
void bitmap_all_by_name(const char *name, int start, int count) {
    struct bitmap *bm = get_bitmap_by_name(name);
    if (bm) {
        printf("%s\n", bitmap_all(bm, start, count) ? "true" : "false");
    }
}

// 구간 중 하나라도 true인지 확인
void bitmap_any_by_name(const char *name, int start, int count) {
    struct bitmap *bm = get_bitmap_by_name(name);
    if (bm) {
        printf("%s\n", bitmap_any(bm, start, count) ? "true" : "false");
    }
}

// 구간이 특정 값들로 채워졌는지 확인
void bitmap_contains_by_name(const char *name, int start, int count, bool value) {
    struct bitmap *bm = get_bitmap_by_name(name);
    if (bm) {
        bool result = bitmap_contains(bm, start, count, value);
        printf("%s\n", result ? "true" : "false");
    }
}

// 구간에서 특정 값의 개수 세기
void bitmap_count_by_name(const char *name, int start, int count, bool value) {
    struct bitmap *bm = get_bitmap_by_name(name);
    if (bm) {
        printf("%zu\n", bitmap_count(bm, start, count, value));
    }
}

// 비트맵을 16진수로 출력
void bitmap_hex_dump_by_name(const char *name) {
    struct bitmap *bm = get_bitmap_by_name(name);
    if (!bm) return;

    size_t bit_cnt = bitmap_size(bm);
    size_t byte_cnt = (bit_cnt + 7) / 8;

    if (byte_cnt < 4) byte_cnt = 4;

    uint8_t *data = (uint8_t *) bitmap_get_raw(bm);

    printf("00000000  ");
    for (size_t i = 0; i < byte_cnt; i++) {
        printf("%02x", data[i]);
        if ((i + 1) % 1 == 0) printf(" ");
    }
    printf("\n");
}

// 전체 비트값을 일괄 설정
void bitmap_set_all_by_name(const char *name, bool value) {
    struct bitmap *bm = get_bitmap_by_name(name);
    if (bm) {
        bitmap_set_all(bm, value);
    }
}

// 특정 비트를 반전
void bitmap_flip_by_name(const char *name, size_t idx) {
    struct bitmap *bm = get_bitmap_by_name(name);
    if (bm && idx < bitmap_size(bm)) {
        bitmap_flip(bm, idx);
    }
}

// 구간이 모두 false인지 확인
void bitmap_none_by_name(const char *name, int start, int count) {
    struct bitmap *bm = get_bitmap_by_name(name);
    if (bm) {
        bool result = bitmap_none(bm, start, count);
        printf("%s\n", result ? "true" : "false");
    }
}

// 조건을 만족하는 구간을 찾아 반전
void bitmap_scan_and_flip_by_name(const char *name, int start, int count, bool value) {
    struct bitmap *bm = get_bitmap_by_name(name);
    if (!bm) return;

    size_t idx = bitmap_scan_and_flip(bm, start, count, value);
    printf("%zu\n", idx);
}

// 조건을 만족하는 구간 찾기
void bitmap_scan_by_name(const char *name, int start, int count, bool value) {
    struct bitmap *bm = get_bitmap_by_name(name);
    if (!bm) return;

    size_t idx = bitmap_scan(bm, start, count, value);
    printf("%zu\n", idx);
}

// 특정 비트를 설정
void bitmap_set_by_name(const char *name, size_t idx, bool value) {
    struct bitmap *bm = get_bitmap_by_name(name);
    if (bm && idx < bitmap_size(bm)) {
        bitmap_set(bm, idx, value);
    }
}

// 여러 비트를 한꺼번에 설정
void bitmap_set_multiple_by_name(const char *name, int start, int count, bool value) {
    struct bitmap *bm = get_bitmap_by_name(name);
    if (bm && start + count <= bitmap_size(bm)) {
        bitmap_set_multiple(bm, start, count, value);
    }
}

// 비트맵 크기 출력
void bitmap_size_by_name(const char *name) {
    struct bitmap *bm = get_bitmap_by_name(name);
    if (bm) {
        printf("%zu\n", bitmap_size(bm));
    }
}

// 특정 비트가 설정되었는지 확인
void bitmap_test_by_name(const char *name, size_t idx) {
    struct bitmap *bm = get_bitmap_by_name(name);
    if (bm && idx < bitmap_size(bm)) {
        printf("%s\n", bitmap_test(bm, idx) ? "true" : "false");
    }
}

// -------------------------------------------------------------------



// 구조체 삭제
void delete_struct(const char *name) {
    // 리스트
    for (int i = 0; i < l_cnt; i++) {
        if (strcmp(list_struct[i].name, name) == 0 && list_struct[i].type == LIST) {
            free(list_struct[i].data);
            for (int j = i; j < l_cnt - 1; j++) {
                list_struct[j] = list_struct[j + 1];
            }
            l_cnt--;
            return;
        }
    }

    // 해시 테이블
    for (int i = 0; i < h_cnt; i++) {
        if (strcmp(hash_struct[i].name, name) == 0 && hash_struct[i].type == HASH_TABLE) {
            struct hash *h = (struct hash *)hash_struct[i].data;
            hash_clear(h, free_hash_elem); 
            free(hash_struct[i].data);     
            for (int j = i; j < h_cnt - 1; j++) {
                hash_struct[j] = hash_struct[j + 1];
            }
            h_cnt--;
            return;
        }
    }

    // 비트맵
    for (int i = 0; i < b_cnt; i++) {
        if (strcmp(bitmap_struct[i].name, name) == 0 && bitmap_struct[i].type == BITMAP) {
            bitmap_destroy((struct bitmap *)bitmap_struct[i].data);
            for (int j = i; j < b_cnt - 1; j++) {
                bitmap_struct[j] = bitmap_struct[j + 1];
            }
            b_cnt--;
            return;
        }
    }
}

// 데이터 구조의 내용을 출력
void dumpdata(const char *name) {
    //리스트
    for (int i = 0; i < l_cnt; i++) {
        if (strcmp(list_struct[i].name, name) == 0) {
            list_dump(list_struct[i].data);
            return;
        }
    }

    // 해시 테이블
    for (int i = 0; i < h_cnt; i++) {
        if (strcmp(hash_struct[i].name, name) == 0) {
            hash_dump_by_name(name);
            return;
        }
    }

    // 비트맵
    for (int i = 0; i < b_cnt; i++) {
        if (strcmp(bitmap_struct[i].name, name) == 0) {
            bitmap_dump_by_name(name); 
            return;
        }
    }
}







int main() {
    char command[64], name[32], name2[32];
    int bit_cnt, value, idx1, idx2, start_idx, end_idx;

    while (1) {
        if (scanf("%s", command) == EOF) break;
        to_lowercase(command);

        if (strcmp(command, "create") == 0) {
            char type[32];
            scanf("%s %s", type, name);
            to_lowercase(type);
            if (strcmp(type, "list") == 0) create_list(name);
            else if (strcmp(type, "hashtable") == 0) {
                create_hashtable(name); 
            }
            else if (strcmp(type, "bitmap") == 0) {
                scanf("%d", &bit_cnt);
                create_bitmap(name, bit_cnt);
            }


        // -----------------------LIST------------------------


            } else if (strcmp(command, "list_push_front") == 0) {
                scanf("%s %d", name, &value);
                to_lowercase(name);
                list_push_front_by_name(name, value);
            } else if (strcmp(command, "list_push_back") == 0) {
                scanf("%s %d", name, &value);
                to_lowercase(name);
                list_push_back_by_name(name, value);
            } else if (strcmp(command, "list_pop_front") == 0) {
                scanf("%s", name);
                to_lowercase(name);
                list_pop_front_by_name(name);
            } else if (strcmp(command, "list_pop_back") == 0) {
                scanf("%s", name);
                to_lowercase(name);
                list_pop_back_by_name(name);
            } else if (strcmp(command, "list_front") == 0) {
                scanf("%s", name);
                to_lowercase(name);
                list_front_by_name(name);
            } else if (strcmp(command, "list_back") == 0) {
                scanf("%s", name);
                to_lowercase(name);
                list_back_by_name(name);
            } else if (strcmp(command, "list_insert_ordered") == 0) {
                scanf("%s %d", name, &value);
                to_lowercase(name);
                list_insert_ordered_by_name(name, value);
            } else if (strcmp(command, "list_empty") == 0) {
                scanf("%s", name);
                to_lowercase(name);
                list_empty_by_name(name);
            } else if (strcmp(command, "list_size") == 0) {
                scanf("%s", name);
                to_lowercase(name);
                list_size_by_name(name);
            } else if (strcmp(command, "list_remove") == 0) {
                scanf("%s %d", name, &idx1);  // value 대신 idx를 받음
                to_lowercase(name);
                list_remove_by_idx(name, idx1);
            } else if (strcmp(command, "list_reverse") == 0) {
                scanf("%s", name);
                to_lowercase(name);
                list_reverse_by_name(name);
            } else if (strcmp(command, "list_shuffle") == 0) {
                scanf("%s", name);
                to_lowercase(name);
                list_shuffle_by_name(name);
            } else if (strcmp(command, "list_sort") == 0) {
                scanf("%s", name);
                to_lowercase(name);
                list_sort_by_name(name);
            } else if (strcmp(command, "list_splice") == 0) {
                scanf("%s %d %s %d %d", name, &idx1, name2, &start_idx, &end_idx);
                to_lowercase(name);
                list_splice_by_name(name, idx1, name2, start_idx, end_idx);
            } else if (strcmp(command, "list_swap") == 0) {
                scanf("%s %d %d", name, &idx1, &idx2);
                to_lowercase(name);
                list_swap_by_idx(name, idx1, idx2);
            } else if (strcmp(command, "list_unique") == 0) {
                char line[128];
                if (fgets(line, sizeof(line), stdin)) {
                    char *token = strtok(line, " \n");
                    if (token != NULL) {
                        strcpy(name, token);
                        to_lowercase(name);
                        token = strtok(NULL, " \n");
                        if (token != NULL) {
                            strcpy(name2, token);
                            to_lowercase(name2);
                            list_unique_by_name(name, name2);
                        } else {
                            list_unique_by_name(name, NULL);
                        }
                    }
                }
            } else if (strcmp(command, "list_max") == 0) {
                scanf("%s", name);
                to_lowercase(name);
                struct list *lst = get_list_by_name(name);
                if (lst) {
                    struct list_elem *max_elem = list_max(lst, compare_items_list, NULL);
                    if (max_elem) {
                        int max_value = list_entry(max_elem, struct list_item, elem)->data;
                        printf("%d\n", max_value);
                        }
                    } else printf("List not found!\n");
                
            } else if (strcmp(command, "list_insert") == 0) {
                scanf("%s %d %d", name, &idx1, &value);
                to_lowercase(name);
                struct list *lst = get_list_by_name(name);
                if (lst) {
                    list_insert_at(lst, idx1, value);
                } else {
                    printf("List not found!\n");
                }
            } else if (strcmp(command, "list_min") == 0) {
                scanf("%s", name);
                to_lowercase(name);
                struct list *lst = get_list_by_name(name);
                if (lst) {
                    struct list_elem *min_elem = list_min(lst, compare_items_list, NULL);
                    if (min_elem) {
                        int min_value = list_entry(min_elem, struct list_item, elem)->data;
                        printf("%d\n", min_value);
                    }
                } else {
                    printf("List not found!\n");
                }
            }


        // -----------------------HASH------------------------
        
            else if (strcmp(command, "hash_insert") == 0) {
                scanf("%s %d", name, &value);
                to_lowercase(name);
                hash_insert_by_name(name, value);
            } else if (strcmp(command, "hash_apply") == 0) {
                char operation[32];
                scanf("%s %s", name, operation);
                to_lowercase(name);
                to_lowercase(operation);
                hash_apply_by_name(name, operation);
            } else if (strcmp(command, "hash_delete") == 0) {
                int value;
                scanf("%s %d", name, &value);
                to_lowercase(name);
                hash_delete_by_name(name, value);
            } else if (strcmp(command, "hash_empty") == 0) {
                scanf("%s", name);
                to_lowercase(name);
                hash_empty_by_name(name);
            }
            else if (strcmp(command, "hash_size") == 0) {
                scanf("%s", name);
                to_lowercase(name);
                hash_size_by_name(name);
            }
            else if (strcmp(command, "hash_clear") == 0) {
                scanf("%s", name);
                to_lowercase(name);
                hash_clear_by_name(name);
            } else if (strcmp(command, "hash_find") == 0) {
                int value;
                scanf("%s %d", name, &value);
                to_lowercase(name);
                hash_find_by_name(name, value);
            } else if (strcmp(command, "hash_replace") == 0) {
                int value;
                scanf("%s %d", name, &value);
                to_lowercase(name);
                hash_replace_by_name(name, value);
            }


        // -----------------------BITMAP------------------------

            else if (strcmp(command, "bitmap_all") == 0) {
                int start, count;
                scanf("%s %d %d", name, &start, &count);
                to_lowercase(name);
                bitmap_all_by_name(name, start, count);
            } else if (strcmp(command, "bitmap_any") == 0) {
                int start, count;
                scanf("%s %d %d", name, &start, &count);
                to_lowercase(name);
                bitmap_any_by_name(name, start, count);
            } else if (strcmp(command, "bitmap_contains") == 0) {
                int start, count;
                char val_str[16];
                scanf("%s %d %d %s", name, &start, &count, val_str);
                to_lowercase(name);
                to_lowercase(val_str);

                bool value;
                if (strcmp(val_str, "true") == 0) value = true;
                else if (strcmp(val_str, "false") == 0) value = false;
                else {
                    printf("Invalid value: must be true or false\n");
                    continue;
                }

                bitmap_contains_by_name(name, start, count, value);
            } else if (strcmp(command, "bitmap_count") == 0) {
                int start, count;
                char val_str[16];
                scanf("%s %d %d %s", name, &start, &count, val_str);
                to_lowercase(name);
                to_lowercase(val_str);

                bool value;
                if (strcmp(val_str, "true") == 0) value = true;
                else if (strcmp(val_str, "false") == 0) value = false;
                else {
                    printf("Invalid value: must be true or false\n");
                    continue;
                }
                bitmap_count_by_name(name, start, count, value);
            } else if (strcmp(command, "bitmap_dump") == 0) {
                scanf("%s", name);
                to_lowercase(name);
                bitmap_hex_dump_by_name(name);
            } else if (strcmp(command, "bitmap_mark") == 0) {
                size_t idx;
                scanf("%s %zu", name, &idx); 
                to_lowercase(name);
                bitmap_mark_by_name(name, idx);
            } else if (strcmp(command, "bitmap_expand") == 0) {
                int more_bits;
                scanf("%s %d", name, &more_bits);
                to_lowercase(name);
                struct bitmap *bm = get_bitmap_by_name(name);
                if (bm) {
                    struct bitmap *new_bm = bitmap_expand(bm, more_bits);
                    if (new_bm) {
                        for (int i = 0; i < b_cnt; i++) {
                            if (strcmp(bitmap_struct[i].name, name) == 0) {
                                bitmap_struct[i].data = new_bm;
                                break;
                            }
                        }
                    }
                }
            } else if (strcmp(command, "bitmap_set_all") == 0) {
                char value_str[16];
                scanf("%s %s", name, value_str);
                to_lowercase(name);
                to_lowercase(value_str);

                bool value;
                if (strcmp(value_str, "true") == 0) value = true;
                else if (strcmp(value_str, "false") == 0) value = false;
                else {
                    printf("Invalid value: must be true or false\n");
                    continue;
                }
                bitmap_set_all_by_name(name, value);
            } else if (strcmp(command, "bitmap_flip") == 0) {
                size_t idx;
                scanf("%s %zu", name, &idx);
                to_lowercase(name);
                bitmap_flip_by_name(name, idx);
            } else if (strcmp(command, "bitmap_none") == 0) {
                int start, count;
                scanf("%s %d %d", name, &start, &count);
                to_lowercase(name);
                bitmap_none_by_name(name, start, count);
            } else if (strcmp(command, "bitmap_reset") == 0) {
                size_t idx;
                scanf("%s %zu", name, &idx);
                to_lowercase(name);
                bitmap_reset_by_name(name, idx);
            } else if (strcmp(command, "bitmap_scan_and_flip") == 0) {
                int start, count;
                char val_str[16];
                scanf("%s %d %d %s", name, &start, &count, val_str);
                to_lowercase(name);
                to_lowercase(val_str);

                bool value;
                if (strcmp(val_str, "true") == 0) value = true;
                else if (strcmp(val_str, "false") == 0) value = false;
                else {
                    printf("Invalid value: must be true or false\n");
                    continue;
                }
                bitmap_scan_and_flip_by_name(name, start, count, value);
            } else if (strcmp(command, "bitmap_scan") == 0) {
                int start, count;
                char val_str[16];
                scanf("%s %d %d %s", name, &start, &count, val_str);
                to_lowercase(name);
                to_lowercase(val_str);

                bool value;
                if (strcmp(val_str, "true") == 0) value = true;
                else if (strcmp(val_str, "false") == 0) value = false;
                else {
                    printf("Invalid value: must be true or false\n");
                    continue;
                }

                bitmap_scan_by_name(name, start, count, value);
            } else if (strcmp(command, "bitmap_set") == 0) {
                size_t idx;
                char val_str[16];
                scanf("%s %zu %s", name, &idx, val_str);
                to_lowercase(name);
                to_lowercase(val_str);

                bool value;
                if (strcmp(val_str, "true") == 0) value = true;
                else if (strcmp(val_str, "false") == 0) value = false;
                else {
                    printf("Invalid value: must be true or false\n");
                    continue;
                }

                bitmap_set_by_name(name, idx, value);
            } else if (strcmp(command, "bitmap_set_multiple") == 0) {
                int start, count;
                char val_str[16];
                scanf("%s %d %d %s", name, &start, &count, val_str);
                to_lowercase(name);
                to_lowercase(val_str);

                bool value;
                if (strcmp(val_str, "true") == 0) value = true;
                else if (strcmp(val_str, "false") == 0) value = false;
                else {
                    printf("Invalid value: must be true or false\n");
                    continue;
                }

                bitmap_set_multiple_by_name(name, start, count, value);
            } else if (strcmp(command, "bitmap_size") == 0) {
                scanf("%s", name);
                to_lowercase(name);
                bitmap_size_by_name(name);
            } else if (strcmp(command, "bitmap_test") == 0) {
                size_t idx;
                scanf("%s %zu", name, &idx);
                to_lowercase(name);
                bitmap_test_by_name(name, idx);
            } 
            
            // -----------------------------------------------

            
            else if (strcmp(command, "quit") == 0) {
                        break;
                    } 
            else if (strcmp(command, "delete") == 0) {
                scanf("%s", name);
                to_lowercase(name);
                delete_struct(name);
            } else if (strcmp(command, "dumpdata") == 0) {
                scanf("%s", name);
                to_lowercase(name);
                dumpdata(name);
            }
        }
  
      return 0;
}
