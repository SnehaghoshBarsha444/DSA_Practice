#include <stdio.h>
#include <stdlib.h>

// --------- Node Definition ---------
typedef struct Node {
    int data;
    struct Node* next;
} Node;

// --------- Stack Node for Undo ---------
typedef struct StackNode {
    Node* snapshot;
    struct StackNode* next;
} StackNode;

// --------- Global Variables ---------
Node* head = NULL;               // Main linked list head
StackNode* undoStack = NULL;     // Stack for undo functionality

// --------- Utility Functions ---------

// Create a new node with given data
Node* create_node(int data) {
    Node* new_node = (Node*)malloc(sizeof(Node));
    if (!new_node) {
        printf("Memory allocation failed.\n");
        exit(1);
    }
    new_node->data = data;
    new_node->next = NULL;
    return new_node;
}

// Deep copy a linked list
Node* copy_list(Node* source) {
    if (source == NULL) return NULL;

    Node* new_head = create_node(source->data);
    Node* temp_new = new_head;
    Node* temp_src = source->next;

    while (temp_src != NULL) {
        temp_new->next = create_node(temp_src->data);
        temp_new = temp_new->next;
        temp_src = temp_src->next;
    }

    return new_head;
}

// Free the entire linked list
void free_list(Node* node) {
    while (node != NULL) {
        Node* temp = node;
        node = node->next;
        free(temp);
    }
}

// --------- Linked List Functions ---------

// Insert a node at the end
void insert_end(int data) {
    Node* new_node = create_node(data);
    if (head == NULL) {
        head = new_node;
        return;
    }

    Node* temp = head;
    while (temp->next != NULL)
        temp = temp->next;

    temp->next = new_node;
}

// Insert a node at the beginning
void insert_begin(int data) {
    Node* new_node = create_node(data);
    new_node->next = head;
    head = new_node;
}

// Insert after a specific position (0-based index)
void insert_after(int position, int data) {
    Node* new_node = create_node(data);
    if (head == NULL) {
        if (position == 0) {
            head = new_node;
        } else {
            printf("List is empty. Can't insert at position %d.\n", position);
            free(new_node);
        }
        return;
    }

    Node* temp = head;
    int index = 0;

    while (temp != NULL && index < position) {
        temp = temp->next;
        index++;
    }

    if (temp == NULL) {
        printf("Position %d out of bounds.\n", position);
        free(new_node);
        return;
    }

    new_node->next = temp->next;
    temp->next = new_node;
}

// Insert before a specific position (0-based index)
void insert_before(int position, int data) {
    if (position < 0) {
        printf("Invalid position %d.\n", position);
        return;
    }

    if (position == 0) {
        // Insert at beginning
        insert_begin(data);
        return;
    }

    Node* new_node = create_node(data);
    Node* temp = head;
    int index = 0;

    // Traverse to node before the position
    while (temp != NULL && index < position - 1) {
        temp = temp->next;
        index++;
    }

    if (temp == NULL) {
        printf("Position %d out of bounds.\n", position);
        free(new_node);
        return;
    }

    new_node->next = temp->next;
    temp->next = new_node;
}

// Edit the first node with given value
void edit_node(int old_value, int new_value) {
    Node* temp = head;
    while (temp != NULL) {
        if (temp->data == old_value) {
            temp->data = new_value;
            return;
        }
        temp = temp->next;
    }
    printf("Value %d not found.\n", old_value);
}

// Print the linked list
void print_list(Node* node) {
    if (node == NULL) {
        printf("List is empty.\n");
        return;
    }

    while (node != NULL) {
        printf("%d -> ", node->data);
        node = node->next;
    }
    printf("NULL\n");
}

// --------- Stack (Undo) Functions ---------

// Push current state of list to stack
void push_stack() {
    StackNode* new_stack_node = (StackNode*)malloc(sizeof(StackNode));
    if (!new_stack_node) {
        printf("Memory allocation failed.\n");
        exit(1);
    }
    new_stack_node->snapshot = copy_list(head);
    new_stack_node->next = undoStack;
    undoStack = new_stack_node;
}

// Pop last saved state and restore
void pop_stack() {
    if (undoStack == NULL) {
        printf("No operations to undo.\n");
        return;
    }

    // Free current list
    free_list(head);

    // Restore from stack
    StackNode* temp_stack = undoStack;
    head = temp_stack->snapshot;
    undoStack = undoStack->next;
    free(temp_stack);
}

// --------- Main Function ---------

int main() {
    int choice, value, new_value, position;

    while (1) {
        printf("\n--- MENU ---\n");
        printf("1. Insert at end\n");
        printf("2. Insert after position\n");
        printf("3. Insert before position\n");        // New option
        printf("4. Insert at beginning\n");           // New option
        printf("5. Edit node\n");
        printf("6. Print list\n");
        printf("7. Undo last operation\n");
        printf("8. Exit\n");
        printf("Enter choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                printf("Enter value to insert at end: ");
                scanf("%d", &value);
                push_stack(); // Save current state
                insert_end(value);
                break;

            case 2:
                printf("Enter position to insert after (0-based): ");
                scanf("%d", &position);
                printf("Enter value to insert: ");
                scanf("%d", &value);
                push_stack(); // Save current state
                insert_after(position, value);
                break;

            case 3:
                printf("Enter position to insert before (0-based): ");
                scanf("%d", &position);
                printf("Enter value to insert: ");
                scanf("%d", &value);
                push_stack(); // Save current state
                insert_before(position, value);
                break;

            case 4:
                printf("Enter value to insert at beginning: ");
                scanf("%d", &value);
                push_stack(); // Save current state
                insert_begin(value);
                break;

            case 5:
                printf("Enter value to edit: ");
                scanf("%d", &value);
                printf("Enter new value: ");
                scanf("%d", &new_value);
                push_stack(); // Save current state
                edit_node(value, new_value);
                break;

            case 6:
                print_list(head);
                break;

            case 7:
                pop_stack(); // Undo last change
                break;

            case 8:
                printf("Exiting...\n");
                free_list(head);
                while (undoStack != NULL) {
                    StackNode* temp = undoStack;
                    undoStack = undoStack->next;
                    free_list(temp->snapshot);
                    free(temp);
                }
                exit(0);

            default:
                printf("Invalid choice. Please try again.\n");
        }
    }

    return 0;
}
