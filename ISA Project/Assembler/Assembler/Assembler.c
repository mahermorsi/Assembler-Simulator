#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_LINE 10000

typedef struct Label
{

	char Name_Of_Label[50];
	// location will store the line number and will be the immediate value in related jump and branch commands
	int Label_Location;
	// a pointer to the next label
	struct Label* next;
} label;


label* make_label(char name[50], int location)
{
	// allocate memory for the label and create a pointer to it
	label* new_label = (label*)malloc(sizeof(label));
	// if allocation successful. insert data to label
	if (new_label != NULL) {
		// use strcpy to insert a string
		strcpy(new_label->Name_Of_Label, name);
		// the other insertions are easy
		new_label->Label_Location = location;
		new_label->next = NULL;
	}
	return new_label;
}

// adding label to the front of the existing label
label* AddLabel(label* head, char name[50], int location) {
	// build the label and check for success. otherwise return a null
	label* new_label = make_label(name, location);
	if (new_label == NULL)
		return NULL;
	// the next pointer will point to the original head
	new_label->next = head;

	return new_label;
}

// this function will scan the label list "head" and look for "name". it will return it's location.
int Find_Label_Name(label* head, char name[50]) {
	// current - the current label's name
	char current[50];
	strcpy(current, head->Name_Of_Label);
	while (strcmp(current, name) != 0) {
		head = head->next;
		// in case not found - return -1
		if (head == NULL) {
			return -1;
		}
		strcpy(current, head->Name_Of_Label);
	}
	return head->Label_Location;
}

// this func destroys the list and frees the memory
void DestroyList(label* head)
{
	label* temporary;
	// all the way to the end
	while (head != NULL)
	{
		// temp gets the current node while head advances
		temporary = head;
		head = head->next;
		// we destroy temp and free the memory
		free(temporary);
	}
}

//  this struct will save the memory unit (format) from the line
typedef struct MemoryUnit {
	// the opcode of the command
	char opcode[6];
	// the registers
	char rd[6];
	char rs[6];
	char rt[6];
	// immediate value
	char imm[50];
	// position of character in line
	int pos;
	// memory will also be a linked list to support infinite length programs
	struct MemoryUnit* next;
}MemoryUnit;

// creates new memory unit 
MemoryUnit* Make_Memory_Unit(char opcode[6], char rd[6], char rs[6], char rt[6], char imm[50], int pos) {
	// allocate memory for the label and create a pointer to it
	MemoryUnit* New_Memory_Unit = (MemoryUnit*)malloc(sizeof(MemoryUnit));
	// if allocation successful. insert data to label
	if (New_Memory_Unit != NULL) {
		// use strcpy to insert the strings
		strcpy(New_Memory_Unit->opcode, opcode);
		strcpy(New_Memory_Unit->rd, rd);
		strcpy(New_Memory_Unit->rs, rs);
		strcpy(New_Memory_Unit->rt, rt);
		strcpy(New_Memory_Unit->imm, imm);
		New_Memory_Unit->pos = pos;
		// no next defined
		New_Memory_Unit->next = NULL;
	}
	return New_Memory_Unit;
}

// adds memoty unit to memory as struct. this line will be added to the end to let the writing run it like an array
MemoryUnit* Add_Memory_Unit(MemoryUnit* head, char opcode[6], char rd[6], char rs[6], char rt[6], char imm[50], int pos)
{

	MemoryUnit* tail;
	MemoryUnit* new_line = Make_Memory_Unit(opcode, rd, rs, rt, imm, pos);
	// if the new line is null. do nothing
	if (new_line == NULL)
		return NULL;
	// and return the current if no head supplied
	if (head == NULL)
		return new_line;
	// get the "tail" to the end of the list
	tail = head;
	while (tail->next != NULL)
		tail = tail->next;
	// add the new line
	tail->next = new_line;
	// return updated memory
	return head;
}

// destroy the memory line list and free the memory the assembler used
void destroy_memLine(MemoryUnit* head) {
	// temp - a pointer to a line we are going to destroy after updating head
	MemoryUnit* temporary;
	// all the way to the end
	while (head != NULL)
	{
		// temp gets the current node while head advances
		temporary = head;
		head = head->next;
		// we destroy temp and free the memory
		free(temporary);
	}
}

// get the memory line at position. can return null if does not exist
MemoryUnit* getAtPosition(MemoryUnit* head, int pos) {
	// go until you find
	while (head != NULL && head->pos != pos)
		head = head->next;
	return head;
}

// Memory struct and related functions. it is used so the second iteration can return two values.
typedef struct Memory
{
	// head of memory line list
	MemoryUnit* head;
	// position of last
	int last;
}Memory;

//create memory structure
Memory* make_mem(MemoryUnit* head, int pos1)
{
	Memory* mem = (Memory*)malloc(sizeof(Memory));

	if (mem != NULL)
	{
		mem->head = head;
		mem->last = pos1;
	}

	return(mem); //Return number of lines
}

// destroys the memory struct after use
void destroy_mem(Memory* mem) {
	// destroy the memory's line list
	destroy_memLine(mem->head);
	// free the memory object's own memory
	free(mem);
}
void ReadOpcodeFromLine(char line[MAX_LINE], char* option, int* Line_Index) {

	char tav; // current read char at index k
	int j = 0; // index of copied char
	do { // reading opcode should continue till dollar of first register
		tav = line[*Line_Index]; // read current
		if (tav != '$') // if it's not dollar
		{
			if (tav != '\t') // or whitespace
				if (tav != ' ')
				{
					option[j] = tolower(tav); // copy
					j = j + 1;
				}
			*Line_Index = *Line_Index + 1;
		}
	} while (tav != '$');
	option[j] = '\0'; // null terminate the opcode
}
// reads dollar sign betwen registers , the func stops untill the first dollar sign
void ReadDollarFromLine(char line[MAX_LINE], int* k) {
	while (line[*k] != '$') *k = *k + 1; // simply make your way to the dollar then stop
}
// reads register value
char* ReadRegistersFromLine(char line[MAX_LINE], char* rdst, int* Line_Index) {
	int j = 0; // Read rd
	while (line[*Line_Index] != ',')
	{
		if (line[*Line_Index] != ' ' && line[*Line_Index] != '\t') { // read if not a whitespace
			rdst[j] = tolower(line[*Line_Index]);
			j = j + 1;
		}
		*Line_Index = *Line_Index + 1;
	}
	rdst[j] = '\0';// null terminate
	return rdst; // return the string
}
//read immediate
char* ReadimmdFromLine(char line[MAX_LINE], char* imm, int* LineIndex) {
	// go to immediate


	while ((line[*LineIndex] == ' ') || (line[*LineIndex] == '\t') || (line[*LineIndex] == ',')) {
		*LineIndex = *LineIndex + 1;
	}
	int j = 0; // index of char being copied in immediate string
	while (line[*LineIndex] != ' ')
	{
		if (line[*LineIndex] != ' ' && line[*LineIndex] != '\t') {
			if ((line[*LineIndex] == '\t') || (line[*LineIndex] == '#') || (line[*LineIndex] == '\n'))	break;
			imm[j] = tolower(line[*LineIndex]);
			j = j + 1;
		}
		*LineIndex = *LineIndex + 1;
	}
	imm[j] = '\0';

	return imm;
}
// part 2 - scanning the files and get the data 
// the code  trough the file line by line and looks for labels, then adds them to the label list according to thier position
label* makeLabelList(FILE* asembl) {
	// lineindex is the char index for label name read, j is the index in the label name string we are building, check is determining if it's a label 
	// counter will be the line position 
	int rowIndex = 0, LineIndex, j, option, counter = 0, check = 0;
	// line the current line being read, tav1 is the first char and i used to check for remarks, // tav - current char when reading label name
	// label_line will contain the name of the label once iteration is complete, dots are used to say "this is a label"
	char line[MAX_LINE], tav1, tav, lable_line[50], dots[50];
	strcpy(dots, ":");
	label* head = NULL;
	while (!feof(asembl)) {
		check = 0;
		fgets(line, MAX_LINE, asembl);  // read a command from the assembler file
		LineIndex = 0;
		option = 0;  // reset option
		while (line[LineIndex] == '\t' || line[LineIndex] == ' ') LineIndex++;
		if (line[LineIndex] == '\n')
		{
			continue;

		}
		if (strcmp(line, "\n") == 0)
		{
			continue;  //If line is blank, continue
		}
		tav1 = line[LineIndex];
		if (tav1 == '#') continue;  //If line is Remark, continue
		if (strstr(line, ".word") != NULL) continue;  //If line is .word, continue
		if (strstr(line, dots) != NULL)  //If dots are found, this is a label
		{
			if (strstr(line, "#") != NULL) // however, ":" can be in a remark. so check for that as well, if so go to another line
				if ((strstr(line, dots)) > (strstr(line, "#"))) continue;
			j = 0;  //Read the label name, first reset indexes
			do {
				tav = line[LineIndex];  // get current char
				// two dots is where it ends so skip
				if (tav != ':') {
					if (tav != '\t' && tav != ' ')   // don't read tabs and spaces
					{
						lable_line[j] = tolower(tav);  // grab the read char and put it in name string
						j++;                            // increment name string index
					}
					LineIndex++;                    // increment reading index
				}
			} while (tav != ':');
			lable_line[j] = '\0';              // label name is null terminated
			LineIndex++;   // Check if the line is lable line only by seeing if there are only spaces and tabs till the end
			while ((line[LineIndex] == ' ') || (line[LineIndex] == '\t'))  LineIndex++;
			option = ((line[LineIndex] == '\n') || (line[LineIndex] == '#'));  // option is 1 on label only line, otherwise 0
			head = AddLabel(head, lable_line, counter);   // finally we add the label to label list
			// Only label line - add label and decrement counter

		}

		if (strstr(line, "$imm") != NULL) // check if line contains $imm
		{

			int i = 0;
			char opcode[6], rd[6], rs[6], rt[6], imm[50];
			ReadOpcodeFromLine(line, &opcode, &i);
			ReadRegistersFromLine(line, &rd, &i);
			ReadDollarFromLine(line, &i);                           // wait for dollar sign
			ReadRegistersFromLine(line, &rs, &i);                    // Read rs
			ReadDollarFromLine(line, &i);                           // wait for dollar sign
			ReadRegistersFromLine(line, &rt, &i);					// Read rt
			ReadimmdFromLine(line, &imm, &i);


			if (((strstr(line, "$imm") < (strstr(line, "#"))) || (strstr(line, "#") == NULL)))
			{
				if (strcmp(rt, "$imm") == 0 && (strcmp(opcode, "jal") == 0) && (strcmp(rs, "$imm") != 0))
				{
					counter++;

					continue;
				}
				if ((strcmp(opcode, "ble") == 0) || (strcmp(opcode, "beq") == 0) || (strcmp(opcode, "bge") == 0) || (strcmp(opcode, "blt") == 0) || (strcmp(opcode, "bne") == 0) || (strcmp(opcode, "out") == 0) || (strcmp(opcode, "in") == 0) || (strcmp(opcode, "bgt") == 0))
				{
					if ((strcmp(rd, "$imm") == 0) || (strcmp(rs, "$imm") == 0) || (strcmp(rt, "$imm") == 0))
					{
						counter = counter + 2;
						continue;
					}

					else
					{


						counter++;

						continue;
					}

				}
				else if ((strcmp(rt, "$imm") != 0) && (strcmp(rs, "$imm") != 0))
				{
					counter++;

					continue;
				}
				counter = counter + 2;

				continue;
			}

		}
		if (option == 0)
		{

			counter++;

		}


	}




	return head;   // return the list
}


// this function scan the file and save the "word"  arguments (data and position)
MemoryUnit* Get_word_Arguments(MemoryUnit* head, char line[MAX_LINE], int* pos1, int* LineIndex) {
	char wordPosition[15], WordValue[15]; // wordP - address, wordN - data
	int j = 0; // index for string we copy to
	*LineIndex = 0;             // reset k index
	while (line[*LineIndex] == ' ' || line[*LineIndex] == '\t') *LineIndex = *LineIndex + 1;            // go past all the spaces
	j = 0; //Copy Address. first reset j then copy char char until the next space
	while (line[*LineIndex] != ' ') {
		*LineIndex = *LineIndex + 1;
	}
	while (line[*LineIndex] == ' ') *LineIndex = *LineIndex + 1;
	while (line[*LineIndex] != ' ') {
		wordPosition[j] = line[*LineIndex];
		j = j + 1; *LineIndex = *LineIndex + 1;
	}
	wordPosition[j] = '\0';
	while (line[*LineIndex] == ' ' || line[*LineIndex] == '\t') *LineIndex = *LineIndex + 1; // terminate string with null and increment end to next char
	j = 0; //Copy Data. using the same way.
	while (line[*LineIndex] != ' ') {
		if (line[*LineIndex] == '\n') break;
		WordValue[j] = line[*LineIndex];
		j = j + 1; *LineIndex = *LineIndex + 1;
	}
	WordValue[j] = '\0';
	int pos = 0; // pos - current line address, can be hexadecimal or decimal
	if (wordPosition[0] == '0') { //  change Address int. the if block considers an hexadecimal input
		if (wordPosition[1] == 'x' || wordPosition[1] == 'X')
			pos = strtol(wordPosition, NULL, 16);
	}
	else pos = atoi(wordPosition);
	// and the else blocks considers a decimal input
	// now. we will save the command in the memory list. NONO will be used as an indicator when writing to turn the command into a .word
	char nono[5] = "NONO"; // a string used to copy nono to required places. fifth char is null
	strcpy(nono, "NONO"); //strcpy(nono, "NONO"); strcpy(nono, "NONO"); strcpy(nono, "NONO");
	head = Add_Memory_Unit(head, nono, nono, nono, nono, WordValue, pos);  // save line to line list. wordN - the immediate value, is used as the data
	if (pos > *pos1) *pos1 = pos;
	// update the location of the end of the memory
	return head;
}

MemoryUnit* readLine(char* line, int* pos1, int* i, MemoryUnit* head, int* k) {
	char option[6], rd[6], rs[6], rt[6], imm[50]; // the line's properties

	ReadOpcodeFromLine(line, option, k);					// read the opcode
	ReadRegistersFromLine(line, rd, k);                       // Read rd
	ReadDollarFromLine(line, k);                           // wait for dollar sign
	ReadRegistersFromLine(line, rs, k);                    // Read rs
	ReadDollarFromLine(line, k);                           // wait for dollar sign
	ReadRegistersFromLine(line, rt, k);                    // Read rt
	ReadimmdFromLine(line, imm, k);                 //handle immediate
	head = Add_Memory_Unit(head, option, rd, rs, rt, imm, *i);	*i = *i + 1;					// save line to line list
	if (*i > *pos1) *pos1 = *i;  //Update last line position
	return head;
}

// this func scan the text and save the lines as memory units using the structs we have defined above 
Memory* Save_Lines_As_memory_units(FILE* file) {

	int LineIndex = 0, i = 0, pos1 = 0;
	char line[MAX_LINE], tav1, * dots = ":";
	MemoryUnit* head = NULL;  // the Memory list's head. it will contain info about each memory line in the end
	while ((fgets(line, MAX_LINE, file)) != NULL) {  // the loop reads the file line by line. and upon reaching null it stops as that's where the file ends
		LineIndex = 0;
		while ((line[LineIndex] == ' ') || (line[LineIndex] == '\t')) LineIndex++;
		if (line[LineIndex] == '\n' || line[LineIndex] == '#' || line[LineIndex] == '\0')
			continue;
		if (strcmp(line, "\n") == 0) continue;  // in case of a Blank line, go
		tav1 = line[LineIndex];          // get first line
		if (tav1 == '#') continue;  // in case of a Remark line, go
		char wo[6] = ".word"; // a string for comparison
		int isword = 0; // booleand for .world detection
		if (strstr(line, wo) != NULL) { // in case of the special .word order
			head = Get_word_Arguments(head, line, &pos1, &LineIndex);
			isword = 1;
		}

		else if (strstr(line, dots) != NULL) {     //in case of regular order and label
			if (strstr(line, "#") != NULL) {       //now we check if the dots is remark and not a label
				if ((strstr(line, dots)) >= (strstr(line, "#"))) {        //label line- check if the line include only label or order
					goto mark; // start reading
				}
			} // the following code section will work if the there is mark # and apears aftre dots or if there isn't mark #
			//k = 0;
			while (line[LineIndex] != ':') LineIndex++;
			LineIndex++;
			if (line[LineIndex] == '\n') continue;
			else
				while ((line[LineIndex] == ' ') || (line[LineIndex] == '\t')) LineIndex++;
			if (line[LineIndex] == '\n') continue;
			if (line[LineIndex] == '#')	continue;
			if (line[LineIndex] == '\0') continue;

			if (i > pos1)
			{
				pos1 = i;

			}//Update last line position
		}
		else// Order line only
		{
			LineIndex = 0;
			while ((line[LineIndex] == ' ') || (line[LineIndex] == '\t')) LineIndex++;  // roll to end of spaces
			if (line[LineIndex] == '#')	continue;
			if (line[LineIndex] == '\n') continue;
			if (line[LineIndex] == '\0') continue;
		}
		if (!isword) { // copy line in all not .world scenarios
		mark:
			head = readLine(line, &pos1, &i, head, &LineIndex);
		}
	}
	return make_mem(head, pos1);  // create memory structure and return it to main function
}


void print_rd_rs_rt(char* rdst, FILE* memin, int* num)
{ // basically a big if block that checks the name of the register and converts it to a number
	if (strcmp(rdst, "$zero") == 0)
		fprintf(memin, "0");
	else if (strcmp(rdst, "$imm") == 0)
		fprintf(memin, "1");
	else if (strcmp(rdst, "$v0") == 0)
		fprintf(memin, "2");
	else if (strcmp(rdst, "$a0") == 0)
		fprintf(memin, "3");
	else if (strcmp(rdst, "$a1") == 0)
		fprintf(memin, "4");
	else if (strcmp(rdst, "$a2") == 0)
		fprintf(memin, "5");
	else if (strcmp(rdst, "$a3") == 0)
		fprintf(memin, "6");
	else if (strcmp(rdst, "$t0") == 0)
		fprintf(memin, "7");
	else if (strcmp(rdst, "$t1") == 0)
		fprintf(memin, "8");
	else if (strcmp(rdst, "$t2") == 0)
		fprintf(memin, "9");
	else if (strcmp(rdst, "$s0") == 0)
		fprintf(memin, "A");
	else if (strcmp(rdst, "$s1") == 0)
		fprintf(memin, "B");
	else if (strcmp(rdst, "$s2") == 0)
		fprintf(memin, "C");
	else if (strcmp(rdst, "$gp") == 0)
		fprintf(memin, "D");
	else if (strcmp(rdst, "$sp") == 0)
		fprintf(memin, "E");
	else if (strcmp(rdst, "$ra") == 0)
		fprintf(memin, "F");
	else if (strcmp(rdst, "NONO") == 0)
		*num = 0;
	else
		fprintf(memin, "0");
}


int printopcode(char* opc, FILE* memin)
{
	if (strcmp(opc, "add") == 0) {
		fprintf(memin, "00"); return 1;
	}
	else if (strcmp(opc, "sub") == 0) {
		fprintf(memin, "01"); return 1;
	}
	else if (strcmp(opc, "mul") == 0) {
		fprintf(memin, "02"); return 1;
	}
	else if (strcmp(opc, "and") == 0) {
		fprintf(memin, "03"); return 1;
	}
	else if (strcmp(opc, "or") == 0) {
		fprintf(memin, "04"); return 1;
	}
	else if (strcmp(opc, "xor") == 0) {
		fprintf(memin, "05"); return 1;
	}
	else if (strcmp(opc, "sll") == 0) {
		fprintf(memin, "06"); return 1;
	}
	else if (strcmp(opc, "sra") == 0) {
		fprintf(memin, "07"); return 1;
	}
	else if (strcmp(opc, "srl") == 0) {
		fprintf(memin, "08"); return 1;
	}
	else if (strcmp(opc, "beq") == 0) {
		fprintf(memin, "09"); return 1;
	}
	else if (strcmp(opc, "bne") == 0) {
		fprintf(memin, "0A"); return 1;
	}
	else if (strcmp(opc, "blt") == 0) {
		fprintf(memin, "0B"); return 1;
	}
	else if (strcmp(opc, "bgt") == 0) {
		fprintf(memin, "0C"); return 1;
	}
	else if (strcmp(opc, "ble") == 0) {
		fprintf(memin, "0D"); return 1;
	}
	else if (strcmp(opc, "bge") == 0) {
		fprintf(memin, "0E"); return 1;
	}
	else if (strcmp(opc, "jal") == 0) {
		fprintf(memin, "0F"); return 1;
	}
	else if (strcmp(opc, "lw") == 0) {
		fprintf(memin, "10"); return 1;
	}
	else if (strcmp(opc, "sw") == 0) {
		fprintf(memin, "11"); return 1;
	}
	else if (strcmp(opc, "reti") == 0) {
		fprintf(memin, "12"); return 1;
	}
	else if (strcmp(opc, "in") == 0) {
		fprintf(memin, "13"); return 1;
	}
	else if (strcmp(opc, "out") == 0) {
		fprintf(memin, "14"); return 1;
	}
	else if (strcmp(opc, "halt") == 0) {
		fprintf(memin, "15"); return 1;
	}
	else // on .word
		return 0;
}



void PrintFromMemoryUnitsToFile(Memory* mem, FILE* memin)
{
	int MinAddr = 50000;
	int currentAddr = 0;
	// i - memory index, num - word for .word
	int i = 0, num = 0; int flag = 0; int check = 0;
	int counter = 0;
	while (i <= mem->last && mem->head != NULL) {
		MemoryUnit* currentLine = getAtPosition(mem->head, i);
		if (currentLine != NULL)
		{
			if ((strcmp(currentLine->opcode, "NONO") == 0)) {

				{
					currentAddr = currentLine->pos;

					if (MinAddr > currentAddr) MinAddr = currentAddr;
				}
			}
		}
		i++;
	}
	i = 0;

	while (i <= mem->last && mem->head != NULL) {
		MemoryUnit* currentLine = getAtPosition(mem->head, i); // get the current line's data once. this will reduce the code's execution time. allowing it to build apps much more quickly
		// Printing Opcode. if data for the ith row does not exist print a zero
		if (currentLine == NULL)
		{
			if (counter < MinAddr)
			{


				fprintf(memin, "00");// if no opcode print 2 zeros

			}
		}
		else flag = printopcode(currentLine->opcode, memin); // print the opcode and return if it was printed
		if (flag == 0 && currentLine != NULL) {// if there is no opcode. this block of code is used to get the word for the .word command
			if ((strcmp(currentLine->opcode, "NONO") == 0)) {
				if ((currentLine->imm[0] == '0') && ((currentLine->imm[1] == 'x') || (currentLine->imm[1] == 'X')))
					num = strtol(currentLine->imm, NULL, 16);
				else  //Imiddiate is decimal
					num = atoi(currentLine->imm);
				fprintf(memin, "%05X", num);  //Print immidiate in hex
				check = 1;


			}
		}
		if (currentLine == NULL) {
			if (counter < MinAddr)
				fprintf(memin, "0"); // Printing Rd
		}
		else if (flag == 1)print_rd_rs_rt(currentLine->rd, memin, &num);
		if (currentLine == NULL)
		{
			if (counter < MinAddr)
				fprintf(memin, "0");
		}// Printing Rs
		else if (flag == 1) print_rd_rs_rt(currentLine->rs, memin, &num);
		if (currentLine == NULL)
		{
			if (counter < MinAddr)
				fprintf(memin, "0");
		}// Printing Rt
		else if (flag == 1) print_rd_rs_rt(currentLine->rt, memin, &num);
		// a check wheter to print the immediate and skip .word lines

		if (currentLine != NULL && check == 0) {
			if ((strcmp(currentLine->rt, "$imm") != 0) && (strcmp(currentLine->rd, "$imm") != 0) && (strcmp(currentLine->rs, "$imm") != 0))
			{
				fprintf(memin, "\n");  //Print \n except the last line
				i++;
				counter++;
				continue;
			}
			if (strcmp(currentLine->rt, "$imm") == 0 && (strcmp(currentLine->opcode, "jal") == 0) && (strcmp(currentLine->rs, "$imm") != 0))
			{
				fprintf(memin, "\n");  //Print \n except the last line
				i++;
				counter++;
				continue;
			}
			if ((strcmp(currentLine->opcode, "ble") == 0) || (strcmp(currentLine->opcode, "beq") == 0) || (strcmp(currentLine->opcode, "bge") == 0) || (strcmp(currentLine->opcode, "blt") == 0) || (strcmp(currentLine->opcode, "bne") == 0) || (strcmp(currentLine->opcode, "out") == 0) || (strcmp(currentLine->opcode, "in") == 0) || (strcmp(currentLine->opcode, "bgt") == 0))
			{
				if ((strcmp(currentLine->rd, "$imm") == 0) || (strcmp(currentLine->rs, "$imm") == 0) || (strcmp(currentLine->rt, "$imm") == 0))
				{
					counter = counter + 2;
					i++;
					if ((currentLine->imm[0] == '0') && ((currentLine->imm[1] == 'x') || (currentLine->imm[1] == 'X')))
						num = strtol(currentLine->imm, NULL, 16);
					else if (currentLine->imm[0] == '-')  //Imiddiate is decimal and negative
					{
						num = strtol(currentLine->imm + 1, NULL, 16); // convert string to hex

						num = (~num) + 1;

						num = num & 0x0FFFFF;

					}
					else
					{

						num = atoi(currentLine->imm);   /// convert string to int

					}



					fprintf(memin, "\n");
					fprintf(memin, "%05X", num);
					fprintf(memin, "\n");

					continue;
				}

				else
				{



					fprintf(memin, "\n");  //Print \n except the last line
					i++;
					counter++;
					continue;
				}

			}
			else if ((strcmp(currentLine->rt, "$imm") != 0) && (strcmp(currentLine->rs, "$imm") != 0))
			{
				counter++;
				fprintf(memin, "\n");  //Print \n except the last line
				i++;
				continue;
			}
			counter = counter + 2;
			i++;
			if ((currentLine->imm[0] == '0') && ((currentLine->imm[1] == 'x') || (currentLine->imm[1] == 'X')))
				num = strtol(currentLine->imm, NULL, 16);
			else if (currentLine->imm[0] == '-')  //Imiddiate is decimal and negative
			{
				num = strtol(currentLine->imm + 1, NULL, 16); // convert string to hex

				num = (~num) + 1;

				num = num & 0x0FFFFF;

			}
			else
			{

				num = atoi(currentLine->imm);   /// convert string to int

			}



			fprintf(memin, "\n");
			fprintf(memin, "%05X", num);
			fprintf(memin, "\n");
			continue;

		}
		if (currentLine == NULL && counter >= MinAddr && i < mem->last)
		{
			i++;
			continue;
		}
		if (check == 1)
		{
			i++;
			fprintf(memin, "\n");  //Print \n except the last line
			continue;
		}
		i++;
		counter++;
		fprintf(memin, "\n");  //Print \n except the last line

		// go to next line
	}
}

void LableChange_position(MemoryUnit* head, label* lb)
{
	char temporary[50];
	// the current memory line
	MemoryUnit* current = head;
	while (current != NULL) {
		// find if there is a label on the immediate and if it exists
		int loc = Find_Label_Name(lb, current->imm);
		// if found
		if (loc != -1) {
			_itoa(loc, temporary, 10); // Changes int to string and puts in temp
			strcpy(current->imm, temporary); // Copy label location number to immidiate
		}
		if (strcmp(current->imm, "$zero") == 0) // If immidiate is &zero
		{
			strcpy(current->imm, "0"); // Changes immidiate to "0"
		}
		current = current->next;
	}
}

//main part
int main(int argc, char* argv[]) {
	if (argc != 3) {
		printf("usage: asm.exe program.asm memin.txt\n");
		exit(1);
	}
	FILE* asembl = fopen(argv[1], "r");
	// leave if null file is supplied
	if (asembl == NULL) {
		exit(1);
	}
	// the first iteration, locate the labels and write thier locations to the linked list
	label* labels = makeLabelList(asembl);
	// close the file from the first iteration
	fclose(asembl);
	// and reopen it for the second
	asembl = fopen(argv[1], "r");
	// another null check in case something happend
	if (asembl == NULL) {
		printf("ERROR: couldn't open files\n");
		exit(1);
	}
	// start the second iteration
	Memory* memory = Save_Lines_As_memory_units(asembl);

	fclose(asembl);
	LableChange_position(memory->head, labels); // Change labels from words to numbers

	// Write Data to file
	FILE* memin = fopen(argv[2], "w");
	if (memin == NULL)
	{
		printf("ERROR: couldn't open files\n");
		exit(1);
	}
	PrintFromMemoryUnitsToFile(memory, memin);
	fclose(memin);
	// End of file writing

	// free the memory taken by the label list and memory structure
	DestroyList(labels);
	destroy_mem(memory);
}