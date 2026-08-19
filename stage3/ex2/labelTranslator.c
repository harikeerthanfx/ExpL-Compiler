#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define START_ADDRESS 2056
#define HEADER_LINES 8

typedef struct Label
{
    char name[20];
    int address;
    struct Label *next;
} Label;

Label *head = NULL;


/* Add label to label table */
void addLabel(char *name, int address)
{
    Label *temp = (Label *)malloc(sizeof(Label));

    strcpy(temp->name, name);
    temp->address = address;

    temp->next = head;
    head = temp;
}


/* Find address of a label */
int findLabel(char *name)
{
    Label *temp = head;

    while (temp != NULL)
    {
        if (strcmp(temp->name, name) == 0)
            return temp->address;

        temp = temp->next;
    }

    printf("Error: label %s not found\n", name);
    exit(1);
}


/*
    Check whether the line is actually a label.

    Example:
        L0:
        L1:

    Normal instructions do not contain ':'.
*/
int isLabel(char *line, char *label)
{
    char *colon = strchr(line, ':');

    if (colon == NULL)
        return 0;

    /*
        Make sure the colon occurs immediately after
        the label name, with no spaces.
    */
    int length = colon - line; //length will be 2 (example "L3:")

    if (length <= 0 || length >= 20)
        return 0;

    strncpy(label, line, length); //destination to copy, source , no. of chars to copy count
    label[length] = '\0';

    return 1;
}


/*
    PASS 1

    Build:

        Label -> Address

    XSM instructions start at address 2056.
    Each instruction occupies 2 words.
*/
void firstPass(FILE *file)
{
    char line[200];
    char label[20];

    int address = START_ADDRESS;
    int lineNumber = 0;

    while (fgets(line, sizeof(line), file) != NULL)
    {
        lineNumber++;

        /*
            First 8 lines are the XSM header.
            They are NOT instructions.
        */
        if (lineNumber <= HEADER_LINES)
            continue;


        /*
            Label does not occupy an instruction.
        */
        if (isLabel(line, label))
        {
            addLabel(label, address);
            continue;
        }


        /*
            Every XSM instruction occupies 2 words.
        */
        address += 2;
    }
}


/*
    PASS 2

    Remove labels and replace label references
    with actual addresses.
*/
void secondPass(FILE *input, FILE *output)
{
    char line[200];
    int lineNumber = 0;

    while (fgets(line, sizeof(line), input) != NULL)
    {
        lineNumber++;

        /*
            Copy the 8-line XSM header unchanged.
        */
        if (lineNumber <= HEADER_LINES)
        {
            fprintf(output, "%s", line);
            continue;
        }


        char label[20];

        /*
            Remove label declarations.
        */
        if (isLabel(line, label))
            continue;


        /*
            Handle:

                JMP L0
        */
        char instruction[20];
        char operand[20];

        if (sscanf(line, "%19s %19s", instruction, operand) == 2)
        {
            operand[strcspn(operand, "\r\n")] = '\0';

            if (strcmp(instruction, "JMP") == 0)
            {
                fprintf(output, "JMP %d\n",
                        findLabel(operand));

                continue;
            }
        }


        /*
            Handle:

                JZ R0, L1
                JNZ R0, L1
        */
        char registerName[20];
        char jumpLabel[20];

        if (sscanf(line, "%19s %19[^,], %19s",
                   instruction,
                   registerName,
                   jumpLabel) == 3)
        {
            jumpLabel[strcspn(jumpLabel, "\r\n")] = '\0';

            if (strcmp(instruction, "JZ") == 0 ||
                strcmp(instruction, "JNZ") == 0)
            {
                fprintf(output, "%s %s, %d\n",
                        instruction,
                        registerName,
                        findLabel(jumpLabel));

                continue;
            }
        }


        /*
            Normal instruction:
            copy unchanged.
        */
        fprintf(output, "%s", line);
    }
}


int main(int argc, char *argv[])
{
    FILE *input;
    FILE *output;

    if (argc != 3)
    {
        printf("Usage: %s <input.xsm> <output.xsm>\n",
               argv[0]);

        return 1;
    }


    input = fopen(argv[1], "r");

    if (input == NULL)
    {
        printf("Cannot open %s\n", argv[1]);
        return 1;
    }


    /*
        PASS 1
    */
    firstPass(input);

    rewind(input);


    output = fopen(argv[2], "w");

    if (output == NULL)
    {
        printf("Cannot create %s\n", argv[2]);

        fclose(input);
        return 1;
    }


    /*
        PASS 2
    */
    secondPass(input, output);


    fclose(input);
    fclose(output);

    return 0;
}