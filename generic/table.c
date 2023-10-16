//table.c
/**
 * This file (table.c) is an implementation for the set data type.
 * Multiple similar file exists (unsorted.c, sorted.c, and strings/table.c) that implements this set in various other ways.
 * The set data type guarantees no duplicate elements.
 * This implementation reduces the time complexity of searches for values by hashing .
 * However, this implementation leads to a O(N) worst case scenario time complexity for the addElement function.
 *
 * @author Max Blennemann
 * @version 10/10/23
 */

#include "set.h"
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdbool.h>

typedef struct set {
    void** data;
    unsigned* flags; // 0 = empty, 1 = filled, 2 = deleted
    unsigned int count; // Number of elements that contain data
    unsigned int size; // How much space is allocated to the array

    int (* compare)(); //Method passed in from createSet that compares two elements

    unsigned (* hash)(); //Method passed in from createSet that hashes an element
} genericTable;

/**
 * Returns a new set with the specified number of elements as the maximum capacity.
 *
 * @param maxElts the maximum amount of elements the set can hold
 * @return the newly allocated set
 * @timeComplexity O(M) Where m is the maximum number of elements the set can hold (maxElts)
 */
SET* createSet(int maxElts, int (* compare)(), unsigned (* hash)()) {
    genericTable* a = malloc(sizeof(genericTable));
    assert(a != NULL);
    assert(maxElts >= 0);
    a->compare = compare;
    a->hash = hash;
    a->count = 0;
    a->size = maxElts;
    a->data = malloc(maxElts * sizeof(void*));
    a->flags = malloc(maxElts * sizeof(unsigned));
    assert(a->data != NULL);
    assert(a->flags != NULL);
    unsigned i = 0;
    for (; i < maxElts; i++)
        a->flags[i] = 0;
    return a;
}

/**
 * Frees the memory allocated to the set.
 *
 * @param sp the set to destroy
 * @timeComplexity O(1)
 */
void destroySet(SET* sp) {
    assert(sp != NULL);
    free(sp->data);
    free(sp->flags);
}

/**
 * Returns the number of elements in the set
 *
 * @param sp the set to access
 * @return the number of unique elements
 * @timeComplexity O(1)
 */
int numElements(SET* sp) {
    assert(sp != NULL);
    return sp->count;
}

/**
 * Finds the index of an element in the set.
 * Returns the location the element would go if the element is not found.
 * Returns sp->size if the element can't be added.
 * Pass a boolean pointer as found if you want found variable returned as a boolean.
 *
 * @param sp the set to search through
 * @param elt the element to search for
 * @return the index where the element is or should be added
 * or sp->size if the element is not found
 * @timeComplexity (O(N) + user given hash function) worst case; (O(1) + user given hash function) average case
 */
static unsigned int findElementIndex(SET* sp, void* elt, bool* found) {
    assert(sp != NULL);
    assert(elt != NULL);
    unsigned const home = (*sp->hash)(elt) % sp->size;
    unsigned index = home;
    unsigned firstDeleted = sp->size;
    if (index < sp->size) {
        if (sp->flags[index] == 0) {
            if (found != NULL)
                *found = false;
            return index;
        } else if (sp->flags[index] == 1 && (*sp->compare)(sp->data[index], elt) == 0) {
            if (found != NULL)
                *found = true;
            return index;
        } else {
            if (sp->flags[index] == 2 && firstDeleted == sp->size)
                firstDeleted = index;
            index = (index + 1) % sp->size;
        }
    }
    while (index < sp->size && index != home) {
        if (sp->flags[index] == 0) {
            if (found != NULL)
                *found = false;
            return index;
        } else if (sp->flags[index] == 1 && (*sp->compare)(sp->data[index], elt) == 0) {
            if (found != NULL)
                *found = true;
            return index;
        } else {
            if (sp->flags[index] == 2 && firstDeleted == sp->size)
                firstDeleted = index;
            index = (index + 1) % sp->size;
        }
    }
    if (found != NULL)
        *found = false;
    return firstDeleted;
}

/**
 * Adds a new element to the set
 * Sorts the list after the element is added to guarantee the set is sorted
 *
 * @param sp the set to add an element to
 * @param elt the element to add.
 * @timeComplexity (O(N) + user given hash function) worst case; (O(1) + user given hash function) average case
 */
void addElement(SET* sp, void* elt) {
    assert(sp != NULL);
    assert(elt != NULL);
    assert(sp->count < sp->size);
    bool alreadyExists = false;
    unsigned int index = findElementIndex(sp, elt, &alreadyExists);
    if (alreadyExists)
        return;
    sp->data[index] = strdup(elt);
    sp->flags[index] = 1;
    sp->count++;
}

/**
 * This method removes an element from the give set.
 * Marks the flag array for removed elements as 2.
 * This function will silently fail if the element given does not exist.
 *
 * @param sp the set to remove the element from
 * @param elt the element to remove
 * @timeComplexity (O(N) + user given hash function) worst case; (O(1) + user given hash function) average case
 */
void removeElement(SET* sp, void* elt) {
    assert(sp != NULL);
    if (elt != NULL) {
        bool found = false;
        unsigned index = findElementIndex(sp, elt, &found);
        if (found == false)
            return;
        sp->flags[index] = 2;
        sp->count--;
    }
}


/**
 * Finds the element in the set.
 * Returns NULL if the element does not exist within the set.
 *
 * @precondition Set is sorted.
 * @param sp the set to search through
 * @param elt the element to search for
 * @return a pointer to the generic in the set if it exists
 * otherwise NULL
 * @timeComplexity O(N) worst case; O(1) average case
 */
void* findElement(SET* sp, void* elt) {
    assert(sp != NULL);
    if (elt == NULL)
        return NULL;
    bool found = false;
    unsigned a = findElementIndex(sp, elt, &found);
    if (found == false)
        return NULL;
    return sp->data[a];
}

/**
 * Copies all the values in the set to a new array and returns that new array.
 * The user must free the array of generics before exiting to avoid a memory leak.
 * Since this set is not sorted by any distinguishable features of the generics
 * the returned array is not guaranteed to be sorted in any way.
 *
 * @param sp The set to access
 * @return A new array of void* pointers to the generics in the sets
 * @timeComplexity O(N)
 */
void* getElements(SET* sp) { //TODO check if this works
    assert(sp != NULL);
    void** toReturn = malloc(sp->count * sizeof(void*));
    assert(toReturn != NULL);
    unsigned whereToAdd = 0;
    unsigned i = 0;
    for (; i < sp->size; i++) {
        if (sp->flags[i] == 1) {
            toReturn[whereToAdd] = sp->data[i];
            whereToAdd++;
        }
    }
    return toReturn;
}