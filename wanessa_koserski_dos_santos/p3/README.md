# Compilers task

## brainf*

### bfc
- receber uma expressão string que seja uma conta (podendo ter soma, subtração, multiplicação e divisão) 
- output a expessão brainfuck


### bfe
- receber o brainfuck
-  executar e o output ser o resultado da expressão


### executar

Compilar arquivos
```bash
gcc bfc.c -o bfc
gcc bfe.c -o bfe
```

Execute uma expressão matemática
```bash
./bfc "3 + 5 * 2" | ./bfe
```
```bash
output: 13
```
---------
```bash
./bfc "10 / 2 - 1" | ./bfe
```
```bash
output: 4
```
---------
```bash
./bfc "2 * (3 + 4)" | ./bfe
```
```bash
output:14
```
