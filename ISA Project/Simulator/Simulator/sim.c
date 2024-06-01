#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <ctype.h>

// Constants definition ------------------------------------------------------------------------------
#define LENGTH_INST 	16   
#define MEMORY_SIZE 	4096
#define NUM_REGSTERS	16
#define NUM_IO_REGS 	23
#define SIZE_SECTOR     128
#define NUM_SECTOR      128   
#define DISK_CYCLES     1024
#define MONITOR_X       256 
#define MONITOR_Y       256 

// define MACROS
#define ADD  0
#define SUB  1
#define MUL  2
#define AND  3
#define OR   4
#define XOR  5
#define SLL  6
#define SRA  7
#define SRL  8
#define BEQ  9
#define BNE  10
#define BLT  11
#define BGT  12
#define BLE  13
#define BGE  14
#define JAL  15
#define LW   16
#define SW   17
#define RETI 18
#define IN   19
#define OUT  20
#define HALT 21

// Global variables --------------------------------------------------------------------------------------
int MaxMonitorAddr = 0;
int PC = 0;
int irq2_cycle = 0;
int flag_irq = 0;
int timer_disk = 0;
int max_address_dem = 0;
int Registers[NUM_REGSTERS] = { 0 };
int IORegisters[NUM_IO_REGS] = { 0 };
unsigned long long Memory[MEMORY_SIZE] = { 0 };
Memory_Disk[NUM_SECTOR][SIZE_SECTOR] = { 0 };
int Monitor[MONITOR_X * MONITOR_Y] = { 0 };
char* IO_reg_names[NUM_IO_REGS] = { "irq0enable","irq1enable","irq2enable","irq0status", "irq1status"
		, "irq2status","irqhandler","irqreturn", "clks", "leds", "display7seg", "timerenable"
		, "timercurrent", "timermax", "diskcmd", "disksector", "diskbuffer", "diskstatus"
		, "reserved", "reserved", "monitoraddr", "monitordata", "monitorcmd" };
FILE* memin_file = NULL;
FILE* diskin_file = NULL;
FILE* irq2in_file = NULL;
FILE* memout_file = NULL;
FILE* regout_file = NULL;
FILE* trace_file = NULL;
FILE* hwregtrace_file = NULL;
FILE* cycles_file = NULL;
FILE* leds_file = NULL;
FILE* display7seg_file = NULL;
FILE* diskout_file = NULL;
FILE* monitor_file = NULL;
FILE* monitor_yuv_file = NULL;

// Function Declarations ---------------------------------------------------------------------------------
/* A function to open file. if it didn't succeed, it will close all files and exit the program. */
FILE* OpenFile(char f_name[], char f_type[]);
/* Fill up memory array by reading memin.txt file */
void Fill_Memin_Array();
/* Fill up memory_disk array by reading diskin.txt file */
void Fill_Diskin_Array();
/* The function returns the next irq2_cycle from the irq2in_file */
int GetNextIRQ2();
/* The function check if the sign extention is required, and do it if so */
int Convert_Negative_Decimal(unsigned long long num);
/* The function gets the instruction and it's parameters then executes it.
(in addition, update the PC in accordance, write to trace, write to leds_file, write to
display7seg_file, write to hwregtrace_file, update the matrix val of the monitor and the memory disk,
update the irq2_cycle, update the timer then check if interruption of type 2 is required) */
void Execute_Instruction(unsigned long long inst, int imm, int rt, int rs, int rd, int opcode, int format);
/* The function gets the instruction and print the following line to the trace_file:
 PC, instruction and all the registers */
void WriteToTrace(unsigned long long inst);
/* The function checks if timerenable==1, updates the timercurrent register and sets irq0status=1
when timercurrent = timermax */
void UpdateTimer();
/* The function checks if the disk is free to receive new command and performs the disk write and read
commands and update the timer_disk in accordance */
void UpdateAndCheckDisk();
/* The function checks if an interupt is ready to be handled, in that case the fuction update the
PC=irqhandler, irqreturn=PC. and turn off the irq2status */
void Check_Interruption();
/* write to regout_file the final values of Registers */
void WriteToRegout();
/* write to memout_file the final memory array values */
void WriteToMemout();
/* write to cycles_file the final number of cycles */
void WriteToCycles();
/* write to diskout_file the final values of Memory disk array*/
void WriteToDiskout();
/* write to monitor_file the final values of Monitor array */
void WriteToMonitor();
/* function returns 1 if I format, 0 if R format. In addition, it takes care of all the possible options where an instruction
might contain immediate register but the instruction is R format.*/
int Check_Format(unsigned long long inst, int rt, int rs, int rd, int opcode);
/* The function close all the files */
void CloseFiles();


int main(int argc, char* argv[])
{
	if (argc != 14) {
		printf("usage: sim.exe memin.txt diskin.txt irq2in.txt memout.txt regout.txt trace.txt hwregtrace.txt cycles.txt leds.txt display7seg.txt diskout.txt monitor.txt monitor.yuv\n");
		exit(1);
	}

	// Open the input and output files
	memin_file = OpenFile(argv[1], "r");
	diskin_file = OpenFile(argv[2], "r");
	irq2in_file = OpenFile(argv[3], "r");
	memout_file = OpenFile(argv[4], "w");
	regout_file = OpenFile(argv[5], "w");
	trace_file = OpenFile(argv[6], "w");
	hwregtrace_file = OpenFile(argv[7], "w");
	cycles_file = OpenFile(argv[8], "w");
	leds_file = OpenFile(argv[9], "w");
	display7seg_file = OpenFile(argv[10], "w");
	diskout_file = OpenFile(argv[11], "w");
	monitor_file = OpenFile(argv[12], "w");
	monitor_yuv_file = OpenFile(argv[13], "w");
	if (memin_file == NULL || diskin_file == NULL || irq2in_file == NULL || memout_file == NULL || regout_file == NULL || trace_file == NULL || hwregtrace_file == NULL || cycles_file == NULL || leds_file == NULL || display7seg_file == NULL || diskout_file == NULL || monitor_file == NULL || monitor_yuv_file == NULL)
	{
		printf("ERROR: couldn't open files\n");
		exit(1);
	}
	// Read every line of Memin file and save it in array
	Fill_Memin_Array();
	// Read every line of Diskin and save it to Memory_disk array
	Fill_Diskin_Array();

	// Get the num in next line in irq2_file
	irq2_cycle = GetNextIRQ2();
	unsigned long long instruction;
	int   rt, rs, rd, opcode, format;
	int imm = 0;
	// Execute all instructions until halt
	while (1) {
		instruction = Memory[PC];

		rt = instruction & 0xF;
		rs = (instruction >> 4) & 0xF;
		rd = (instruction >> 8) & 0xF;
		opcode = (instruction >> 12) & 0xFF;
		format = Check_Format(instruction, rt, rs, rd, opcode);
		if (format == 1)
		{
			imm = Convert_Negative_Decimal(Memory[PC + 1]);
		}
		Execute_Instruction(instruction, imm, rt, rs, rd, opcode, format);
		if (opcode == HALT) break;
	}

	// Write to the output files
	WriteToMemout();
	WriteToRegout();
	WriteToCycles();
	WriteToDiskout();
	WriteToMonitor();
	// Close all the files
	CloseFiles();
	return 0;
}


void add(int rd, int rs, int rt, int format) {
	Registers[rd] = Registers[rs] + Registers[rt];
	PC = PC + format + 1;
	;
}
void sub(int rd, int rs, int rt, int format) {
	Registers[rd] = Registers[rs] - Registers[rt];
	PC = PC + format + 1;
}
void mul(int rd, int rs, int rt, int format) {
	Registers[rd] = Registers[rs] * Registers[rt];
	PC = PC + format + 1;
}
void and(int rd, int rs, int rt, int format) {
	Registers[rd] = Registers[rs] & Registers[rt];
	PC = PC + format + 1;
}
void or(int rd, int rs, int rt, int format) {
	Registers[rd] = Registers[rs] | Registers[rt];
	PC = PC + format + 1;
}
void xor(int rd, int rs, int rt, int format) {
	Registers[rd] = Registers[rs] ^ Registers[rt];
	PC = PC + format + 1;
}
void sll(int rd, int rs, int rt, int format) {
	Registers[rd] = Registers[rs] << Registers[rt];
	PC = PC + format + 1;
}
void sra(int rd, int rs, int rt, int format) {
	Registers[rd] = Registers[rs] >> Registers[rt];
	PC = PC + format + 1;
}
void srl(int rd, int rs, int rt, int format) {
	Registers[rd] = (int)((unsigned int)Registers[rs] << Registers[rt]);
	PC = PC + format + 1;
}
void beq(int rd, int rs, int rt, int format) {
	if (Registers[rs] == Registers[rt]) {
		PC = (Registers[rd]);

	}
	else {
		PC = PC + format + 1;
	}
}
void bne(int rd, int rs, int rt, int format) {
	if (Registers[rs] != Registers[rt]) {
		PC = (Registers[rd]);
	}
	else {
		PC = PC + format + 1;
	}
}
void blt(int rd, int rs, int rt, int format) {
	if (Registers[rs] < Registers[rt]) {
		PC = (Registers[rd]);
	}
	else {
		PC = PC + format + 1;
	}
}
void bgt(int rd, int rs, int rt, int format) {
	if (Registers[rs] > Registers[rt]) {
		PC = (Registers[rd]);
	}
	else {
		PC = PC + format + 1;
	}
}
void ble(int rd, int rs, int rt, int format) {
	if (Registers[rs] <= Registers[rt]) {
		PC = (Registers[rd]);
	}
	else {
		PC = PC + format + 1;
	}
}
void bge(int rd, int rs, int rt, int format) {
	if (Registers[rs] >= Registers[rt]) {
		PC = (Registers[rd]);

	}
	else {
		PC = PC + format + 1;
	}
}
void jal(int rd, int rs, int rt, int format) {
	Registers[rd] = PC + 1 + format;
	PC = (Registers[rs]);
}
void lw(int rd, int rs, int rt, int format) {
	Registers[rd] = (int)Memory[Registers[rs] + (int)Registers[rt]];
	PC = PC + format + 1;
}
void sw(int rd, int rs, int rt, int format) {
	Memory[Registers[rs] + Registers[rt]] = Registers[rd];
	PC = PC + format + 1;
}
void reti(int rd, int rs, int rt, int format) {
	PC = IORegisters[7];
	flag_irq = 0;
}
void in(int rd, int rs, int rt, int format) {
	Registers[rd] = IORegisters[Registers[rs] + Registers[rt]];
	PC = PC + format + 1;
}
void out(int rd, int rs, int rt, int format) {
	if (Registers[rs] + Registers[rt] == 20)
	{
		if (MaxMonitorAddr < IORegisters[Registers[rs] + Registers[rt]])     //save the last monitor address we wrote to
		{
			MaxMonitorAddr = IORegisters[Registers[rs] + Registers[rt]];
		}
	}
	IORegisters[Registers[rs] + Registers[rt]] = Registers[rd];

	PC = PC + format + 1;
}

FILE* OpenFile(char f_name[], char f_type[]) {
	FILE* Output_file = fopen(f_name, f_type);
	if (Output_file == NULL)
	{
		printf("Error, Cant Open %s file\n", f_name);
		CloseFiles();
		exit(-1);
	}
	return Output_file;
}

void Fill_Memin_Array() {
	char line[LENGTH_INST];
	int pc = 0;
	while (!feof(memin_file) && fgets(line, LENGTH_INST, memin_file)) {
		Memory[pc] = strtoll(line, NULL, 16) & 0xFFFFF;
		pc++;
	}
}



void Fill_Diskin_Array() {
	char line[LENGTH_INST];
	int i = 0, j = 0;
	while (!feof(diskin_file) && fgets(line, LENGTH_INST, diskin_file)) {
		if (j == SIZE_SECTOR) {
			j = 0;
			i++;
		}
		Memory_Disk[i][j] = strtol(line, NULL, 16) & 0xFFFFFFFF;
		j++;
	}
}

int GetNextIRQ2()
{
	int num = 0;
	if (!feof(irq2in_file) && fscanf_s(irq2in_file, "%d ", &num))
		return num;
	if (feof(irq2in_file))
		return -1;
}

int Convert_Negative_Decimal(unsigned long long num) {
	int mask = 0x80000;
	if (mask & num)
	{// convert to negative decimal
		num = -2 * (0x80000) + (num);


	}

	return (int)num;
}

void Execute_Instruction(unsigned long long inst, int imm, int rt, int rs, int rd, int opcode, int format)
{

	Registers[1] = imm;
	int opcode_clk = 0;
	WriteToTrace(inst);
	if (opcode == SW || opcode == LW)  // if opcode is LW or SW, it means there is additional cycle clock to be added
		opcode_clk = 1;
	if ((IORegisters[8] + format + opcode_clk + 1) > (irq2_cycle) && irq2_cycle != -1)  // clks=irq2_cycle
	{
		IORegisters[5] = 1;            // irq2status=1 (irq 2 is triggered)
		irq2_cycle = GetNextIRQ2();

	}
	UpdateTimer();

	// Do the operation
	switch (opcode) {
	case ADD:
		add(rd, rs, rt, format);
		break;
	case SUB:
		sub(rd, rs, rt, format);
		break;
	case MUL:
		mul(rd, rs, rt, format);
		break;
	case AND:
		and(rd, rs, rt, format);
		break;
	case OR:
		or(rd, rs, rt, format);
		break;
	case XOR:
		xor(rd, rs, rt, format);
		break;
	case SLL:
		sll(rd, rs, rt, format);
		break;
	case SRA:
		sra(rd, rs, rt, format);
		break;
	case SRL:
		srl(rd, rs, rt, format);
		break;
	case BEQ:
		beq(rd, rs, rt, format);
		break;
	case BNE:
		bne(rd, rs, rt, format);
		break;
	case BLT:
		blt(rd, rs, rt, format);
		break;
	case BGT:
		bgt(rd, rs, rt, format);
		break;
	case BLE:
		ble(rd, rs, rt, format);
		break;
	case BGE:
		bge(rd, rs, rt, format);
		break;
	case JAL:
		jal(rd, rs, rt, format);
		break;
	case LW:
		lw(rd, rs, rt, format);
		break;
	case SW:
		sw(rd, rs, rt, format);
		break;
	case RETI:
		reti(rd, rs, rt, format);
		break;
	case IN:
		in(rd, rs, rt, format);
		fprintf(hwregtrace_file, "%d READ %s %08X\n", (IORegisters[8] + 1), IO_reg_names[Registers[rs] + Registers[rt]], IORegisters[Registers[rs] + Registers[rt]]);
		break;
	case OUT:  // write to files after updating Input/output registers
		out(rd, rs, rt, format);
		if (Registers[rs] + Registers[rt] == 9)
		{
			fprintf(leds_file, "%d %08X\n", (IORegisters[8] + 1), IORegisters[9]);
		}
		if (Registers[rs] + Registers[rt] == 10)
		{

			fprintf(display7seg_file, "%d %08X\n", (IORegisters[8] + 1), IORegisters[10]);
		}
		if ((Registers[rs] + Registers[rt] == 22) & (IORegisters[Registers[rs] + Registers[rt]] == 1))
		{
			Monitor[IORegisters[20]] = IORegisters[21];    // copy monitor data in monitor array
		}
		fprintf(hwregtrace_file, "%d WRITE %s %08X\n", (IORegisters[8] + 1), IO_reg_names[Registers[rs] + Registers[rt]], IORegisters[Registers[rs] + Registers[rt]]);
		break;
	case HALT:
		break;
	}
	UpdateAndCheckDisk();
	Check_Interruption();
	IORegisters[8] = (IORegisters[8] + 1 + format + opcode_clk) % 0xffffffff;
}

void UpdateTimer() {
	if (IORegisters[11] == 1) {   // timerenable=1
		if (IORegisters[13]) {    // timerMax>0
			if (IORegisters[12] == IORegisters[13]) { // timercurrent=timermax
				IORegisters[3] = 1;  // irq0status=1
				IORegisters[12] = 0; // timercurrent=0
			}
			else
				IORegisters[12]++; // timercurrent++
		}
		else {               // timerMax=0
			if (IORegisters[12] == 0xFFFFFFFF) {
				IORegisters[12] = 0;
				return;
			}
			IORegisters[12]++;     // timercurrent++
		}
	}
}

void UpdateAndCheckDisk() {
	int i = 0;
	if (IORegisters[17] == 0)  // diskstatus=0 (free to receive new command)
	{
		timer_disk = 0;
		if (IORegisters[14] == 2)  // diskcmd=2 (write sector)
		{
			IORegisters[17] = 1;               // diskstatus=1
			for (; i < SIZE_SECTOR; i++) // Memory_Disk[disksector]=Memory[diskbuffer]
			{
				Memory_Disk[IORegisters[15]][i] = (int)Memory[IORegisters[16] + i];

			}
		}
		else if (IORegisters[14] == 1) // diskcmd=1 (read sector)
		{
			IORegisters[17] = 1;               // diskstatus=1
			for (; i < SIZE_SECTOR; i++) // Memory[diskbuffer]=Memory_Disk[disksector]
				Memory[IORegisters[16] + i] = Memory_Disk[IORegisters[15]][i];
		}
	}
	else if (IORegisters[17] == 1) // diskstatus=1 (busy handling a read/write commad)
	{
		timer_disk++;   //update timer cycle

	}
	if (timer_disk == DISK_CYCLES) // 1024 cycles
	{
		timer_disk = 0;
		IORegisters[17] = 0;
		IORegisters[4] = 1;    // irqstatus1=1
		IORegisters[14] = 0;
	}
}

void Check_Interruption() {
	int irq = (IORegisters[0] && IORegisters[3]) || (IORegisters[1] && IORegisters[4]) || (IORegisters[2] && IORegisters[5]);
	if (irq && !flag_irq)
	{
		flag_irq = 1;
		IORegisters[7] = PC;  // irqreturn=PC
		PC = IORegisters[6];  // PC=irqhandler

	}
	IORegisters[5] = 0;       // irq2status=0
}

void WriteToTrace(unsigned long long inst)
{
	fprintf(trace_file, "%03X %05X ", PC, (int)inst);
	for (int i = 0; i < NUM_REGSTERS; i++)
		fprintf(trace_file, "%08X ", Registers[i]);
	fprintf(trace_file, "\n");
}

void WriteToMemout() {
	// Save the maximum address where Memory instruction is not zero.
	int max_address = MEMORY_SIZE - 1;
	for (; max_address >= 0; max_address--)
		if (Memory[max_address] != 0) break;


	for (int i = 0; i <= max_address; i++)
	{
		fprintf(memout_file, "%05X\n", (int)Memory[i]);

	}
}

void WriteToRegout() {
	for (int i = 2; i < NUM_REGSTERS; i++)
		fprintf(regout_file, "%08X\n", Registers[i]);
}

void WriteToCycles() {
	fprintf(cycles_file, "%d\n", (IORegisters[8]));
}

void WriteToDiskout()
{
	// save the last sector position where it contains non zero words.
	int max_i;
	int max_j;
	int flag = 0;
	for (max_i = NUM_SECTOR - 1; max_i >= 0; max_i--) {
		for (max_j = SIZE_SECTOR - 1; max_j >= 0; max_j--) {
			if (Memory_Disk[max_i][max_j]) {

				flag = 1;
				break;
			}
		}
		if (flag) break;
	}

	// Write to diskout_file until last non zero word
	for (int i = 0; i <= max_i; i++) {
		for (int j = 0; j < SIZE_SECTOR; j++) {
			if (i == max_i)
			{
				if (j >= max_j) break;
			}

			fprintf(diskout_file, "%05X\n", Memory_Disk[i][j]);
		}
	}
}

void WriteToMonitor()
{
	for (int i = 0; i < (MaxMonitorAddr + 1); i += 1)
	{

		fprintf(monitor_file, "%02X\n", Monitor[i]);
		fprintf(monitor_yuv_file, "%c", Monitor[i]);


	}

}


int Check_Format(unsigned long long inst, int rt, int rs, int rd, int opcode)
{
	if ((rt == 1) && (opcode == JAL) && (rs != 1))
		return 0;

	if (opcode == BGT || opcode == BLE || opcode == BEQ || opcode == BGE || opcode == BLT || opcode == BNE || opcode == OUT || opcode == IN)
	{
		if ((rd == 1) || (rs == 1) || (rt == 1))
		{

			return 1;

		}
		else
			return 0;
	}
	else if ((rt != 1) && (rs != 1))
	{
		return 0;

	}
	return 1;
}
void CloseFiles() {
	if (memin_file != NULL) fclose(memin_file);
	if (diskin_file != NULL) fclose(diskin_file);
	if (irq2in_file != NULL) fclose(irq2in_file);
	if (memout_file != NULL) fclose(memout_file);
	if (regout_file != NULL) fclose(regout_file);
	if (trace_file != NULL) fclose(trace_file);
	if (hwregtrace_file != NULL) fclose(hwregtrace_file);
	if (cycles_file != NULL) fclose(cycles_file);
	if (leds_file != NULL) fclose(leds_file);
	if (display7seg_file != NULL) fclose(display7seg_file);
	if (diskout_file != NULL) fclose(diskout_file);
	if (monitor_file != NULL) fclose(monitor_file);
	if (monitor_yuv_file != NULL) fclose(monitor_yuv_file);
}