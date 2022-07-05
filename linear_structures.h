#include <stdio.h>
#include <stdlib.h>
#define MAX_SIZE 101

typedef struct Info {
    char name[10];
    // TODO: Add your own data members
} info;

//=============================================================
// Queue, implemented as a circular array
// Stack, implementede as an array
//=============================================================

// Typedef for the queue
typedef struct Queue {
    info data[MAX_SIZE];
    int head;
    int tail;
} queue;

// Enqueue
void enqueue(queue *q, info data) {
    q->data[q->tail] = data;
    if (q->tail == MAX_SIZE - 1) {
        q->tail = 0;
    } else {
        q->tail++;
    }
}

// Dequeue
info dequeue(queue *q) {
    info data = q->data[q->head];
    if (q->head == MAX_SIZE - 1) {
        q->head = 0;
    } else {
        q->head++;
    }
    return data;
}

// Typedef for the stack
typedef struct Stack {
    info data[MAX_SIZE];
    int top;
} stack;

// Push
void push(stack *s, info data) {
    s->data[s->top] = data;
    if (s->top == MAX_SIZE - 1) {
        printf("Stack overflow\n");
    } else {
        s->top++;
    }
}

// Pop
info pop(stack *s) {
    info data = s->data[s->top - 1];
    if (s->top == 0) {
        printf("Stack underflow\n");
    } else {
        s->top--;
    }
    return data;
}

//==================================================================
// Double Linked List
//==================================================================

// Typedef double-linked list
typedef struct dll_node {
    struct dll_node *prev;
    struct dll_node *next;
    int key;
    info *info;
} dll_node;

// Print the list
void print_list(dll_node *head) {
    dll_node *curr = head;
    while (curr != NULL) {
        printf("%d: %s\n", curr->key, curr->info->name);
        curr = curr->next;
    }
}

// List insert
dll_node *dll_insert(dll_node *head, int key, info *info) {
    dll_node *new_node = (dll_node *)malloc(sizeof(dll_node));
    new_node->key = key;
    new_node->info = info;
    new_node->prev = NULL;
    new_node->next = NULL;
    if (head == NULL) {
        return new_node;
    }
    dll_node *current = head;
    while (current->next != NULL) {
        current = current->next;
    }
    current->next = new_node;
    new_node->prev = current;
    return head;
}

// List insert at the beginning
dll_node *dll_head_insert(dll_node *head, int key, info *info) {
    dll_node *new_node = (dll_node *)malloc(sizeof(dll_node));
    new_node->key = key;
    new_node->info = info;
    new_node->prev = NULL;
    new_node->next = NULL;
    if (head == NULL) {
        return new_node;
    }
    new_node->next = head;
    head->prev = new_node;
    return new_node;
}

// List search
dll_node *dll_search(dll_node *head, int key) {
    dll_node *current = head;
    while (current != NULL) {
        if (current->key == key) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

// List delete
dll_node *dll_delete(dll_node *head, int key) {
    dll_node *current = head;
    while (current != NULL) {
        if (current->key == key) {
            if (current->prev != NULL) {
                current->prev->next = current->next;
            }
            if (current->next != NULL) {
                current->next->prev = current->prev;
            }
            free(current);
            return head;
        }
        current = current->next;
    }
    return head;
}

//==================================================================
// Hash Table
//==================================================================
#define CONST_A 0.61803398874989484820458683436564  // golden ratio complement
#define M 701
#define TABLE_SIZE 50

// Typedef hash table
typedef dll_node *hash_table[TABLE_SIZE];

// Initialize hash table
hash_table *hash_init() {
    hash_table *hash_head = (hash_table *)malloc(sizeof(dll_node) * TABLE_SIZE);
    for (int i = 0; i < TABLE_SIZE; i++) {
        (*hash_head)[i] = NULL;
    }
    return hash_head;
}

// Hash function division method
int d_h(int key) {
    return (key % M) % TABLE_SIZE;
}

// Hash function multiplication method
int m_h(int key) {
    float temp = (float)key;
    temp *= CONST_A;
    key = temp * M;
    return key % TABLE_SIZE;
}

// Hash insert, division and multiplication method
hash_table *d_hash_insert(hash_table *table, int key, info *info) {
    int index = d_h(key);
    printf("Index is %d\n", index);
    (*table)[index] = dll_insert((*table)[index], key, info);
    return table;
}

hash_table *m_hash_insert(hash_table *table, int key, info *info) {
    int index = m_h(key);
    printf("Index is %d\n", index);
    (*table)[index] = dll_insert((*table)[index], key, info);
    return table;
}

// Hash search, division and multiplication method
dll_node *d_hash_search(hash_table table, int key) {
    int index = d_h(key);
    return dll_search(table[index], key);
}

dll_node *m_hash_search(hash_table table, int key) {
    int index = m_h(key);
    return dll_search(table[index], key);
}
