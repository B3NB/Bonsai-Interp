#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#define PROGRAM_SIZE 4096
#define DATA_SIZE    65535
#define LINE_LENGTH  20

#define SUCCESS 0
#define FAILURE 1

enum OP_CODES {
    HLT, INC, DEC, JMP, TST, PRT, EOPROGRAM
};

struct statement_t {
    enum OP_CODE opcode;
    unsigned short oparg;
};

static struct statement_t PROGRAM[PROGRAM_SIZE]; // Program is stored in here

int exp(int a, int b) {
    if (b == 0) return 1;
    return a * exp(a, b - 1);
}

void trimStr(char *new, char *orig, int beg, int end)  {
    for (int i = beg; i < end + 1; i++) {
        new[i - beg] = orig[i];
    }
    new[end - beg + 1] = '\0';
}

int lineLen(char *str) {
    int c = 0;
    bool hasNLine = false;
    while (str[c++] != '\0') {
        if (str[c] == '\n')
            hasNLine = true;
    }
    return c - 1 - hasNLine;
}

int strToInt(char *str, int len) {
    int num = 0, i = 0;
    while (str[i] != '\0' && str[len - i] != ' ') {
        num += (str[len - i] - '0') * exp(10, i);
        i++;
    }
    return num;
}

int parser(FILE *fp) {
	int lc = 0; // statement counter
    char fl[LINE_LENGTH];
    while (fgets(fl, LINE_LENGTH, fp) != NULL && lc < PROGRAM_SIZE) {
        char oc[4], oa[LINE_LENGTH - 3];
        trimStr(oc, fl, 0, 2);

        //printf("%s %d  ", oc, lc);
        
        if      (strcmp(oc, "inc") == 0) PROGRAM[lc].opcode = INC;
        else if (strcmp(oc, "dec") == 0) PROGRAM[lc].opcode = DEC;
        else if (strcmp(oc, "jmp") == 0) PROGRAM[lc].opcode = JMP;
        else if (strcmp(oc, "tst") == 0) PROGRAM[lc].opcode = TST;
        else if (strcmp(oc, "prt") == 0) PROGRAM[lc].opcode = PRT;
        else if (strcmp(oc, "hlt") == 0) PROGRAM[lc].opcode = HLT;

        int len = lineLen(fl);
        if (PROGRAM[lc].opcode != HLT) {
            trimStr(oa, fl, 3, len - 1);
            PROGRAM[lc].oparg = strToInt(oa, len - 3 - 1);
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
        switch (cs->opcode) {
            case INC: data[cs->oparg]++; break;
            case DEC: data[cs->oparg]--; break;
            case JMP: i = cs->oparg - 1; break;             // i++ at bottom 
            case TST: if (data[cs->oparg] == 0) i++; break; // here again
            case PRT: printf("[%d:%d] ", cs->oparg, data[cs->oparg]); break;
        }
        i++;
    }
    return SUCCESS;
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
