/*
Made by Josh Caithness (https://github.com/BendyCabbage)
Made for MATH3411 (Information, Codes and Ciphers)

For more information on the implementation of the Huffman Algorithm:
https://www.geeksforgeeks.org/huffman-coding-greedy-algo-3/
This is a more generalised version of the one shown, 
where instead of each node having two children, they have radix children
*/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#define PARENT_SYMBOL '*'
#define DUMMY_SYMBOL '~'
#define MAX_RADIX 10
#define MAX_CODE_LENGTH 100
#define ASCII_ZERO 48


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
void find_codes(Node root);
void do_find_codes(Node node, char *code_string);

int main (void) {
    int radix;
    printf("Enter radix: ");
    scanf("%d", &radix);
    assert(radix > 1 && radix <= MAX_RADIX);

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

    //Inserting dummy symbols
    int num_required_dummies = (1 - num_symbols) % (radix - 1);
    for (int i = 0; i < num_required_dummies; i++) {
        leaf_nodes = list_insert(leaf_nodes, create_node(0, DUMMY_SYMBOL, NULL));
    }

    Node root = construct_huffman_tree(radix, num_symbols, leaf_nodes);
    find_codes(root);

    return 0;
}

Node construct_huffman_tree(int radix, int num_symbols, List leaf_nodes) {
    int average_word_length = 0;

    int num_remaining_symbols = num_symbols;
    while (num_remaining_symbols > 1) {
        List children = get_last_n(leaf_nodes, num_remaining_symbols, radix);
        leaf_nodes = remove_last_n(leaf_nodes, num_remaining_symbols, radix);
        num_remaining_symbols -= radix;
        
        int child_sum = list_sum(children);
        average_word_length += child_sum;

        Node parent = create_node(child_sum, PARENT_SYMBOL, children);
        leaf_nodes = list_insert(leaf_nodes, parent);
        num_remaining_symbols++;
    }
    printf("Average word length: %d\n", average_word_length);
    return leaf_nodes->node;
}

void find_codes(Node root) {
    char code_string[MAX_CODE_LENGTH];
    do_find_codes(root, code_string);
}

void do_find_codes(Node node, char *code_string) {
    if (node == NULL) return;

    if (node->symbol != PARENT_SYMBOL) {
        printf("%c: %s\n", node->symbol, code_string);
    } else {
        List current = node->children;
        int symbol = 0;
        while (current != NULL) {
            //Appending symbol to the end of code_string
            char new_code_string[MAX_CODE_LENGTH];
            sprintf(new_code_string, "%s%d", code_string, symbol);
            do_find_codes(current->node, new_code_string);

            symbol++;
            current = current->next;
        }
    }
}

//////////////////////// Helper Functions ////////////////////////

int compare_nodes(const void *s1, const void *s2) {
    return ((Node)s1)->value - ((Node)s2)->value;
}

//////////////////////// Node Functions ////////////////////////

//Creates a new node
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

/////////////////////////// List Functions //////////////////////////

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