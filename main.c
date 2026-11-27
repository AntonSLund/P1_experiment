#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define COUNT 10000

typedef struct {
    const char *word0;
    const char *word1;
    int occurrences;
}Bigram;

typedef struct {
    const char *word0;
    const char *word1;
    const char *word2;
    int occurrences;
}Trigram;

int next_word(FILE* file, char word[], int size);
int read_next_words(FILE* file, char* words[], int words_size);
int find_bigrams(Bigram bigrams[], int bigrams_size, char* words[], int words_len);
int find_trigrams(Trigram trigrams[], int trigrams_size, char* words[], int words_len);
int bigrams_cmp(const void* a, const void* b);
int trigrams_cmp(const void* a, const void* b);

int main(void) {

    // Opens the file in read.
    FILE* file = fopen("trainingdata.txt", "r");
    // A check that secures that the file has opened.
    if (file == NULL) {
        printf("Error: File not open\n");
        exit(EXIT_FAILURE);
    }

    // Makes an array to store the words in the file.
    char** words = malloc(sizeof(char*)*COUNT);
    // Saves the words in the array and gives back the length of the array.
    int words_len = read_next_words(file, words, COUNT);

    // Makes an array to store the different bigram in.
    Bigram* bigrams = malloc(sizeof(*bigrams)*COUNT);
    // Inputs the bigram into the array and returns the length of the array
    int number_of_bigrams = find_bigrams(bigrams, COUNT, words, words_len);
    // We sort the array and prints it.
    qsort(bigrams, number_of_bigrams, sizeof(bigrams[0]), bigrams_cmp);
    for (int i = 0; i < number_of_bigrams; i++) {
        printf("%s %s %d\n", bigrams[i].word0, bigrams[i].word1, bigrams[i].occurrences);
    }
    free(bigrams);

    // This section is just like with the bigrams but for trigrams.
    Trigram* trigrams = malloc(sizeof(*trigrams)*COUNT);
    int number_of_trigrams = find_trigrams(trigrams, COUNT, words, words_len);
    qsort(trigrams, number_of_trigrams, sizeof(trigrams[0]), trigrams_cmp);
    for (int i = 0; i < number_of_trigrams; i++) {
        printf("%s %s %s %d\n", trigrams[i].word0, trigrams[i].word1, trigrams[i].word2, trigrams[i].occurrences);
    }
    free(trigrams);


    // Remember to free the space we malloc, also close the file.
    for (int i = 0; i < words_len; i++) {
        free(words[i]);
    }
    free(words);
    fclose(file);
    return 0;
}

// Reads a singular word.
int next_word(FILE* file, char word[], int size) {
    // Looks at a character in the file and saves it in ch.
    int ch = fgetc(file);
    int len = 0;
    // If the ch we are looking at it is ' ' or new line then we get the next character.
    while (isspace(ch)) {
        ch = fgetc(file);
    }
    // Here if we are at a letter it will save the letter in a array.
    while (!isspace(ch) && ch != EOF) {
        word[len] = tolower(ch);
        len++;
        ch = fgetc(file);
    }
    // Adds the null char to the array.
    word[len] = '\0';

    // Returns the length of array word.
    return len;
}

// Saves the word into an array named words.
int read_next_words(FILE* file, char* words[], int words_size) {
    int words_len = 0;
    for (int i = 0; i < words_size; i++) {
        // Makes an array to save a word.
        char *word = malloc(sizeof(char)*100);
        // Saves the length of the word.
        int word_len = next_word(file, word, 100);
        // If the word length is 0. It didn't find a word. Free the memory of the previous malloc. Then break the loop.
        if (word_len == 0) {
            free(word);
            break;
        }
        // Change the memory from the whole length to the length of the word.
        word = realloc(word, word_len + 1);
        // Saves the word in a predefined array.
        words[words_len] = word;
        words_len += 1;
    }
    // Returns the length of array words.
    return words_len;
}

static Bigram* find_bigram_by_bigram(Bigram bigrams[], int number_of_bigrams, const char* word0, const char* word1) {
    for (int i = 0; i < number_of_bigrams; i++) {
        if (strcmp(word0, bigrams[i].word0) == 0 && strcmp(word1, bigrams[i].word1) == 0) {
            return &bigrams[i];
        }
    }
    return NULL;
}

int find_bigrams(Bigram bigrams[], int bigrams_size, char* words[], int words_len) {
    int number_of_bigrams = 0;
    for (int i = 1; i < words_len; i++) {
        const char* word0 = words[i - 1];
        const char* word1 = words[i];
        assert (word0 && word1);
        Bigram* existing = find_bigram_by_bigram(bigrams, number_of_bigrams, word0, word1);

        if (existing != NULL) {
            existing->occurrences += 1;
            continue;
        }

        if (number_of_bigrams >= bigrams_size) {
            return -1;
        }

        bigrams[number_of_bigrams] = (Bigram) {
            .word0 = word0,
            .word1 = word1,
            .occurrences = 1,
        };

        number_of_bigrams += 1;
    }
    return number_of_bigrams;
}

static Trigram* find_trigram_by_trigram(Trigram trigrams[], int number_of_trigrams, const char* word0, const char* word1, const char* word2) {
    for (int i = 0; i < number_of_trigrams; i++) {
        if (strcmp(word0, trigrams[i].word0) == 0 && strcmp(word1, trigrams[i].word1) == 0 && strcmp(word2, trigrams[i].word2) == 0) {
            return &trigrams[i];
        }
    }
    return NULL;
}

int find_trigrams(Trigram trigrams[], int trigrams_size, char* words[], int words_len) {
    int number_of_trigrams = 0;
    for (int i = 2; i < words_len; i++) {
        const char* word0 = words[i - 2];
        const char* word1 = words[i - 1];
        const char* word2 = words[i];
        assert (word0 && word1);
        Trigram* existing = find_trigram_by_trigram(trigrams, number_of_trigrams, word0, word1, word2);

        if (existing != NULL) {
            existing->occurrences += 1;
            continue;
        }

        if (number_of_trigrams >= trigrams_size) {
            return -1;
        }

        trigrams[number_of_trigrams] = (Trigram) {
            .word0 = word0,
            .word1 = word1,
            .word2 = word2,
            .occurrences = 1,
        };

        number_of_trigrams += 1;
    }
    return number_of_trigrams;
}

int bigrams_cmp(const void* a, const void* b) {
    const Bigram* bigram_a = a;
    const Bigram* bigram_b = b;
    if (bigram_a->occurrences != bigram_b->occurrences) {
        return bigram_b->occurrences - bigram_a->occurrences;
    }
    if (strcmp(bigram_a->word0, bigram_b->word0) == 0) {
        return strcmp(bigram_a->word1, bigram_b->word1);
    }
    return strcmp(bigram_a->word0, bigram_b->word0);
}

int trigrams_cmp(const void* a, const void* b) {
    const Trigram* trigram_a = a;
    const Trigram* trigram_b = b;
    if (trigram_a->occurrences != trigram_b->occurrences) {
        return trigram_b->occurrences - trigram_a->occurrences;
    }
    int word0_cmp = strcmp(trigram_a->word0, trigram_b->word0);
    if (word0_cmp != 0) {
        return word0_cmp;
    }
    int word1_cmp = strcmp(trigram_a->word1, trigram_b->word1);
    if (word1_cmp != 0) {
        return word1_cmp;
    }
    return strcmp(trigram_a->word2, trigram_b->word2);
}
