#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdint.h>

const char instructions[9][6] = { "add", "nand", "lw", "sw", "beq", "jalr", "halt", "noop", ".fill" };

int stringCompare(char* input);
int copyArrayContent(int* array1ptr, int* array2ptr, int size); 
void createMachineCode(char* fileName, int* array);
void trim(char * str);
void printArray(int* sourceArrayPtr, int size);
void writeArray(int* sourceArrayPtr, int size, char* mfile);
int checkDuplicate(char** source, int size);
int isCorrectLabel(char* input);

int main(int argc, char **argv)
{
	// a b and m are used in getopt to get the assembly and machine files
	int b;
	int i;
	int o;
	int arraySize = 0; // holds size that we need the result array to be
	char* afile = NULL; // holds assembly file object
	char afile_string[15]; // holds assemblyfile name in string
	char* mfile = NULL; // holds machine file objectt
	char mfile_string; // holds machine file name in a string
	char line[100]; // holds line we are reading in from file
	FILE* fptr; // holds a pointer to assembly file
	int* result; // holds a pointer to the result array
	

	while((b = getopt(argc, argv, "i:o:")) != -1)
	{
		switch(b)
		{
			case 'i':
			{ // takes the assembly file, counts the lines to make the result array, and
			  // calls createMachineCode to put the machine code in result;
				afile = optarg;
				strcpy(afile_string,afile);
				fptr = fopen(afile_string,"r");
				while(fgets(line, 100, fptr) != NULL)
				{
					arraySize++;
				}
				fclose(fptr);
				result =(int*)malloc(arraySize*sizeof(int));
				createMachineCode(afile, result);
				break;
			}
			case 'o':
			{ // if we have a machine code file, write the result to it.
				mfile = optarg;
				writeArray(result, arraySize, mfile);
				break;
			}
			case '?':
			{
				if(optopt == 'a' || optopt == 'm')
				{
					fprintf(stderr, "Option -%c requires an argument.\n", optopt);
				}
				else if( isprint (optopt))
				{
					fprintf(stderr, "Unknown option `-%c'.\n", optopt);
				}
				else
				{
					fprintf(stderr, "uUnknown option character `\\x%x'.\n", optopt);
				}
				return 1;
			}
			default:
			{
				abort();
			}
		}
	}
	// if we do not have a machine file to write to, print to screen
	// if we do not have an assembly file given, print an error 
	if(afile == NULL)
	{
		printf("ERROR: no assembly file given\n");
		exit(-1);
	}
	if(mfile == NULL)
	{
		printArray(result,arraySize);
	}
	
	free(result);
}

void createMachineCode(char* fileName, int* array)
{
	char line[100]; // holds one line of assembly file
	char afile_string[15]; // holds assembly file name in a string 
	char* tok; // holds each part of the line
	FILE* asm_file; // holds pointer to the assembly file
	int line_counter=0; // counts how many lines of assembly code we have
	int label_counter=0; // counts how many labels we have
	int label_length=0; // counts how long the longest label is
	
	strcpy(afile_string,fileName); // opens the asm file
	asm_file = fopen(fileName,"r");
	
	// gets us how many lines in assembly code, how many
	// labels we have, and what the longest label is 
	while(fgets(line, 100, asm_file) != NULL)
	{
		line_counter++;
		tok = strtok(line," \t");
		trim(tok);
		if(label_length < strlen(tok))
		{
			label_length = strlen(tok);
		}
		if(stringCompare(tok)==-1 && strcmp(tok, " ") != 0 && strcmp(tok, "	") !=0)
		{
			label_counter++;
		}
	}

	rewind(asm_file); //g oes back to beginning of file
	int counter=0; // keeps track at where we are in are symbolic values array
	char* symbolic_labels[label_counter]; // array where the symbolic labels will be held
	int symbolic_values[label_counter]; // holds each symbolic value
	int i = 0; //holds where we are in both arrays in both the for and while loops below
	
	for (i=0 ; i<label_counter; i++) 
	{ // allocates the space for the symbolic labels
		symbolic_labels[i] = malloc(sizeof(char) * (label_length+1) );
	}
	
	i = 0;

	//puts each label and its address into 2 arrays
	while(fgets(line, 100,asm_file) != NULL)
	{
		tok = strtok(line," \t");
        if(tok != NULL)
		{
			trim(tok);
		}
		
		isCorrectLabel(tok); // checks to see if the label has the correct format 

		// puts the symbolic values and labels in their respective arrays. Also 
		// error checks to see if there is appropriate whitespace
		if(stringCompare(tok)==-1 && strcmp(tok, " ") != 0 && strcmp(tok, "     ") !=0 )
		{
			if( isalpha(line[0]) == 0 )
			{
				printf("ERROR: if there is a label entered, there can be no whitespace before it\n");
				exit(-1);
			}
			strcpy(*(symbolic_labels + i), tok);
		
			symbolic_values[i] = counter;
			i++;
		}
		else if( stringCompare(tok)!=-1 && isalpha(line[0]) != 0 )
		{
			printf("ERROR: if there is no label, you need to enter in whitespace\n");
			exit(-1);
		}
		counter++;
	}
	//checks to see if we have any duplicate labels and starts 
	checkDuplicate(symbolic_labels,label_counter); 
	rewind(asm_file);
	
	//generate machine language instruction
	int opcode = 0; //holds what the opcode is
	int* result =(int*)malloc(line_counter*sizeof(int));
	int numbers=0; //holds the decimal number for each line
	int shift; //holds how much we have to left shift bits
	int j; //holds how much we have went through a certain line
	int int_tok; //holds the converted token to int
	int k = 0; //holds where we are in result array
	int tempJ;
	int pc = 0;
	int offset = 0;
	int if_label = 0;

    while(fgets(line, 100,asm_file) != NULL)
    {
	    tok = strtok(line," \t");
	    if(tok != NULL)
		{
			trim(tok);
		}
	    if(stringCompare(tok)==-1)
	    {
		    tok = strtok(NULL," \t"); //move token to next value after delimiter
		    if(tok != NULL)
			{
			trim(tok);
			}
	    }
	    opcode = stringCompare(tok);
		if(opcode == -1)
		{
			printf("ERROR: unrecognized opcode = %s\n", tok);
			exit(-1);
		}
	    shift = 22;
	    j = 0;
	    numbers = 0;
	    if( opcode < 2 )
	    {
		    numbers = numbers | (opcode<<shift);
		    shift = shift -3;
		    tok = strtok(NULL, " \t");
			if(tok != NULL)
			{
			trim(tok);
			}
			
		    while( tok != NULL && strcmp(tok,"#")!=0 && strcmp(tok,"") != 0)//(isalpha(tok[0]) != 0 || (tok[0] >= '0' && tok[0] <= '9')))
		    {
				if( !(atoi(tok) >= 0 && atoi(tok)<= 9) )
				{
					printf("ERROR: add and nand fields require you to enter a register number less then 8. You entered: %s\n", tok);
					exit(-1);
				}
			    int_tok = atoi(tok);
			    if(j==0)
			    {
					if( tok[0] == '0' )
					{
						printf("ERROR: you cannot put register 0 in destination field of add\n");
						exit(-1);
					}
				    tempJ = int_tok;
				    j++;
					tok = strtok(NULL, " \t");
					if(tok != NULL)
					{
					trim(tok);
					}
			    }
			    else
			    {
				    numbers = numbers | (int_tok<<shift);
				    shift = shift -3;
				    tok = strtok(NULL, " \t");
				    if(tok != NULL)
					{
					trim(tok);
					}		
					j++;
					
			    }
		    }
			if( j != 3 )
			{
				printf("ERROR: incorrect amount of fields for add or nand. fields entered: %i\n",j);
				exit(-1);
			}
		    numbers = numbers | tempJ;
	    }

	    else if(opcode > 1 && opcode < 5)
	    {
			numbers = numbers | (opcode << shift);
			
			shift = shift - 3;
			tok = strtok(NULL, " \t");
			if(tok != NULL)
			{
			trim(tok);
			}
			offset = 0;
			if_label = 0;
		
			while( tok != NULL && strcmp(tok,"#")!=0 && strcmp(tok,"") != 0)//(isalpha(tok[0]) != 0 || (tok[0] >= '0' && tok[0] <= '9')))
			{
				if(j==2)
				{
					for(int i = 0;i<sizeof(symbolic_values)/sizeof(symbolic_values[0]);i++)
					{
						if(strcmp(tok,*(symbolic_labels + i))==0)
						{
							if( opcode == 4 )
							{
								offset = symbolic_values[i] - (pc+1);
							}
							else
							{
								offset = symbolic_values[i];
							}
							
							if_label = 1;
						}
					}
					if(if_label == 0)
					{
						offset = atoi(tok);
						if( offset == 0 && tok[0] != 48 )
						{
							printf("ERROR: undefined label in lw, sw, or beq: %s\n", tok);
							exit(-1);
						}
						if_label = 1;
					}
					
					if( offset > 32767 || offset < -32768 )
					{
						printf("ERROR: offset field is out of range: %i\n", offset);
						exit(-1);
					}
					offset = (offset >> 31) == 0 ? offset : offset & 0x0000FFFF;
					tok = strtok(NULL, " \t");
					if(tok != NULL)
					{
					trim(tok);
					}
					j++;
				}
				else
				{
					if( j == 0 && tok[0] == '0' && opcode == 2 )
					{
						printf("ERROR: you cannot put register 0 in the first field of a lw\n");
						exit(-1);
					}
					
					if( !(atoi(tok) >= 0 && atoi(tok)<= 9))
					{
						printf("ERROR: lw, sw, and beq first 2 fields require you to enter a register number less then 8. You entered: %s\n", tok);
						exit(-1);
					}
					int_tok = atoi(tok);
					numbers = numbers | (int_tok << shift);
					shift = shift - 3;
					tok = strtok(NULL, " \t");
					if(tok != NULL)
					{
						trim(tok);
					}
					j++;
				}
			}
			
			if( j != 3 )
			{
				printf("ERROR: incorrect amount of fields for lw, sw or beq. fields entered: %i\n",j);
				exit(-1);
			}
			numbers = numbers | offset;

	    }

	    else if(opcode == 5)
	    {
		    numbers = numbers | (opcode<<shift);
			shift = shift -3;
			tok = strtok(NULL, " \t");
			if(tok != NULL)
			{
			trim(tok);
			}
			while( tok != NULL && strcmp(tok,"#")!=0 && j<2 && strcmp(tok,"") != 0)//(isalpha(tok[0]) != 0 || (tok[0] >= '0' && tok[0] <= '9')))
			{
				if( !(atoi(tok) >= 0 && atoi(tok)<= 9) )
				{
					printf("ERROR: jalr requires you to enter a register number less then 8. You entered: %s\n", tok);
					exit(-1);
				}
				if( j == 0 && tok[0] == '0' )
				{
					printf("ERROR: you cannot put register 0 in the first field of a jalr\n");
					exit(-1);
				}
					int_tok = atoi(tok);
					numbers = numbers | (int_tok<<shift);
					shift = shift - 3;
					j++;
					tok = strtok(NULL, " \t");
					if(tok != NULL)
					{
						trim(tok);
					}
			}
			
			if( j != 2 )
			{
				printf("ERROR: incorrect amount of fields for jalr. fields entered: %i\n",j);
				exit(-1);
			}
	    }

	    else if(opcode == 6 || opcode == 7)
	    {
			numbers = numbers | (opcode<<shift);
			tok = strtok(NULL," \t");
			if( tok != NULL )
			{
				trim(tok);
			}
			j++;
			if( tok != NULL )
			{
				if(strcmp(tok,"#")!=0 && strcmp(tok,"") != 0)//(isalpha(tok[0]) != 0 || (tok[0] >= '0' && tok[0] <= '9')))
				{	
					printf("ERROR: incorrect amount of fields for noop or halt. fields entered: %i\n",j);
					exit(-1);
				}
			}
	    }

		else if(opcode == 8)
		{
			tok = strtok(NULL," \t");
			if(tok != NULL)
			{
			trim(tok);
			}
			j++;
			if_label = 0;
			
			for(int i = 0;i<sizeof(symbolic_values)/sizeof(symbolic_values[0]);i++)
			{
			   if(strcmp(tok,*(symbolic_labels + i))==0)
				{
					numbers = numbers | symbolic_values[i];
					if_label = 1;
				}
			}

			if(if_label != 1 )
			{
			   tempJ = atoi(tok);
			   if( tempJ == 0 && tok[0] != 48 )
				{
					printf("ERROR: undefined label in fill: %s\n", tok);
					exit(-1);
				}
				numbers = numbers | atoi(tok);
			}
			tok = strtok(NULL," \t");
			if( tok != NULL )
			{
				trim(tok);
			}
			j++;
			if( tok != NULL )
			{
				if(strcmp(tok,"#")!=0 && strcmp(tok,"")!=0)//(isalpha(tok[0]) != 0 || (tok[0] >= '0' && tok[0] <= '9')))
				{	
					printf("ERROR: incorrect amount of fields for a .fill. fields entered: %i\n",j);
					exit(-1);
				}
			}
		}

		*(result + k) = numbers;
	    k++;
	    pc++;
    }

	copyArrayContent(array,result,line_counter);
	
	free(result);
	
	for (i=0 ; i<label_counter; i++) 
	{ 
		free(symbolic_labels[i]);
	}
	
}


int checkDuplicate(char** source, int size)
{
	for( int i = 0; i < size; i++ )
	{
		for(int j = 1; j < size; j++)
		{
			if (strcmp(*(source + i),*(source + j))==0 &&  (j!= i))
			{
				printf("ERROR: labels must not have duplicates. The duplicate value is %s.\n", *(source + i));
				exit(-1);
			}
		}
	}

	return 0;
}

int isCorrectLabel(char* input)
{
	int i;
	int ch;
	int charcount = 0;
	int specialCharacter = 0;

	for( i = 0; input[i]; i++ )
	{
		ch = input[i];
		if (ch != ' ')
		{
			charcount++;
		}

		if (!((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z')) && !(ch >= '0' && ch <= '9'))
		{
			specialCharacter++;
		}
	}

	if(specialCharacter != 0)
	{
		printf("ERROR: labels must only include letters and numbers. You entered %s.\n", input);
		exit(-1);
	}

	if(isalpha(input[0]) == 0)
	{
		printf("ERROR: labels must start with a character. You entered %s.\n", input);
		exit(-1);
	}

	if(charcount > 6)
	{
		printf("ERROR: labels must not exceed 6 characters. You entered %s\n", input);
		exit(-1);
	}

	return 0;
}


int stringCompare(char* input)
{
	int result;
	result = -1;
	for( int i = 0; i < 9; i++ )
	{
		if (strcmp(input,instructions[i])==0)
		{
			result = i;
		}
	}
	return result;
}


int copyArrayContent(int* array1ptr, int* array2ptr, int size)
{
	memcpy(array1ptr,array2ptr, size*sizeof(int));
}

void printArray(int* sourceArrayPtr, int size)
{
	int j;

	for(j = 0; j < size; j++)
	{
		if(j == size - 1)
		{
			printf("%d", *(sourceArrayPtr + j));
		}
		else
		{
			printf("%d\n", *(sourceArrayPtr + j));
		}
	}

	free(sourceArrayPtr);
}

void writeArray(int* sourceArrayPtr, int size, char* mfile)
{
	int j;
	FILE *fp;
	char outputFile[15];
	
	strcpy(outputFile, mfile);
	fp = fopen(outputFile,"w+");

	for(j = 0; j < size; j++)
	{ 
		if(j == size - 1)
		{
			fprintf(fp,"%d",*(sourceArrayPtr + j));
		}
		else
		{
			fprintf(fp,"%d\n",*(sourceArrayPtr + j));
		}
	
	}

	free(sourceArrayPtr);
}


void trim(char* str)
{
    int index, i;

    index = 0;
    while(str[index] == ' ' || str[index] == '\t' || str[index] == '\n')
    {
        index++;
    }

    i = 0;
    while(str[i + index] != '\0')
    {
        str[i] = str[i + index];
        i++;
    }

    str[i] = '\0'; 

    i = 0;
    index = -1;
    while(str[i] != '\0')
    {
        if(str[i] != ' ' && str[i] != '\t' && str[i] != '\n')
        {
            index = i;
        }

        i++;
    }

    str[index + 1] = '\0';
}

