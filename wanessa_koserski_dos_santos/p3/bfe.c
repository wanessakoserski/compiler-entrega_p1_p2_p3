#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>

#define MEMORY_SIZE 30000

int main() {
    setlocale(LC_ALL, "en_US.UTF-8");

    unsigned char memory[MEMORY_SIZE] = {0};
    int ptr = 0;
    int c;

    char header[10] = {0};
    int hpos = 0;
    while ((c = getchar()) != EOF && c != ':' && hpos < 9) {
        header[hpos++] = c;
    }
    header[hpos] = '\0';

    char var_name[100] = {0};
    if (strcmp(header, "VAR") == 0) {
        int vpos = 0;
        while ((c = getchar()) != EOF && c != ':' && vpos < 99) {
            var_name[vpos++] = c;
        }
        var_name[vpos] = '\0';
    }

    // read brainfuck code
    char code[10000];
    int pos = 0;
    while ((c = getchar()) != EOF && pos < 9999) {
        if (c == '+' 
            || c == '-' 
            || c == '>' 
            || c == '<' 
            || c == '[' 
            || c == ']' 
            || c == '.' 
            || c == ',') 
        {
            code[pos++] = c;
        }
    }
    code[pos] = '\0';

    // Executa o código Brainfuck
    int loop_stack[1000], loop_ptr = 0;
    for (int i = 0; code[i]; i++) {
        switch (code[i]) {
            case '>': 
                ptr++; 
                break;
            
            case '<': 
                ptr--; 
                break;
            
            case '+': 
                memory[ptr]++; 
                break;
            
            case '-': 
                memory[ptr]--; 
                break;
            
            case '.': 
                putchar(memory[ptr]); 
                break;
            
            case ',': 
                memory[ptr] = getchar(); 
                break;
            
            case '[':
                if (memory[ptr] == 0) {
                    int depth = 1;
                    while (depth > 0 && code[++i]) {
                        if (code[i] == '[') depth++;
                        else if (code[i] == ']') depth--;
                    }
                } else {
                    loop_stack[loop_ptr++] = i;
                }
                break;
            
            case ']':
                if (memory[ptr] != 0) {
                    i = loop_stack[loop_ptr - 1];
                } else {
                    loop_ptr--;
                }
                break;
        }
    }

    if (strcmp(header, "VAR") == 0) {
        printf("%s = %d\n", var_name, memory[0]);
    } else {
        printf("%d\n", memory[0]);
    }

    return 0;
}
