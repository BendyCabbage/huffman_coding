/*
Made by Josh Caithness (https://github.com/BendyCabbage)
Made for MATH3411 (Information, Codes and Ciphers)
Only use for educational purposes

For more information on the implementation of the Huffman Algorithm:
https://www.geeksforgeeks.org/huffman-coding-greedy-algo-3/
This is a more generalised version of the one shown, 
where instead of each node having two children, they have radix children

If anyone finds a more intelligent method (aka formula) for determining the number
of dummy symbols needed please let me know
*/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#define PARENT_SYMBOL '*'
#define DUMMY_SYMBOL '~'
#define MAX_RADIX 10
#define MAX_CODE_LENGTH 100

#define TRUE 1
#define FALSE 0

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
int compare_nodes_by_value(Node n1, Node n2);

void free_node(Node node);
int value(Node n);

//List functions
List create_list(Node node);
List list_append(List head, Node new_node);
List list_insert_high(List head, Node new_node, int (*compare)(Node, Node));
List list_insert_low(List head, Node new_node, int (*compare)(Node, Node));

void free_list(List head, int free_nodes);
int list_sum(List head);
void print_list(List head);

List get_last_n(List head, int total_nodes, int total_nodes_to_get);
List remove_last_n(List head, int total_nodes, int total_nodes_to_remove);

//Huffman encoding
Node construct_huffman_tree(int radix, int num_symbols, List leaf_nodes);
List insert_dummy_symbols(int num_required_dummies, List leaf_nodes);
void find_codes(Node root);
void do_find_codes(Node node, char *code_string);

int main (int argc, char *argv[]) {
    int radix;
    int num_symbols;
    List leaf_nodes = NULL;

    if (argc > 2) {
        radix = atoi(argv[1]);
        num_symbols = argc - 2;
        for (int i = 2; i < argc; i++) {
            char symbol = i + '1' - 2;
            int probability = atoi(argv[i]);
            leaf_nodes = list_insert_low(
                leaf_nodes,
                create_node(probability, symbol, NULL),
                compare_nodes_by_value
            );
        }
    } else {
        printf("Enter radix: ");
        scanf("%d", &radix);
        assert(radix > 1 && radix <= MAX_RADIX);

        printf("How many symbols are there: ");
        scanf("%d", &num_symbols);
        assert(num_symbols > 0);

        printf("Enter symbols and probabilities (s1 p1 s2 p2...): ");

        int probability;
        char symbol;
        for (int i = 0; i < num_symbols; i++) {
            scanf(" %c", &symbol);
            scanf("%d", &probability);
            leaf_nodes = list_insert_low(
                leaf_nodes, 
                create_node(probability, symbol, NULL),
                compare_nodes_by_value
            );
        }
    }

    //Inserting dummy symbols
    int num_required_dummies = 0;
    int current_remainder = (num_symbols + num_required_dummies) % (radix - 1);

    while (radix != 2 && current_remainder != 1) {
        num_required_dummies++;
        current_remainder = (num_symbols + num_required_dummies) % (radix - 1);
    }

    leaf_nodes = insert_dummy_symbols(num_required_dummies, leaf_nodes);
    num_symbols += num_required_dummies;
    Node root = construct_huffman_tree(radix, num_symbols, leaf_nodes);
    find_codes(root);
    free_node(root);

    return 0;
}

List insert_dummy_symbols(int num_required_dummies, List leaf_nodes) {
    for (int i = 0; i < num_required_dummies; i++) {
        leaf_nodes = list_insert_low(
            leaf_nodes, 
            create_node(0, DUMMY_SYMBOL, NULL),
            compare_nodes_by_value
        );
    }
    return leaf_nodes;
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
        leaf_nodes = list_insert_high(leaf_nodes, parent, compare_nodes_by_value);
        num_remaining_symbols++;
    }
    printf("Average word length: %d\n", average_word_length);
    return leaf_nodes->node;
}

void find_codes(Node root) {
    char code_string[MAX_CODE_LENGTH] = "";
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

int compare_nodes_by_value(Node n1, Node n2) {
    if (n2 == NULL) return 1;
    if (n1 == NULL) return -1;
    return n1->value - n2->value;
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

void free_node(Node node) {
    if (node == NULL) return;
    free_list(node->children, TRUE);
    free(node);
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
List list_insert_high(List head, Node new_node, int (*compare)(Node, Node)) {
    List new_list = create_list(new_node);

    if (head == NULL) return new_list;
    if (compare(new_list->node, head->node) >= 0) {
        new_list->next = head;
        return new_list;
    }
    List current = head;
    List next = current->next;
    while (current->next != NULL) {
        if (compare(new_list->node, next->node) >= 0) {
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

//Inserts a node at the last location that maintains the list being sorted
List list_insert_low(List head, Node new_node, int (*compare)(Node, Node)) {
    List new_list = create_list(new_node);

    if (head == NULL) return new_list;
    if (compare(new_list->node, head->node) > 0) {
        new_list->next = head;
        return new_list;
    }
    List current = head;
    List next = current->next;
    while (current->next != NULL) {
        if (compare(new_list->node, next->node) > 0) {
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

void free_list(List head, int free_nodes) {
    if (head == NULL) return;

    List current = head;
    while (current != NULL) {
        List temp = current->next;
        if (free_nodes == TRUE) {
            free_node(current->node);
        }
        free(current);
        current = temp;
    }
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