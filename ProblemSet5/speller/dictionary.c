// Implements a dictionary's functionality

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "dictionary.h"

// Represents number of buckets in a hash table
#define N 100000

int words_counter = 0;

// Represents a node in a hash table
typedef struct node
{
    char word[LENGTH + 1];
    struct node *next;
}
node;

// Represents a hash table
node *hashtable[N];
node *last_node_table[N];

// Hashes given word
// It's a good hash-function. Source: Kernighan & Pike. The practice of programming
unsigned int hash(const char *word)
{

    int MULTIPLIER = 31;
    unsigned char *c;
    unsigned int h = 0;

    for (c = (unsigned char *) word; *c != '\0'; c++)
    {
        h = MULTIPLIER * h + *c;
    }

    // printf("%s %d\n", word, h);
    return h % N;
}

// Loads dictionary into memory, returning true if successful else false
bool load(const char *dictionary)
{
    // Initialize hash table
    for (int i = 0; i < N; i++)
    {
        hashtable[i] = NULL;
        last_node_table[i] = NULL;
    }

    int position;

    // Open dictionary
    FILE *file = fopen(dictionary, "r");
    if (file == NULL)
    {
        unload();
        return false;
    }

    // Buffer for a word
    char word[LENGTH + 1];

    // Insert words into hash table
    while (fscanf(file, "%s", word) != EOF)
    {
        // Push the next just read in the dictionary word into the list
        node *new_node = (node *) malloc(sizeof(node));
        if (!new_node)   // Insufficient memory
        {
            words_counter = 0;
            return false;
        }
        words_counter += 1;
        strcpy(new_node->word, word);
        new_node->next = NULL;
        position = hash(word);

        if (hashtable[position])
        {
            last_node_table[position]->next = new_node;
            last_node_table[position] = new_node;
        }
        else
        {
            hashtable[position] = new_node;
            last_node_table[position] = new_node;
        }
    }

    // Close dictionary
    fclose(file);

    // Indicate success
    return true;
}

// Returns number of words in dictionary if loaded else 0 if not yet loaded
unsigned int size(void)
{
    return words_counter;
}

// Returns true if word is in dictionary else false
bool check(const char *word)
{
    char lowercased_word[LENGTH + 1];
    word_to_lower(word, lowercased_word);

    int hash_index = hash(lowercased_word);

    // printf("%s %d\n", lowercased_word, start_bigram);

    node *ptr = hashtable[hash_index];
    if (!ptr)
    {
        return false;
    }

    int words_diff;
    do
    {
        words_diff = strcmp(ptr->word, lowercased_word);
        ptr = ptr->next;
    }
    while (ptr && words_diff < 0);
    if (words_diff == 0)
    {
        return true;
    }

    return false;
}

// Unloads dictionary from memory, returning true if successful else false
bool unload(void)
{
    for (int i = 0; i < N; ++i)
    {
        node *ptr = hashtable[i];
        while (ptr != NULL)
        {
            node *next_ptr = ptr->next;
            free(ptr);
            ptr = next_ptr;
        }
    }
    return true;
}

// -------------------------------------------------------------
// Converts a given word to lowercase
void word_to_lower(const char *from, char to[])
{
    int i = 0;
    char c;
    while ((c = from[i]) != '\0')
    {
        to[i] = tolower(c);
        i += 1;
    }
    to[i] = '\0';
}