#include <stdio.h>
#include <stdlib.h>

// Define the Task structure
typedef struct {
    int periodicity;
    int priority;
} Task;

// Define the Node structure
typedef struct Node {
    Task* data;
    struct Node* next;
} Node;

// Initialize the head of the linked list
Node* init() {
    return NULL;
}

// Create a new Node
Node* createNode(Task* task) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    if (!newNode) {
        printf("Memory allocation failed.\n");
        exit(1);
    }
    newNode->data = task;
    newNode->next = NULL;
    return newNode;
}

// Add a node to the end of the list or head if the list is empty
void addNode(Node** head, Task* task) {
    Node* newNode = createNode(task);
    if (*head == NULL) {
        *head = newNode;
        return;
    }
    Node* current = *head;
    while (current->next != NULL) {
        current = current->next;
    }
    current->next = newNode;
}

// Remove a node
void removeNode(Node** head, Task* task) {
    if (*head == NULL) {
        printf("List is empty.\n");
        return;
    }

    Node* current = *head;
    Node* previous = NULL;

    while (current != NULL) {
        if (current->data == task) {
            if (previous == NULL) {
                *head = current->next;
            } else {
                previous->next = current->next;
            }
            free(current->data);
            free(current);
            printf("Node removed.\n");
            return;
        }
        previous = current;
        current = current->next;
    }
    printf("Task not found in the list.\n");
}

// Free the entire list
void freeList(Node** head) {
    Node* current = *head;
    Node* nextNode;
    while (current != NULL) {
        nextNode = current->next;
        free(current->data);
        free(current);
        current = nextNode;
    }
    *head = NULL;
}

// Add a node in a sorted manner
void addNodeSorted(Node** head, Task* task) {
    Node* newNode = createNode(task);
    if (*head == NULL || 
        (*head)->data->periodicity > task->periodicity || 
        ((*head)->data->periodicity == task->periodicity && (*head)->data->priority > task->priority)) {
        newNode->next = *head;
        *head = newNode;
        return;
    }

    Node* current = *head;
    while (current->next != NULL && 
          (current->next->data->periodicity < task->periodicity || 
          (current->next->data->periodicity == task->periodicity && current->next->data->priority <= task->priority))) {
        current = current->next;
    }
    newNode->next = current->next;
    current->next = newNode;
}

// Utility function to print the list
void printList(Node* head) {
    Node* current = head;
    while (current != NULL) {
        printf("Task(periodicity: %d, priority: %d) -> ", current->data->periodicity, current->data->priority);
        current = current->next;
    }
    printf("NULL\n");
}

// Main function to demonstrate the implementation
int main() {
    Node* list = init();

    Task* t1 = (Task*)malloc(sizeof(Task)); t1->periodicity = 5; t1->priority = 2;
    Task* t2 = (Task*)malloc(sizeof(Task)); t2->periodicity = 3; t2->priority = 1;
    Task* t3 = (Task*)malloc(sizeof(Task)); t3->periodicity = 5; t3->priority = 1;

    addNodeSorted(&list, t1);
    addNodeSorted(&list, t2);
    addNodeSorted(&list, t3);

    printf("List after adding tasks:\n");
    printList(list);

    removeNode(&list, t2);
    printf("List after removing a task:\n");
    printList(list);

    freeList(&list);
    printf("List after freeing:\n");
    printList(list);

    return 0;
}