#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define START_ADDRESS 2056
#define HEADER_LINES 8

// why LL? we dont know how many labels are there, so table would need resizing
typedef struct Label {
    char name[20];
    int address;
    struct Label *next;
} Label;

Label* head = NULL;

// Add label to label LL
// preprend, i.e L2 -> L1 -> L0 (head)
// why prepend? the easiest insert. allel tail ptr venam
void addLabel(char* name, int address) {
    Label *temp = (Label *)malloc(sizeof(Label));
    strcpy(temp->name, name);
    temp->address = address;
    temp->next = head;
    head = temp;
}

// Find address of a label
int findLabelAddress(char* name) {
    Label* temp = head;
    while (temp != NULL) {
        if (strcmp(temp->name, name) == 0) {
            return temp->address;
        }

        temp = temp->next;
    }

    printf("Error: label %s not found\n", name);
    exit(1);
}
// TASK1: forgotten crossroads (iykyk)
//  Check that ':' belongs to a valid label name.
// Previously, any ':' was treated as a label, so a string like
// "sum is:" was incorrectly detected as a label and removed.
// Now we ensure the part before ':' contains only valid label characters.
int isLabel(char *line, char *label) {
    char *colon = strchr(line, ':');

    if (colon == NULL)
        return 0;

    int length = colon - line;

    if (length <= 0 || length >= 20)
        return 0;

    // Label must contain only letters/digits/underscore
    for (int i = 0; i < length; i++) {
        if (!(line[i] >= 'A' && line[i] <= 'Z') &&
            !(line[i] >= 'a' && line[i] <= 'z') &&
            !(line[i] >= '0' && line[i] <= '9') &&
            line[i] != '_')
            return 0;
    }

    strncpy(label, line, length);
    label[length] = '\0';

    return 1;
}

void firstPass(FILE* file) {
    char line[200]; // one string.
    char label[20]; // one string.

    int address = START_ADDRESS;
    int lineNumber = 0;

    while (fgets(line, sizeof(line), file) != NULL) {
        lineNumber++;

        // First 8 lines are the XSM header.
        // They are NOT instructions.
        if (lineNumber <= HEADER_LINES)
            continue;

        if (isLabel(line, label)) {
            addLabel(label, address);
            continue; // Label != instruction. So dont count it in memory. So do not go to the address += 2 line 
        }

        // Every XSM instruction occupies 2 words.
        address += 2;
    }
}

void secondPass(FILE* input, FILE* output) {
    char line[200];
    int lineNumber = 0;
    
    // start from instruction 1 again
    while (fgets(line, sizeof(line), input) != NULL) {
        lineNumber++;

        // Copy the 8-line XSM header unchanged.
        if (lineNumber <= HEADER_LINES) {
            fprintf(output, "%s", line);
            continue;
        }

        char label[20];

        // skip label declarations, copy everything else
        if (isLabel(line, label)) continue;

        // handle JMP L0
        char instruction[20]; // JMP
        char operand[20]; // Li
        if (sscanf(line, "%19s %19s", instruction, operand) == 2) {
            operand[strcspn(operand, "\r\n")] = '\0';

            if (strcmp(instruction, "JMP") == 0) { 
                fprintf(output, "JMP %d\n", findLabelAddress(operand));
                continue;
            }
        }

        // JZ R0, Li
        // JNZ R0, Li
        char registerName[20];
        char jumpLabel[20];
        if (sscanf(line, "%19s %19[^,], %19s", instruction, registerName, jumpLabel) == 3) {
            jumpLabel[strcspn(jumpLabel, "\r\n")] = '\0';

            if (strcmp(instruction, "JZ") == 0 || strcmp(instruction, "JNZ") == 0) {
                fprintf(output, "%s %s, %d\n", instruction, registerName, findLabelAddress(jumpLabel));
                continue;
            }
        }

        // normal instruction copied as it is
        fprintf(output, "%s", line);
    }
}


int main(int argc, char *argv[]) {
    FILE *input;
    FILE *output;

    if (argc != 3) {
        printf("Usage: %s <input.xsm> <output.xsm>\n", argv[0]);

        return 1;
    }

    input = fopen(argv[1], "r");
    if (input == NULL) {
        printf("Cannot open %s\n", argv[1]);
        return 1;
    }

    // pass 1 - build LL
    firstPass(input);
    rewind(input); // reset the file position indicator

    output = fopen(argv[2], "w");

    if (output == NULL) {
        printf("Cannot create %s\n", argv[2]);
        fclose(input);
        return 1;
    }


    // pass 2 - Remove labels and replace label references with actual addresses. 
    secondPass(input, output);

    fclose(input);
    fclose(output);
    return 0;
}