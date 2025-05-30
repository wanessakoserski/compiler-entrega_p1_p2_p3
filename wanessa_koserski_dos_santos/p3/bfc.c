#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <locale.h>

void bf_set(int value) {
    printf("[-]"); 
    for (int i = 0; i < value; i++) {
        printf("+");
    }
}

int evaluate_expression(char *expr);

int main(int argc, char *argv[]) {
    setlocale(LC_ALL, "en_US.UTF-8");

    if (argc != 2) {
        fprintf(stderr, "Uso: %s \"expressao\"\n", argv[0]);
        return 1;
    }

    char *input = argv[1];
    char *equals = strchr(input, '=');

    if (equals == NULL) {
        // simples expression
        int result = evaluate_expression(input);
        printf("VALOR:");
        bf_set(result);
    } else {
        // expression with variable
        *equals = '\0';
        char *var_name = input;
        char *expr = equals + 1;

        // clean variable
        while (*var_name == ' ') var_name++;
        char *end = var_name + strlen(var_name) - 1;
        while (end > var_name && *end == ' ') end--;
        *(end + 1) = '\0';

        int result = evaluate_expression(expr);

        // "VAR:VALUE:" format
        printf("VAR:%s:", var_name);
        bf_set(result);
    }

    return 0;
}

int is_operator(char c) {
    return c == '+' || c == '-' || c == '*' || c == '/';
}

int precedence(char op) {
    if (op == '*' || op == '/') 
        return 2;
    
    if (op == '+' || op == '-') 
        return 1;
    
    return 0;
}

int apply_op(int a, int b, char op) {
    switch(op) {
        case '+': 
            return a + b;
        
        case '-': 
            return a - b;
        
        case '*':
            return a * b;
        
        case '/': 
            return b != 0 ? a / b : 0;
    }
    return 0;
}

int evaluate_expression(char *expr) {
    int i;
    int values[100];    
    int val_ptr = 0;

    // operators
    char ops[100];     
    int op_ptr = 0;

    for (i = 0; expr[i]; i++) {
        if (expr[i] == ' ') continue; 

        if (isdigit(expr[i])) {
            int num = 0;
            while (expr[i] && isdigit(expr[i])) {
                num = num * 10 + (expr[i] - '0');
                i++;
            }
            i--;
            values[val_ptr++] = num;
        }
        else if (expr[i] == '(') {
            ops[op_ptr++] = expr[i];
        }
        else if (expr[i] == ')') {
            while (op_ptr > 0 && ops[op_ptr-1] != '(') {
                int val2 = values[--val_ptr];
                int val1 = values[--val_ptr];
                char op = ops[--op_ptr];
                values[val_ptr++] = apply_op(val1, val2, op);
            }
            if (op_ptr > 0) op_ptr--;
        }
        else if (is_operator(expr[i])) {
            while (op_ptr > 0 && precedence(ops[op_ptr-1]) >= precedence(expr[i])) {
                int val2 = values[--val_ptr];
                int val1 = values[--val_ptr];
                char op = ops[--op_ptr];
                values[val_ptr++] = apply_op(val1, val2, op);
            }
            ops[op_ptr++] = expr[i];
        }
    }

    while (op_ptr > 0) {
        int val2 = values[--val_ptr];
        int val1 = values[--val_ptr];
        char op = ops[--op_ptr];
        values[val_ptr++] = apply_op(val1, val2, op);
    }

    return values[0];
}
