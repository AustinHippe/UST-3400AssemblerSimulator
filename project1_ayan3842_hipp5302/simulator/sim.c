#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdint.h>

typedef struct state_struct {
	int pc;
	int* mem; //[NUMMEMORY()];
	int* reg; //[8];
	int num_memory;
	int instructionCount;
} statetype;

void simulateAsm(statetype *(stateSimulater),int machineCode);
void print_stats(int n_instrs);
int convert_num(int num);


void print_state(statetype *stateptr){
	int i;
	printf("\n@@@\nstate:\n");
	printf("\tpc %d\n", stateptr->pc);
	printf("\tmemory:\n");
	for(i = 0; i < stateptr->num_memory; i++){
		printf("\t\tmem[%d]=%d\n", i, stateptr->mem[i]);
	}
	printf("\tregisters:\n");
	for(i = 0; i < 8; i++){
		printf("\t\treg[%d]=%d\n", i, stateptr->reg[i]);
	}
	printf("end state\n");
}



int main(int argc, char **argv)
{
	// i is used in getopt to get the machine file
	int i;
	int j=0;
	int b;
	char* ifile = NULL; // holds assembly file object
	char ifile_string[15]; // holds assemblyfile name in string
	int n_instrs = 0; // int that holds how many intstructions we have did
	int counter = 0; // counts how many lines in the file and puts it in nummemory
	FILE* fptr; // pointer to the machine code file
	char line[100]; // holds line we are reading in from file
	statetype* state_struct; /* holds the struct that storest the pc, memory, registers and how much memory we have */
	int end = 0; // tells us when to end the program
	int machineCode = 0;
	int instructionCount = 0;

	while((b = getopt(argc, argv, "i:")) != -1)
	{
		switch(b)
		{
			case 'i':
			{ 
				ifile = optarg;
				break;
			}
			case '?':
			{
				if(optopt == 'i' )
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
		//malloc space for struct adn read in file see how many lines file has and set to num_memory

		state_struct = (statetype*)malloc(sizeof(statetype));
		strcpy(ifile_string,ifile);
		fptr = fopen(ifile_string,"r");
		while(fgets(line, 100, fptr) != NULL)
		{
			counter++;
		}
		
		state_struct->instructionCount = 0;
		
		state_struct->num_memory = counter;
		
		//set initial value of program counter to 0
		state_struct->pc = 0;
		
		//allocate size 8 to reg
		state_struct->reg = (int*)malloc(8*sizeof(int*));
		
		//set initial value of registers to 0, reg 0 must always be set to 0
		for(int i = 0; i < 8; i++)
		{
			*(state_struct->reg + i) = 0;
		}
		
		//allocate space for memory using num_memory
		state_struct->mem = (int*)malloc(state_struct->num_memory*sizeof(int*));
		fclose(fptr);
		
		//set memory values to 0
		for(int i = 0; i < state_struct->num_memory; i++)
		{
			*(state_struct->mem + i) = 0;
		}
		
		print_state(state_struct);
		
		
		//populate memory with machine code at respective address
		fptr = fopen(ifile_string,"r");
		while(fgets(line, 100, fptr) != NULL )
		{
			machineCode = atoi(line);
			*(state_struct->mem + j) = machineCode;
			j = j + 1;
		}
		fclose(fptr);
		
		//for each address value in memory, call simulateAsm function
		for(int i = state_struct->pc; i < state_struct->num_memory; i = state_struct->pc)
		{
			state_struct->instructionCount = state_struct->instructionCount + 1;
			simulateAsm(state_struct,*(state_struct->mem + i));
			print_state(state_struct);
			state_struct->pc = state_struct->pc + 1;
		}
			
		return 0;
	}


}

int convert_num(int num)
{
	if (num & (1<<15) ) 
	{
		num -= (1<<16);
	}
	return(num);
}


void simulateAsm(statetype *(stateSimulater),int machineCode)
{
	int opcode;
	int regA;
	int regB;
	int destReg;
	int destIndex;
	int offset;
	
	
	opcode = (machineCode >> 22) & 0x7;
	
	// add
	if(opcode == 0)
	{
		regA = (machineCode >> 19) & 0x7;
		regB = (machineCode >> 16) & 0x7;
		destIndex = (machineCode & 0x7);
		printf("regA: %d    regB: %d    destReg: %d\n", regA,regB,destIndex); 
		destReg = *(stateSimulater->reg + regA) + *(stateSimulater->reg + regB);
		*(stateSimulater->reg + destIndex) = destReg;
	}
	
	// nand, needs test
	else if(opcode == 1)
	{
		regA = (machineCode >> 19) & 0x7;
		regB = (machineCode >> 16) & 0x7;
		destIndex = (machineCode & 0x7);
		destReg = ~(*(stateSimulater->reg + regA) & *(stateSimulater->reg + regB));
		*(stateSimulater->reg + destIndex) = destReg;
	}
	
	// lw
	else if(opcode == 2)
	{
		destIndex = (machineCode >> 19) & 0x7;
		regA = (machineCode >> 16) & 0x7;
		offset = convert_num(machineCode & 0xFFFF);
		destReg = *(stateSimulater->mem + (*(stateSimulater->reg + regA) + offset));
		*(stateSimulater->reg + destIndex) = destReg;
	}
	
	// sw
	else if(opcode == 3)
	{
		regA = (machineCode >> 19) & 0x7;
		regB = (machineCode >> 16) & 0x7;
		offset = convert_num(machineCode & 0xFFFF);
		*(stateSimulater->mem + (regB + offset)) = *(stateSimulater->reg + regA);
	}
	
	// beq
	else if(opcode == 4)
	{
		
		regA = (machineCode >> 19) & 0x7;
		regB = (machineCode >> 16) & 0x7;
		offset = convert_num(machineCode & 0xFFFF);
		
		if(*(stateSimulater->reg + regA) == *(stateSimulater->reg + regB))
		{
			stateSimulater->instructionCount = stateSimulater->instructionCount + 1;
			stateSimulater->pc = stateSimulater->pc + 1 + offset;
			simulateAsm(stateSimulater,*(stateSimulater->mem + (stateSimulater->pc)));
		}
	}

	// jalr
	else if(opcode == 5)
	{
		regA = (machineCode >> 19) & 0x7;
		regB = (machineCode >> 16) & 0x7;
		
		//stateSimulater->pc = stateSimulater->pc + 1;
		*(stateSimulater->reg + regA) = stateSimulater->pc + 1;
		stateSimulater->pc = *(stateSimulater->reg + regB) -1;
	}
	
	// halt
	else if(opcode == 6)
	{
		print_state(stateSimulater);
		print_stats(stateSimulater->instructionCount);
		exit(-1);
	}
	
	// noop
	else if(opcode == 7)
	{
		//do nothing
	}

}

void print_stats(int n_instrs)
{
	printf("INSTRUCTIONS: %d\n", n_instrs); // total executed instructions
}
