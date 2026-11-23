#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define COUNT 10000

typedef struct {
    const char *word;
    int occurrences;
}CountedWord;

typedef struct {
    const char *word0;
    const char *word1;
    int occurrences;
}Bigram;

int next_word(FILE* file, char word[], int size);
int read_next_words(FILE* file, char* words[], int words_size);
int find_bigrams(Bigram bigrams[], int bigrams_size, char* words[], int words_len);
int find_occurrences(CountedWord counted_words[], int counted_words_size, char* words[], int words_len);
int word_cmp(const void* a, const void* b);
int counted_word_cmp(const void* a, const void* b);
int bigrams_cmp(const void* a, const void* b);

int main(void) {

    FILE* file = fopen("license.txt", "r");
    if (file == NULL) {
        printf("Error: File not open\n");
        exit(EXIT_FAILURE);
    }

    char** words = malloc(sizeof(char*)*COUNT);
    int words_len = read_next_words(file, words, COUNT);

    for (int i = 0; i < words_len; i++) {
        //printf("%s \n", words[i]);
    }
    //qsort(words, words_len, sizeof(words[0]), word_cmp);


    CountedWord* counted_words = malloc(sizeof(*counted_words)*COUNT);
    int counted_words_len = find_occurrences(counted_words, COUNT, words, words_len);
    qsort(counted_words, counted_words_len, sizeof(counted_words[0]), counted_word_cmp);

    //for (int i = 0; i < counted_words_len; i++) {
    //    printf("%s %d\n", counted_words[i].word, counted_words[i].occurrences);
    //}

    Bigram* bigrams = malloc(sizeof(*bigrams)*COUNT);
    int number_of_bigrams = find_bigrams(bigrams, COUNT, words, words_len);
    qsort(bigrams, number_of_bigrams, sizeof(bigrams[0]), bigrams_cmp);
    for (int i = 0; i < number_of_bigrams; i++) {
        printf("%s %s %d\n", bigrams[i].word0, bigrams[i].word1, bigrams[i].occurrences);
    }

    for (int i = 0; i < words_len; i++) {
        free(words[i]);
    }

    free(bigrams);
    free(words);
    free(counted_words);
    fclose(file);
    return 0;
}

int next_word(FILE* file, char word[], int size) {
    int ch = fgetc(file);
    int len = 0;
    while (isspace(ch)) {
        ch = fgetc(file);
    }
    while (!isspace(ch) && ch != EOF) {
        word[len] = tolower(ch);
        len++;
        ch = fgetc(file);
    }
    word[len] = '\0';

    return len;
}

int read_next_words(FILE* file, char* words[], int words_size) {
    int words_len = 0;
    for (int i = 0; i < words_size; i++) {
        char *word = malloc(sizeof(char)*100);
        int word_len = next_word(file, word, 100);
        if (word_len == 0) {
            free(word);
            break;
        }
        word = realloc(word, word_len + 1);
        words[words_len] = word;
        words_len += 1;
    }
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

static CountedWord* find_counted_word_by_word(CountedWord counted_words[], int counted_words_len, const char* word) {
    for (int j = 0; j < counted_words_len; j++) {
        if (strcmp(word, counted_words[j].word) == 0) {
            return &counted_words[j];
        }
    }
    return NULL;
}

int find_occurrences(CountedWord counted_words[], int counted_words_size, char* words[], int words_len) {
    int counted_words_len = 0;
    for (int i = 0; i < words_len; i++) {
        CountedWord* existing = find_counted_word_by_word(counted_words, counted_words_len, words[i]);

        if (existing != NULL) {
            existing->occurrences += 1;
            continue;
        }

        if (counted_words_len >= counted_words_size) {
            return -1;
        }

        counted_words[counted_words_len] = (CountedWord) {
            .word = words[i],
            .occurrences = 1,
        };
        counted_words_len += 1;
    }
    return counted_words_len;
}

int word_cmp(const void* a, const void* b) {
    const char* const* word_a = a;
    const char* const* word_b = b;
    return strcmp(*word_a, *word_b);
}

int counted_word_cmp(const void* a, const void* b) {
    const CountedWord* counted_word_a = a;
    const CountedWord* counted_word_b = b;
    if (counted_word_a->occurrences != counted_word_b->occurrences) {
        return counted_word_b->occurrences - counted_word_a->occurrences;
    }
    return strcmp(counted_word_a->word, counted_word_b->word);
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
