#include "linked_list.h"
#include <stddef.h>
#include <vcruntime.h>

// Function pointers to (potentially) custom malloc() and
// free() functions.
//
static void * (*malloc_fptr)(size_t size) = NULL;
static void   (*free_fptr)(void* addr)    = NULL; 

// Creates a new linked_list.
// PRECONDITION: Register malloc() and free() functions via the
//               linked_list_register_malloc() and 
//               linked_list_register_free() functions.
// POSTCONDITION: An empty linked_list has its head point to NULL.
// Returns a new linked_list on success, NULL on failure.
//
struct linked_list * linked_list_create(void){
    if (!malloc_fptr) 
        return NULL;
    struct linked_list * ll = malloc_fptr(sizeof(struct linked_list));
    if (!ll) 
        return NULL;
    ll->head = NULL;
    return ll;
}
// Deletes a linked_list.
// \param ll : Pointer to linked_list to delete
// POSTCONDITION : An empty linked_list has its head point to NULL.
// Returns TRUE on success, FALSE otherwise.
//
bool linked_list_delete(struct linked_list * ll){
    if (ll == NULL)
        return false;

    struct node *current = ll->head;
    while (current != NULL) {
        struct node *next = current->next;
        free_fptr(current);      
        current = next;
    }

    ll->head = NULL;
    return true;
}
// Returns the size of a linked_list.
// \param ll : Pointer to linked_list.
// Returns size on success, SIZE_MAX on failure.
//
size_t linked_list_size(struct linked_list * ll){
    if(ll==NULL||ll->head==NULL){
        return 0;
    }
    size_t counter=0;
    struct node *curr = ll->head;
    while(curr!=NULL){
        curr = curr->next;
        counter++;
    }
    return counter;
}

// Inserts an element at the end of the linked_list.
// \param ll   : Pointer to linked_list.
// \param data : Data to insert.
// Returns TRUE on success, FALSE otherwise.
//
bool linked_list_insert_end(struct linked_list * ll,unsigned int data){
    if(ll==NULL){
        return false;
    }
    while(ll->head!=NULL){
        ll->head = ll->head->next;
    }
    ll->head->data=data;
    return true;
    }


// Inserts an element at a specified index in the linked_list.
// \param ll    : Pointer to linked_list.
// \param index : Index to insert data at.
// \param data  : Data to insert.
// Returns TRUE on success, FALSE otherwise.
//
bool linked_list_insert(struct linked_list * ll,size_t index,unsigned int data){
    if (ll == NULL)
        return false;

    struct node *new_node = malloc_fptr(sizeof(struct node));
    if (new_node == NULL)
        return false;

    new_node->data = data;
    new_node->next = NULL;

    if (index == 0) {
        new_node->next = ll->head;
        ll->head = new_node;
        return true;
    }

    struct node *curr = ll->head;
    size_t i = 0;
    while (curr != NULL && i < index - 1) {
        curr = curr->next;
        i++;
    }

    if (curr == NULL)
        return false;

    new_node->next = curr->next;
    curr->next = new_node;
    return true;
    }
// Inserts an element at the front of the linked_list.
// \param ll   : Pointer to linked_list.
// \param data : Data to insert.
// Returns TRUE on success, FALSE otherwise.
//
bool linked_list_insert_front(struct linked_list * ll,unsigned int data){
    if (ll == NULL) return false;
    
    struct node *new_node = malloc_fptr(sizeof(struct node));
    if (!new_node) 
        return false;
    
    new_node->data = data;
    new_node->next = ll->head;
    ll->head = new_node;
    return true;
}

// Finds the first occurrence of data and returns its index.
// \param ll   : Pointer to linked_list.
// \param data : Data to find.
// Returns index of the first index with that data, SIZE_MAX otherwise.
//
size_t linked_list_find(struct linked_list * ll,unsigned int data){
    if (ll == NULL)
        return SIZE_MAX;

    struct node *curr = ll->head;
    size_t index = 0;

    while (curr != NULL) {
        if (curr->data == data)
            return index;
        curr = curr->next;
        index++;
    }
    return index;
    }

// Removes a node from the linked_list at a specific index.
// \param ll    : Pointer to linked_list.
// \param index : Index to remove node.
// Returns TRUE on success, FALSE otherwise.
//
bool linked_list_remove(struct linked_list * ll,size_t index){
    if (ll == NULL || ll->head == NULL)
        return false;

    struct node *to_delete = NULL;

    if (index == 0) {
        to_delete = ll->head;
        ll->head = ll->head->next;
        free_fptr(to_delete);

        return true;
    }

    struct node *prev = ll->head;
    for (size_t i = 0; i < index - 1; i++) {
        if (prev->next == NULL)
            return false; 
        prev = prev->next;
    }

    to_delete = prev->next;
    if (to_delete == NULL)
        return false; 

    prev->next = to_delete->next;
    free_fptr(to_delete);


    return true;
}

// Creates an iterator struct at a particular index.
// \param linked_list : Pointer to linked_list.
// \param index       : Index of the linked list to start at.
// Returns pointer to an iterator on success, NULL otherwise.
//
struct iterator * linked_list_create_iterator(struct linked_list * ll,size_t index){

    struct node *node_ptr = ll->head;
    size_t i = 0;
    while (i < index) {
        node_ptr = node_ptr->next;
        i++;
    }
        struct iterator *iter = (struct iterator *)malloc_fptr(sizeof(struct iterator));
        iter->ll = ll;
        iter->data = node_ptr->data;
        iter->current_node = node_ptr;
        iter->current_index = index;
        return iter;
}

// Deletes an iterator struct.
// \param iterator : Iterator to delete.
// Returns TRUE on success, FALSE otherwise.
//
bool linked_list_delete_iterator(struct iterator * iter){
    if (!iter) {
        return false;  
    }
    free_fptr(iter);
    return true; 
}

// Iterates to the next node in the linked_list.
// \param iterator: Iterator to iterate on.
// Returns TRUE when next node is present, FALSE once end of list is reached.
//
bool linked_list_iterate(struct iterator * iter){
    if (!iter || !iter->current_node || !iter->current_node->next) {
        return false; 
    }

    iter->current_node = iter->current_node->next;
    iter->current_index++;
    
    if (iter->current_node) {
        iter->data = iter->current_node->data;
        return true;
    }
    
    return false;  
}

// Registers malloc() function.
// \param malloc : Function pointer to malloc()-like function.
// Returns TRUE on success, FALSE otherwise.
//
bool linked_list_register_malloc(void * (*malloc)(size_t)){
    if (malloc == NULL) {
        return false;
    }
    malloc_fptr = malloc;
    return true;
}

// Registers free() function.
// \param free : Function pointer to free()-like function.
// Returns TRUE on success, FALSE otherwise.
//
bool linked_list_register_free(void (*free)(void*)){
    if(free==NULL){
        return false;
    }
    free_fptr = free;
    return true;
}