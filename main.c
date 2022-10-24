#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

typedef struct node *Node;
typedef struct list *List;

struct list {
    Node node;
    List next;
};

struct node {
    int value;
    char symbol;
    List children;
};

//Node functions
Node create_node(int value, char symbol, List children);
int compare_nodes(const void *s1, const void *s2);
int value(Node n);

//List functions
List create_list(Node node);
List list_append(List head, Node new_node);
List list_insert(List head, Node new_node);

List get_last_n(List head, int total_nodes, int total_nodes_to_get);
List remove_last_n(List head, int total_nodes, int total_nodes_to_remove);

int list_sum(List head);
void print_list(List head);

//Huffman encoding
Node construct_huffman_tree(int radix, int num_symbols, List leaf_nodes);

int main (void) {
    int radix;
    printf("Enter radix: ");
    scanf("%d", &radix);
    assert(radix > 1);

    printf("How many symbols are there: ");
    int num_symbols;
    scanf("%d", &num_symbols);
    assert(num_symbols > 0);

    List leaf_nodes = NULL;
    printf("Enter symbols and probabilities (s1 p1 s2 p2...): ");

    int probability;
    char symbol;
    for (int i = 0; i < num_symbols; i++) {
        scanf(" %c", &symbol);
        scanf("%d", &probability);
        leaf_nodes = list_insert(leaf_nodes, create_node(probability, symbol, NULL));
    }
    Node root = construct_huffman_tree(radix, num_symbols, leaf_nodes);
    return 0;
}

Node construct_huffman_tree(int radix, int num_symbols, List leaf_nodes) {
    print_list(leaf_nodes);
    int average_word_length = 0;

    int num_remaining_symbols = num_symbols;
    while (num_remaining_symbols > 1) {
        List children = get_last_n(leaf_nodes, num_remaining_symbols, radix);
        leaf_nodes = remove_last_n(leaf_nodes, num_remaining_symbols, radix);
        num_remaining_symbols -= radix;
        
        int child_sum = list_sum(children);
        average_word_length += child_sum;

        Node parent = create_node(child_sum, '*', children);
        leaf_nodes = list_insert(leaf_nodes, parent);
        num_remaining_symbols++;
    }
    printf("Average word length: %d\n", average_word_length);
    return leaf_nodes->node;
}

//////////////////////// Helper Functions ////////////////////////

int compare_nodes(const void *s1, const void *s2) {
    return ((Node)s1)->value - ((Node)s2)->value;
}

//////////////////////// Node ADT ////////////////////////

Node create_node(int value, char symbol, List children) {
    Node new_node = malloc(sizeof(*new_node));
    
    new_node->value = value;
    new_node->symbol = symbol;
    new_node->children = children;

    return new_node;
}

//Returns the value of a given node
int value(Node n) {
    if (n == NULL) return 0;
    return n->value;
}

/////////////////////////// List ADT //////////////////////////

//Creates a new list
List create_list(Node node) {
    List new_list = malloc(sizeof(*new_list));
    new_list->node = node;
    new_list->next = NULL;

    return new_list;
}

//Adds a new node to the end of the List
List list_append(List head, Node new_node) {
    List new_list = create_list(new_node);

    if (head == NULL) return new_list;

    List current = head;
    while (current->next != NULL) {
        current = current->next;
    }
    current->next = new_list;
    return head;
}

//Inserts a node at the first location that maintains the list being sorted
List list_insert(List head, Node new_node) {
    List new_list = create_list(new_node);

    if (head == NULL) return new_list;
    if (compare_nodes(new_list->node, head->node) >= 0) {
        new_list->next = head;
        return new_list;
    }
    List current = head;
    List next = current->next;
    while (current->next != NULL) {
        if (compare_nodes(new_list->node, next->node) >= 0) {
            current->next = new_list;
            new_list->next = next;
            return head;
        }
        current = next;
        next = next->next;
    }
    current->next = new_list;
    return head;
}

//Returns the last n elements in a list
List get_last_n(List head, int total_nodes, int total_nodes_to_get) {
    if (head == NULL) return NULL;
    if (total_nodes <= total_nodes_to_get) return head;

    int current_node_number = total_nodes;
    List current = head;
    while (current != NULL && (current_node_number > total_nodes_to_get)) {
        current = current->next;
        current_node_number--;
    }
    if (current == NULL) return head;

    return current;
}

//Removes the last total_nodes_to_remove nodes from a list (doesn't free memory)
List remove_last_n(List head, int total_nodes, int total_nodes_to_remove) {
    if (head == NULL || total_nodes_to_remove >= total_nodes) return NULL;

    int current_node_number = total_nodes;
    List current = head;
    while (current != NULL && (current_node_number > total_nodes_to_remove + 1)) {
        current = current->next;
        current_node_number--;
    }
    if (current == NULL) return NULL;
    current->next = NULL;
    return head;
}

//Finds the sum of all nodes in the list
int list_sum(List head) {
    int sum = 0;
    List current = head;
    while (current != NULL) {
        sum += value(current->node);
        current = current->next;
    }
    return sum;
}

//Prints out all values in the list
void print_list(List head) {
    List current = head;
    while (current != NULL) {
        printf("%d ", value(current->node));
        current = current->next;
    }
    printf("\n");
}