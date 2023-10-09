/**
 * This file (sorted.c) is an implementation for the set data type.
 * A similar file exists (unsorted.c) that implements this set but does not sort the elements as they are added.
 * The set data type guarantees no duplicate elements.
 * This implementation reduces the time complexity of searches for values by sorting the array (from O(N) to O(log(N)))).
 * However, this implementation leads to a O(Nlog(N)) worst case scenario time complexity for the addElement function.
 *
 * @author Max Blennemann
 * @version 9/25/23
 */

#include "set.h"
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdbool.h>

typedef struct set {
    char** data;
    unsigned* flags; // 0 = empty, 1 = filled, 2 = deleted
    unsigned int count; // Number of elements that contain data
    unsigned int size; // How much space is allocated to the array
} stringTable;

/**
 * Returns a new set with the specified number of elements as the maximum capacity.
 *
 * @param maxElts the maximum amount of elements the set can hold
 * @return the newly allocated set
 * @timeComplexity O(N)
 */
SET* createSet(int maxElts) { // maxElts should be unsigned but the header file has this variable signed
    stringTable* a = malloc(sizeof(stringTable));
    assert(a != NULL);
    a->count = 0;
    a->size = maxElts;
    a->data = malloc(maxElts * sizeof(char*));
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
 * @timeComplexity O(N)
 */
void destroySet(SET* sp) {
    assert(sp != NULL);
    int i = 0;
    for (; i < sp->count; i++)
        free(sp->data[i]);
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
 * Returns -1 if the element does not exist within the set.
 * Pass a boolean pointer as found if you want found variable returned as a boolean.
 *
 * @precondition Set is sorted.
 * @param sp the set to search through
 * @param elt the element to search for
 * @return the index where the element is or should be added and also found kinda
 * @timeComplexity O(log(n))
 */
static unsigned int findElementIndex(SET* sp, char* elt, bool* found) {
    assert(sp != NULL);
    assert(elt != NULL);
    unsigned index = strhash(elt) % sp->size;
    unsigned firstDeleted = sp->size;
    while (index < sp->size) {
        if (sp->flags[index] == 0) {
            if (found != NULL)
                *found = false;
            return index;
        } else if (sp->flags[index] == 1 && strcmp(sp->data[index], elt) == 0) {
            if (found != NULL)
                *found = true;
            return index;
        } else {
            if (sp->flags[index] == 2 && firstDeleted == sp->size)
                firstDeleted = index;
            index++;
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
 * @timeComplexity O(Nlog(N))
 */
void addElement(SET* sp, char* elt) {
    assert(sp != NULL);
    assert(elt != NULL);
    assert(sp->count < sp->size);
    bool alreadyExists = false;
    unsigned int index = findElementIndex(sp, elt, &alreadyExists);
    if (alreadyExists)
        return;
    sp->data[index] = strdup(elt);
    sp->count++;
}

/**
 * This method removes an element from the give set.
 * It also shifts all the elements after it forward 1 to speed up future searches.
 * This element keeps the set sorted.
 * This function will silently fail if the string given does not exist.
 *
 * @param sp the set to remove the element from
 * @param elt the element to remove
 * @timeComplexity O(N)
 */
void removeElement(SET* sp, char* elt) {
    assert(sp != NULL);
    if (elt != NULL) {
        bool found = false;
        unsigned index = findElementIndex(sp, elt, &found);
        if (found == false)
            return;
        free(sp->data[index]);
        sp->flags[index] = 2;
        sp->data[index] = NULL;
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
 * @return a pointer to the string in the set if it exists
 * otherwise NULL
 * @timeComplexity O(log(N))
 */
char* findElement(SET* sp, char* elt) {
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
 * The user must free the array of strings before exiting to avoid a memory leak.
 * Since this set maintains an array of strings sorted alphabetically this method is guaranteed to
 * return an array of alphabetically sorted strings.
 *
 * @param sp The set to access
 * @return A new array of strings
 * @timeComplexity O(N)
 */
char** getElements(SET* sp) {
    assert(sp != NULL);
    char** toReturn = malloc(sp->count * sizeof(char*));
    assert(toReturn != NULL);
    unsigned whereToAdd = 0;
    unsigned i = 0;
    for (; i < sp->size; i++) {
        if (sp->flags[i] == 1) {
            toReturn[whereToAdd] = strdup(sp->data[i]);
            whereToAdd++;
        }
    }
    return toReturn;
}