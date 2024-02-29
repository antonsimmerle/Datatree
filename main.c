#include <stdio.h>
#include <stdlib.h>

#define COLOR_YELLOW "\x1b[33m"
#define COLOR_CYAN "\033[0;36m"
#define COLOR_RESET "\x1b[0m"

size_t getlength(char* string);

struct line;
void printline(int* in, int depth, char* literal, int islast);
void printtree(struct line* in, size_t length);
void processline(char* in, struct line* out);
void processlines(FILE* in, struct line** out, size_t* length);

size_t getlength(char* string) {
    size_t i = 0;
    for (; string[i] != '\0'; i++);
    return i;
}

#define UNIX_BRANCH_MID COLOR_CYAN "├─" COLOR_RESET
#define UNIX_BRANCH_END COLOR_CYAN "└─" COLOR_RESET
#define TAB_LINE COLOR_CYAN "│   " COLOR_RESET
#define TAB_FILL COLOR_CYAN "    " COLOR_RESET

#define CHUNK 10

struct line {
    int depth;
    char* literal;
};

void printline(int* in, int depth, char* literal, int islast) {
    for (size_t j = 0; j < depth; j++) {
        printf("%s", in[j] ? TAB_LINE : TAB_FILL);
    }
    printf("%s %s\n", islast ? UNIX_BRANCH_END : UNIX_BRANCH_MID, literal);
}

void printtree(struct line* in, size_t length) {
    int* linestates = NULL;
    size_t capacity = 0;
    size_t i = 0;
    for (; i < length; i++) {
        if (in[i].depth >= capacity) {
            capacity += CHUNK;
            linestates = realloc(linestates, capacity*sizeof(int));
            if (linestates == NULL) {
                perror("Memory allocation failed\n");
                exit(EXIT_FAILURE);
            }
        }
        if ((i > 0 && in[i].depth-1 > in[i-1].depth) ||
            (i == 0 && in[i].depth > 0) ||
            getlength(in[i].literal) == 0) {
            printf("Construction failed at line %zu\n", i+1);
            exit(EXIT_FAILURE);
        }
        int islast = 1;
        for (size_t j = i+1; j < length && in[i].depth <= in[j].depth; j++) {
            if (in[j].depth == in[i].depth) {
                islast = 0;
            }
        }
        printline(linestates, in[i].depth, in[i].literal, islast);
        linestates[in[i].depth] = !islast;
    }
    for (size_t j = 0; j < length; j++) {
        free(in[j].literal);
    }
    free(linestates);
}

void processline(char* in, struct line* out) {
    out->depth = 0;
    out->literal = NULL;
    size_t i = 0;
    for (; in[i] == ' '; i++) {
        out->depth++;
    }
    size_t capacity = 0;
    size_t j = 0;
    for (; in[i] != '\n' && in[i] != '\0'; i++, j++) {
        if (j >= capacity) {
            capacity += CHUNK;
            out->literal = realloc(out->literal, (capacity+1)*sizeof(char));
            if (out->literal == NULL) {
                perror("Memory allocation failed\n");
                exit(EXIT_FAILURE);
            }
        }
        out->literal[j] = in[i];
    }
    out->literal[j] = '\0';
}

void processlines(FILE* in, struct line** out, size_t* length) {    
    char* templine = NULL;
    size_t capacity = 0;
    size_t size = 0;
    for (; getline(&templine, &size, in) != -1; (*length)++) {
        if (*length >= capacity) {
            capacity += CHUNK;
            *out = realloc(*out, capacity*sizeof(struct line));
            if (*out == NULL) {
                perror("Memory allocation failed\n");
                exit(EXIT_FAILURE);
            }
        }
        processline(templine, &(*out)[*length]);
    }
    free(templine);
}

int main(int argc, char** argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <file-path>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    FILE* filein = fopen(argv[1], "r");
    if (filein == NULL) {
        fprintf(stderr, "Error opening file\n");
        exit(EXIT_FAILURE);
    }
    
    struct line* lines = NULL;
    size_t i = 0;
    
    processlines(filein, &lines, &i);
    fclose(filein);
    
    printf(COLOR_YELLOW"Datatree"COLOR_RESET"\n\n");
    printtree(lines, i);
    printf("\nTotal Nodes: "COLOR_CYAN"%zu\n"COLOR_RESET, i);
    free(lines);

    return EXIT_SUCCESS;
}
