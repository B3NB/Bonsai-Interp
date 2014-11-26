#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#define PROGRAM_SIZE 4096
#define DATA_SIZE    65535
#define LINE_LENGTH  20

#define SUCCESS 0
#define FAILURE 1

enum Opcodes {
    HLT, INC, DEC, JMP, TST, PRT, EOPROGRAM
};

struct statement_t {
    enum Opcodes opcode;
    unsigned short oparg;
};

const char NUMERALS[] = "0123456789";
static struct statement_t PROGRAM[PROGRAM_SIZE]; // Program is stored in here

/// Program printing functions
void printStatement(const struct statement_t * st) {
    switch (st->opcode) {
        case INC: printf("Inc %d\n", st->oparg); break;
        case DEC: printf("Dec %d\n", st->oparg); break;
        case JMP: printf("Jmp %d\n", st->oparg); break;
        case TST: printf("Tst %d\n", st->oparg); break;
        case PRT: printf("Prt %d\n", st->oparg); break;
        case HLT: printf("Hlt \n"); break;
        default: break;
    }
}
void printProgram() {
    int i = 0;
    while (PROGRAM[i].opcode != EOPROGRAM) {
        switch (PROGRAM[i].opcode) {
            case INC: printf("Inc %d\n", PROGRAM[i].oparg); break;
            case DEC: printf("Dec %d\n", PROGRAM[i].oparg); break;
            case JMP: printf("Jmp %d\n", PROGRAM[i].oparg); break;
            case TST: printf("Tst %d\n", PROGRAM[i].oparg); break;
            case PRT: printf("Prt %d\n", PROGRAM[i].oparg); break;
            case HLT: printf("Hlt \n"); break;
            default: break;
        }
        i++;
    }
}

/// String manipulation
int convertStrInt(const char str[]) {
    const char *c = &str[0];
    int n = 0;
    while (*c && *c != '\n') {
        n *= 10;
        n += *c - '0';
        c++;
    }
    return n;
}
bool charIsNumeral(const char *c) {
    const char *l = &NUMERALS[0];
    while (*l)
        if (*c == *l++)
            return true;
    return false;
}
char *firstNumeral(const char str[]) {
    const char *c = &str[0];
    while (!charIsNumeral(c)) ++c;
    return (char *)c;
}

/// Parsing and Execution
int parser(FILE *fp) {
	int lc = 0; // statement counter
    char fl[LINE_LENGTH];
    while (fgets(fl, LINE_LENGTH, fp) != NULL && lc < PROGRAM_SIZE) {
        char oc[4], oa[LINE_LENGTH - 3];
        strncpy(oc, fl, 3);
        
        if      (strcmp(oc, "inc") == 0) PROGRAM[lc].opcode = INC;
        else if (strcmp(oc, "dec") == 0) PROGRAM[lc].opcode = DEC;
        else if (strcmp(oc, "jmp") == 0) PROGRAM[lc].opcode = JMP;
        else if (strcmp(oc, "tst") == 0) PROGRAM[lc].opcode = TST;
        else if (strcmp(oc, "prt") == 0) PROGRAM[lc].opcode = PRT;
        else if (strcmp(oc, "hlt") == 0) PROGRAM[lc].opcode = HLT;
        
        if (PROGRAM[lc].opcode != HLT) {
            strcpy(oa, firstNumeral(fl));
            PROGRAM[lc].oparg = convertStrInt(oa);
        }
        
        lc++;
    }
    if (lc == PROGRAM_SIZE)
        return FAILURE;
    PROGRAM[lc].opcode = EOPROGRAM;
    return SUCCESS;
}
int execute() {
    int i = DATA_SIZE;
    unsigned int data[DATA_SIZE];
    while (--i) data[i] = 0; 
    while (PROGRAM[i].opcode != HLT && i < DATA_SIZE) {
        struct statement_t *cs = &PROGRAM[i];
        //printStatement(cs);
        
        switch (cs->opcode) {
            case INC: data[cs->oparg]++; break;
            case DEC: data[cs->oparg]--; break;
            case JMP: i = cs->oparg - 1; break;             // i++ at bottom 
            case TST: if (data[cs->oparg] == 0) i++; break; // here again
            case PRT: printf("[%d:%d] ", cs->oparg, data[cs->oparg]); break;
            default: break;
        }
        i++;
    }
    return SUCCESS;
}

int main(int argc, const char *argv[]) {
	int status;
	FILE *fp;
    if (argc != 2 || (fp = fopen(argv[1], "r")) == NULL) {
        fprintf(stderr, "usage: %s filename", argv[0]);
		return FAILURE;
	}
	status = parser(fp);
    if (status == SUCCESS) {
        printProgram();
        status = execute();
    }
    if (status == FAILURE) {
        fprintf(stderr, "Error!\n");
    }
	return SUCCESS;
}