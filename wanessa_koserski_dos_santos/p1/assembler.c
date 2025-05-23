#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
  NOP = 0x00,
  STA = 0x10,
  LDA = 0x20,
  ADD = 0x30,
  OR = 0x40,
  AND = 0x50,
  NOT = 0x60,
  JMP = 0x80,
  JN = 0x90,
  JZ = 0xA0,
  HLT = 0xF0
} Operation;

typedef enum {
  SECTION_NONE = 0,
  SECTION_VALUES = 1,
  SECTION_STEPS = 2
} SectionState;

typedef struct {
  char name[32];
  int address;
} Label;

typedef struct {
  char name[32];
  int address;
} Variable;

Variable variables[256];
Label labels[256];
int var_count = 0;
int label_count = 0;
int next_address = 0x91; 

void write_binary_file(const char *filename, uint8_t *memory) {
  FILE *file = fopen(filename, "wb");
  if (!file) {
    perror("Failed to open file for writing");
    exit(EXIT_FAILURE);
  }

  uint8_t header[] = {0x03, 0x4E, 0x44, 0x52};
  fwrite(header, sizeof(uint8_t), 4, file);
  fwrite(memory, sizeof(uint8_t), 512, file);
  fclose(file);
}

int get_variable_address(const char *name) {
  for (int i = 0; i < var_count; i++) {
    if (strcmp(variables[i].name, name) == 0) {
      return variables[i].address;
    }
  }

  if (var_count < 256 && next_address < 256) {
    strncpy(variables[var_count].name, name, 31);
    variables[var_count].address = next_address++;
    var_count++;
    return variables[var_count - 1].address;
  }

  fprintf(stderr, "Error: Too many variables or memory full\n");
  exit(EXIT_FAILURE);
}

int get_label_address(const char *name) {
  for (int i = 0; i < label_count; i++) {
    if (strcmp(labels[i].name, name) == 0) {
      return labels[i].address;
    }
  }
  return -1;
}

int is_valid_name(const char *name) {
  if (!name || !*name)
    return 0;
  if (!isalpha(name[0]))
    return 0;
  for (int i = 1; name[i]; i++) {
    if (!isalnum(name[i]) && name[i] != '_') {
      return 0;
    }
  }
  return 1;
}

int main(int argc, char *argv[]) {
  if (argc != 3) {
    fprintf(stderr, "Usage: %s <input_file.txt> <output_file.mem>\n", argv[0]);
    return EXIT_FAILURE;
  }

  const char *input_filename = argv[1];
  const char *output_filename = argv[2];

  FILE *file = fopen(input_filename, "r");
  if (!file) {
    perror("Failed to open file");
    return EXIT_FAILURE;
  }

  uint8_t memory[512] = {0};
  char line[256];
  SectionState section = SECTION_NONE;
  int pc = 0;

  // Primeira passada: processa labels
  while (fgets(line, sizeof(line), file)) {
    line[strcspn(line, "\n")] = '\0';

    if (strstr(line, ".values")) {
      section = SECTION_VALUES;
    } else if (strstr(line, ".steps")) {
      section = SECTION_STEPS;
      pc = 0;
    } else if (section == SECTION_STEPS) {
      char opcode[4];
      char label_name[32];

      if (sscanf(line, "LBL %31s", label_name) == 1) {
        if (!is_valid_name(label_name)) {
          fprintf(stderr, "Error: Invalid label name '%s'\n", label_name);
          fclose(file);
          return EXIT_FAILURE;
        }

        // O label aponta para o início da PRÓXIMA instrução
        strncpy(labels[label_count].name, label_name, 31);
        labels[label_count].address = pc / 2;
        label_count++;
      } else {
        // Avança o PC conforme a instrução
        if (sscanf(line, "%3s", opcode) == 1) {
          if (strcmp(opcode, "NOP") == 0 || strcmp(opcode, "NOT") == 0 ||
              strcmp(opcode, "HLT") == 0) {
            pc += 2;
          } else {
            pc += 4;
          }
        }
      }
    }
  }

  // Segunda passada: processa instruções e variáveis
  rewind(file);
  section = SECTION_NONE;
  pc = 0;

  while (fgets(line, sizeof(line), file)) {
    line[strcspn(line, "\n")] = '\0';

    if (strstr(line, ".values")) {
      section = SECTION_VALUES;
    } else if (strstr(line, ".steps")) {
      section = SECTION_STEPS;
      pc = 0;
    } else if (section == SECTION_VALUES) {
      char var_name[32];
      int value;

      if (sscanf(line, "SET %31[^,], %d", var_name, &value) == 2) {
        if (!is_valid_name(var_name)) {
          fprintf(stderr, "Error: Invalid variable name '%s'\n", var_name);
          fclose(file);
          return EXIT_FAILURE;
        }

        int address = get_variable_address(var_name);
        memory[address * 2] = (uint8_t)value;
        memory[address * 2 + 1] = 0x00;
      }
    } else if (section == SECTION_STEPS) {
      char opcode[4];
      char operand[32];

      if (sscanf(line, "%3s %31s", opcode, operand) == 2) {
        Operation op;

        if (strcmp(opcode, "LBL") == 0) {
          continue;
        } else if (strcmp(opcode, "NOP") == 0) {
          op = NOP;
        } else if (strcmp(opcode, "STA") == 0) {
          op = STA;
        } else if (strcmp(opcode, "LDA") == 0) {
          op = LDA;
        } else if (strcmp(opcode, "ADD") == 0) {
          op = ADD;
        } else if (strcmp(opcode, "OR") == 0) {
          op = OR;
        } else if (strcmp(opcode, "AND") == 0) {
          op = AND;
        } else if (strcmp(opcode, "NOT") == 0) {
          op = NOT;
        } else if (strcmp(opcode, "JMP") == 0) {
          op = JMP;
        } else if (strcmp(opcode, "JN") == 0) {
          op = JN;
        } else if (strcmp(opcode, "JZ") == 0) {
          op = JZ;
        } else if (strcmp(opcode, "HLT") == 0) {
          op = HLT;
        } else {
          continue;
        }

        memory[pc++] = (uint8_t)op;
        memory[pc++] = 0x00;

        if (op != NOP && op != NOT && op != HLT) {
          int address = get_label_address(operand);
          if (address == -1) {

            if (!is_valid_name(operand)) {
              fprintf(stderr, "Error: Invalid operand '%s'\n", operand);
              fclose(file);
              return EXIT_FAILURE;
            }
            address = get_variable_address(operand);
          }
          memory[pc++] = (uint8_t)address;
          memory[pc++] = 0x00;
        }
      } else if (strcmp(line, "HLT") == 0) {
        memory[pc++] = (uint8_t)HLT;
        memory[pc++] = 0x00;
      }
    }
  }

  fclose(file);
  write_binary_file(output_filename, memory);

  printf("Binary file generated: %s\n", output_filename);
  printf("Variables (starting at 0x91):\n");
  for (int i = 0; i < var_count; i++) {
    printf("  %s -> 0x%02X (%d)\n", variables[i].name, variables[i].address,
           variables[i].address);
  }
  printf("Labels:\n");
  for (int i = 0; i < label_count; i++) {
    printf("  %s -> 0x%02X (%d)\n", labels[i].name, labels[i].address,
           labels[i].address);
  }

  return EXIT_SUCCESS;
}
