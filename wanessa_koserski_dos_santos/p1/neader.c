#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "neader.h"

#define HEADER_SIZE 4


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


const char* operation_to_string(Operation op) {
    switch (op) {
        case NOP: return "NOP";
        case STA: return "STA";
        case LDA: return "LDA";
        case ADD: return "ADD";
        case OR: return "OR";
        case AND: return "AND";
        case NOT: return "NOT";
        case JMP: return "JMP";
        case JN: return "JN";
        case JZ: return "JZ";
        case HLT: return "HLT";
        default: return "   ";
    }
}


struct Register {
  u_int8_t ac;
  u_int8_t pc;
  u_int8_t flag_n;
  u_int8_t flag_z;
  u_int8_t memory[256];
  bool end;
};


Register *init() {
  Register *reg = (Register *)malloc(sizeof(Register));
  reg->ac = 0;
  reg->pc = 0;
  reg->flag_n = 0;
  reg->flag_z = 0;
  reg->end = false;
  for (int i = 0; i < 256; i++) {
    reg->memory[i] = 0;
  }

  return reg;
}


bool verify_file(u_int8_t dig1, u_int8_t dig2, u_int8_t dig3, u_int8_t dig4) {
  if (dig1 == 0x03 && dig2 == 0x4E && dig3 == 0x44 && dig4 == 0x52) {
    return true;
  }
  return false;
}


bool set_memory(Register *reg, FILE *file) {
  u_int8_t buffer[516];
  size_t bytesRead;

  while ((bytesRead = fread(buffer, 1, sizeof(buffer), file)) > 0) {

    if (!verify_file((u_int8_t) buffer[0], (u_int8_t) buffer[1], (u_int8_t) buffer[2], (u_int8_t) buffer[3])) {
      printf("Invalid file\n");
      return false;
    }

    for (size_t i = HEADER_SIZE; i < bytesRead; i++) {

      
      if ((i - HEADER_SIZE) % 2 == 0) {
        u_int8_t num = buffer[i];
        reg->memory[(i - HEADER_SIZE) / 2] = num;
        printf("%02X ", num);
      }
      

      /*
      u_int8_t num = buffer[i];
      reg->memory[i - HEADER_SIZE] = num;
      printf("%02X ", num);
      */

      
      if ((i - HEADER_SIZE) % 16 == 15) {
        printf("\n");
      }
    
      
    }

    show_memory(reg);
  }

  return true;
}


void show_memory(Register *reg) {
  printf("AC: %02X\n", reg->ac);
  printf("PC: %02X\n", reg->pc);
  printf("Flag N: %d\n", reg->flag_n);
  printf("Flag Z: %d\n", reg->flag_z);

  for (int i = 0; i < sizeof(reg->memory); i++) {
    printf("[ %02X   %02X    %s ]\n", i, reg->memory[i], operation_to_string(reg->memory[i]));

    /*
    if ((i + 1) % 16 == 0) {
      printf("\n");
    }
    */
  }
}


void destroy(Register *reg) { free(reg); }


void execute(Register *reg) { 
  while (!reg->end) {    
    Operation operation = (Operation) reg->memory[reg->pc];

    switch (operation) {
      case NOP:
          printf("NOP\n");
          reg->pc++;
          break;

      case STA:
          printf("STA\n");
          reg->memory[reg->memory[reg->pc + 1]] = reg->ac;
          reg->pc += 2;
          break;

      case LDA:
          printf("LDA\n");
          reg->ac = reg->memory[reg->memory[reg->pc + 1]];
          reg->pc += 2;
          break;

      case ADD:
          printf("ADD\n");
          reg->ac += reg->memory[reg->memory[reg->pc + 1]];
          reg->pc += 2;
          break;

      case OR:
          printf("OR\n");
          reg->ac |= reg->memory[reg->memory[reg->pc + 1]];
          reg->pc += 2;
          break;

      case AND:
          printf("AND\n");
          reg->ac &= reg->memory[reg->memory[reg->pc + 1]];
          reg->pc += 2;
          break;

      case NOT:
          printf("NOT\n");
          reg->ac = -reg->ac;
          reg->pc++;
          break;

      case JMP:
          printf("JMP\n");
          reg->pc = reg->memory[reg->pc + 1];
          break;

      case JN:
          printf("JN\n");
          if (reg->flag_n == true) {
            reg->pc = reg->memory[reg->pc + 1];
          } else {
            reg->pc += 2;
          }
          break;

      case JZ:
          printf("JZ\n");
          if (reg->flag_z == true) {
            reg->pc = reg->memory[reg->pc + 1];
          } else {
            reg->pc += 2;
          }
          break;

      case HLT:
          printf("HLT\n");
          reg->end = true;
          break;

      default:    
          reg->pc++;
          break;
    }

    if (reg->ac == 0x00) {
      reg->flag_z = true;
    } else {
      reg->flag_z = false;
    }

    if (reg->ac >= 0X80) {
      reg->flag_n = true;
    } else {
      reg->flag_n = false;
    }
  }

  return;
}


int main(int argc, char *argv[]) {
  if (argc != 2) {
      printf("%d\n", argc);
      printf("Expected: ./neader <nome_do_arquivo>\n");
      return 1;
  }

  FILE *file = fopen(argv[1], "rb");

  if (file == NULL) {
    perror("Error: cannot open file");
    return 1;
  }
  
  Register *reg = init();
  set_memory(reg, file);
  
  // printf("\n\nBefore execute: \n");
  // show_memory(reg);

  // printf("\n\nAfter execute: \n");
  execute(reg);
  show_memory(reg);
  
  destroy(reg);

  return 0;
}
