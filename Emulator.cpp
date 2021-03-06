
/*
* Author: Robert Painter (17024721)
* Created: 02/02/2018
* Revised: 24/02/2018 - cleaned up code
* Revised: 09/03/2018 - cleaned up code and added comments and banners 
* so that it prints properly
* Description: Chimera-2018-E Emulator code
* User Advice: None
*/

#include "stdafx.h"
#include <winsock2.h>

#pragma comment(lib, "wsock32.lib")


#define STUDENT_NUMBER    "17024721"

#define IP_ADDRESS_SERVER "127.0.0.1"

#define PORT_SERVER 0x1984 // We define a port that we are going to use.
#define PORT_CLIENT 0x1985 // We define a port that we are going to use.

#define WORD  unsigned short
#define DWORD unsigned long
#define BYTE  unsigned char

#define MAX_FILENAME_SIZE 500
#define MAX_BUFFER_SIZE   500

SOCKADDR_IN server_addr;
SOCKADDR_IN client_addr;

SOCKET sock;  // This is our socket, it is the handle to the IO address to read/write packets

WSADATA data;

char InputBuffer [MAX_BUFFER_SIZE];

char hex_file [MAX_BUFFER_SIZE];
char trc_file [MAX_BUFFER_SIZE];

//////////////////////////
//   Registers          //
//////////////////////////

#define FLAG_I  0x10
#define FLAG_V  0x08
#define FLAG_N  0x04
#define FLAG_Z  0x02
#define FLAG_C  0x01
#define REGISTER_A	5
#define REGISTER_F	4
#define REGISTER_E	3
#define REGISTER_D	2
#define REGISTER_C	1
#define REGISTER_B	0
#define REGISTER_X 0
#define REGISTER_Y 1
BYTE Index_Registers[2];

BYTE Registers[6];
BYTE Flags;
WORD ProgramCounter;
WORD StackPointer;


////////////
// Memory //
////////////

#define MEMORY_SIZE	65536

BYTE Memory[MEMORY_SIZE];

#define TEST_ADDRESS_1  0x01FA
#define TEST_ADDRESS_2  0x01FB
#define TEST_ADDRESS_3  0x01FC
#define TEST_ADDRESS_4  0x01FD
#define TEST_ADDRESS_5  0x01FE
#define TEST_ADDRESS_6  0x01FF
#define TEST_ADDRESS_7  0x0200
#define TEST_ADDRESS_8  0x0201
#define TEST_ADDRESS_9  0x0202
#define TEST_ADDRESS_10  0x0203
#define TEST_ADDRESS_11  0x0204
#define TEST_ADDRESS_12  0x0205


///////////////////////
// Control variables //
///////////////////////

bool memory_in_range = true;
bool halt = false;


///////////////////////
// Disassembly table //
///////////////////////

char opcode_mneumonics[][14] =
{
"ILLEGAL     ", 
"ILLEGAL     ", 
"STX abs      ", 
"ILLEGAL     ", 
"ILLEGAL     ", 
"ILLEGAL     ", 
"ILLEGAL     ", 
"MV  #,B      ", 
"MV  #,C      ", 
"MV  #,D      ", 
"MV  #,E      ", 
"MV  #,F      ", 
"MAY impl     ", 
"MYA impl     ", 
"MAS impl     ", 
"CSA impl     ", 

"ILLEGAL     ", 
"ILLEGAL     ", 
"STX abs,X    ", 
"ILLEGAL     ", 
"ILLEGAL     ", 
"ILLEGAL     ", 
"SWI impl     ", 
"RTI impl     ", 
"CLC impl     ", 
"SEC impl     ", 
"CLI impl     ", 
"STI impl     ", 
"STV impl     ", 
"CLV impl     ", 
"ILLEGAL     ", 
"ILLEGAL     ", 

"ILLEGAL     ", 
"ILLEGAL     ", 
"STX abs,Y    ", 
"ADD A,B      ", 
"SUB A,B      ", 
"CMP A,B      ", 
"OR A,B       ", 
"AND A,B      ", 
"EOR A,B      ", 
"BT A,B       ", 
"LD A,A       ", 
"LD B,A       ", 
"LD C,A       ", 
"LD D,A       ", 
"LD E,A       ", 
"LD F,A       ", 

"ILLEGAL     ", 
"LDX  #       ", 
"STX abs,XY   ", 
"ADD A,C      ", 
"SUB A,C      ", 
"CMP A,C      ", 
"OR A,C       ", 
"AND A,C      ", 
"EOR A,C      ", 
"BT A,C       ", 
"LD A,B       ", 
"LD B,B       ", 
"LD C,B       ", 
"LD D,B       ", 
"LD E,B       ", 
"LD F,B       ", 

"ILLEGAL     ", 
"LDX abs      ", 
"STX (ind),XY ", 
"ADD A,D      ", 
"SUB A,D      ", 
"CMP A,D      ", 
"OR A,D       ", 
"AND A,D      ", 
"EOR A,D      ", 
"BT A,D       ", 
"LD A,C       ", 
"LD B,C       ", 
"LD C,C       ", 
"LD D,C       ", 
"LD E,C       ", 
"LD F,C       ", 

"ILLEGAL     ", 
"LDX abs,X    ", 
"ILLEGAL     ", 
"ADD A,E      ", 
"SUB A,E      ", 
"CMP A,E      ", 
"OR A,E       ", 
"AND A,E      ", 
"EOR A,E      ", 
"BT A,E       ", 
"LD A,D       ", 
"LD B,D       ", 
"LD C,D       ", 
"LD D,D       ", 
"LD E,D       ", 
"LD F,D       ", 

"ILLEGAL     ", 
"LDX abs,Y    ", 
"ILLEGAL     ", 
"ADD A,F      ", 
"SUB A,F      ", 
"CMP A,F      ", 
"OR A,F       ", 
"AND A,F      ", 
"EOR A,F      ", 
"BT A,F       ", 
"LD A,E       ", 
"LD B,E       ", 
"LD C,E       ", 
"LD D,E       ", 
"LD E,E       ", 
"LD F,E       ", 

"ILLEGAL     ", 
"LDX abs,XY   ", 
"ILLEGAL     ", 
"NOP impl     ", 
"HLT impl     ", 
"ILLEGAL     ", 
"ILLEGAL     ", 
"ILLEGAL     ", 
"ILLEGAL     ", 
"ILLEGAL     ", 
"LD A,F       ", 
"LD B,F       ", 
"LD C,F       ", 
"LD D,F       ", 
"LD E,F       ", 
"LD F,F       ", 

"ILLEGAL     ", 
"LDX (ind),XY ", 
"ADI  #       ", 
"SBI  #       ", 
"CPI  #       ", 
"ORI  #       ", 
"ANI  #       ", 
"XRI  #       ", 
"ILLEGAL     ", 
"ILLEGAL     ", 
"ILLEGAL     ", 
"ILLEGAL     ", 
"ILLEGAL     ", 
"ILLEGAL     ", 
"ILLEGAL     ", 
"ILLEGAL     ", 

"LDA  #       ", 
"TST abs      ", 
"INC abs      ", 
"DEC abs      ", 
"RCR abs      ", 
"RLC abs      ", 
"ASL abs      ", 
"SAR abs      ", 
"COM abs      ", 
"RAL abs      ", 
"ROR abs      ", 
"LX  #,A      ", 
"ILLEGAL     ", 
"LODS  #      ", 
"PUSH  ,A     ", 
"POP A,       ", 

"LDA abs      ", 
"TST abs,X    ", 
"INC abs,X    ", 
"DEC abs,X    ", 
"RCR abs,X    ", 
"RLC abs,X    ", 
"ASL abs,X    ", 
"SAR abs,X    ", 
"COM abs,X    ", 
"RAL abs,X    ", 
"ROR abs,X    ", 
"ILLEGAL     ", 
"STO abs      ", 
"LODS abs     ", 
"PUSH  ,s     ", 
"POP s,       ", 

"LDA abs,X    ", 
"TST abs,Y    ", 
"INC abs,Y    ", 
"DEC abs,Y    ", 
"RCR abs,Y    ", 
"RLC abs,Y    ", 
"ASL abs,Y    ", 
"SAR abs,Y    ", 
"COM abs,Y    ", 
"RAL abs,Y    ", 
"ROR abs,Y    ", 
"ILLEGAL     ", 
"STO abs,X    ", 
"LODS abs,X   ", 
"PUSH  ,B     ", 
"POP B,       ", 

"LDA abs,Y    ", 
"TST abs,XY   ", 
"INC abs,XY   ", 
"DEC abs,XY   ", 
"RCR abs,XY   ", 
"RLC abs,XY   ", 
"ASL abs,XY   ", 
"SAR abs,XY   ", 
"COM abs,XY   ", 
"RAL abs,XY   ", 
"ROR abs,XY   ", 
"ILLEGAL     ", 
"STO abs,Y    ", 
"LODS abs,Y   ", 
"PUSH  ,C     ", 
"POP C,       ", 

"LDA abs,XY   ", 
"TSTA A,A     ", 
"INCA A,A     ", 
"DECA A,A     ", 
"RCRA A,A     ", 
"RLCA A,A     ", 
"ASLA A,A     ", 
"SARA A,A     ", 
"COMA A,A     ", 
"RALA A,A     ", 
"RORA A,A     ", 
"RTN impl     ", 
"STO abs,XY   ", 
"LODS abs,XY  ", 
"PUSH  ,D     ", 
"POP D,       ", 

"LDA (ind),XY ", 
"DEX impl     ", 
"INX impl     ", 
"DEY impl     ", 
"INCY impl    ", 
"ILLEGAL     ", 
"ILLEGAL     ", 
"ILLEGAL     ", 
"ILLEGAL     ", 
"JSR abs      ", 
"JMP abs      ", 
"ILLEGAL     ", 
"STO (ind),XY ", 
"LODS (ind),XY", 
"PUSH  ,E     ", 
"POP E,       ", 

"BRA rel      ", 
"BCC rel      ", 
"BCS rel      ", 
"BNE rel      ", 
"BEQ rel      ", 
"BVC rel      ", 
"BVS rel      ", 
"BMI rel      ", 
"BPL rel      ", 
"BGE rel      ", 
"BLE rel      ", 
"BGT rel      ", 
"BLT rel      ", 
"ILLEGAL     ", 
"PUSH  ,F     ", 
"POP F,       ", 

}; 

////////////////////////////////////////////////////////////////////////////////
//                           Simulator/Emulator (Start)                       //
////////////////////////////////////////////////////////////////////////////////
/*
* Function:
* Description:
* Parameters:
* Returns:
* Warnings:
*/

/*
* Function: Fetch()
* Description: fetches the next byte in the program from memory using the programcounter
* Parameters: None
* Returns: BYTE byte
* Warnings:
*/
BYTE fetch()
{
	BYTE byte = 0;

	if ((ProgramCounter >= 0) && (ProgramCounter <= MEMORY_SIZE))
	{
		memory_in_range = true;
		byte = Memory[ProgramCounter];
		ProgramCounter++;
	}
	else
	{
		memory_in_range = false;
	}
	return byte;
}

/*
* Function: set_flag_i(BYTE inReg)
* Description:sets the Interrupt flag if bit set not in use
* Parameters: BYTE inreg
* Returns: None
* Warnings:
*/
void set_flag_i(BYTE inReg)//interrupt//chk
{
	BYTE reg;
	reg = inReg;

	if ((reg & 0x80) != 0) // not implemented properly 
	{
		Flags = Flags | FLAG_I;
	}
	else
	{
		Flags = Flags & (0xFF - FLAG_I);
	}
}

/*
* Function: set_flag_n(BYTE inReg)
* Description: sets the Negative flag if the MSB is set
* -----------: clears the flag if the bit is not set
* Parameters: BYTE inreg
* Returns: None
* Warnings:
*/
void set_flag_n(BYTE inReg) 
{
	BYTE reg; 
	reg = inReg; 

	if ((reg & 0x80) != 0) // msbit set 
	{ 
		Flags = Flags | FLAG_N;
	}
	else 
	{ 
		Flags = Flags & (0xFF - FLAG_N);
	}
}

/*
* Function: set_flag_z(BYTE inReg)
* Description:sets the zero flag if the byte is equal to 0
* -----------: clears the flag if not equal to 0
* Parameters: BYTE inreg
* Returns: None
* Warnings:
*/
void set_flag_z(BYTE inReg)//zero flag //chk
{
	BYTE reg;
	reg = inReg;

	if (reg == 0) // msbit set 
	{
		Flags = Flags | FLAG_Z;
	}
	else
	{
		Flags = Flags & (0xFF - FLAG_Z);
	}
}

/*
* Function: set_flag_n_word(WORD inReg)
* Description:sets the Negative flag on a word if the MSB is set
* -----------: clears the flag if the bit is not set
* Parameters: BYTE inreg
* Returns: None
* Warnings:
*/
void set_flag_n_word(WORD inReg)
{
	WORD reg;
	reg = inReg;

	if ((reg & 0x8000) != 0)
	{
		Flags = Flags | FLAG_N;
	}
	else
	{
		Flags = Flags & (0xFF - FLAG_N);
	}
}

/*
* Function: set_flag_z(WORD inReg)
* Description:sets the Zero flag on a word
* Parameters: BYTE inreg
* Returns: None
* Warnings:
*/
void set_flag_z_word(WORD inReg)
{
	WORD reg;
	reg = inReg;

	if (reg == 0)
	{
		Flags = Flags | FLAG_Z;
	}
	else
	{
		Flags = Flags & (0xFF - FLAG_Z);
	}
}

/*
* Function: set_flag_c(WORD inReg)
* Description:sets the Carry flag
* Parameters: BYTE inreg
* Returns: None
* Warnings:
*/
void set_flag_c(BYTE inReg)//carry flag 
{
	return;//
	BYTE reg;
	reg = inReg;

	if ((reg & 0x80) != 0) // not implemented properly 
	{
		Flags = Flags | FLAG_C;
	}
	else
	{
		Flags = Flags & (0xFF - FLAG_C);
	}

}

/*
* Function: set_flag_v(BYTE in1, BYTE in2, BYTE out1)
* Description:sets the Overflow flag
* Parameters: BYTE in1, BYTE in2, BYTE out1
* Returns: None
* Warnings:
*/
void set_flag_v(BYTE in1, BYTE in2, BYTE out1){

	BYTE reg1in;
	BYTE reg2in;
	BYTE regOut;

	reg1in = in1;
	reg2in = in2;
	regOut = out1;

	if(		 (((reg1in & 0x80) == 0x80) && ((reg2in & 0x80) == 0x80) && ((regOut & 0x80) != 0x80)) //overflow
		||	 (((reg1in & 0x80) != 0x80) && ((reg2in & 0x80) != 0x80) && ((regOut & 0x80) == 0x80))) //overflow/!=
	{
		Flags = Flags | FLAG_V;
	}
	else{
		Flags = Flags & (0xFF - FLAG_V);
	}
}
////

////START FUNCTIONS
//START ADDRESS RETRIEVAL
/*
WORD get_addr_word() {
	BYTE LB = 0;
	BYTE HB = 0;
	WORD address = 0;
	return 0;
}
*/

/*
* Function: get_addr_abs()
* Description: gets the absolute address
* Parameters: None
* Returns: WORD address
* Warnings:
*/
WORD get_addr_abs() {
	BYTE LB = 0;
	BYTE HB = 0;
	WORD address = 0;
	HB = fetch();
	LB = fetch();
	address += (WORD)((WORD)HB << 8) + LB;
	return address;
}

/*
* Function: get_addr_absx()
* Description: gets the absolute address offset by register X
* Parameters: None
* Returns: WORD address
* Warnings:
*/
WORD get_addr_absx() {
	BYTE LB = 0;
	BYTE HB = 0;
	WORD address = 0;
	address += Index_Registers[REGISTER_X];
	HB = fetch();
	LB = fetch();
	address += (WORD)((WORD)HB << 8) + LB;
	return address;
}

/*
* Function: get_addr_absy()
* Description: gets the absolute address offset by register Y
* Parameters: None
* Returns: WORD address
* Warnings:
*/
WORD get_addr_absy() {
	BYTE LB = 0;
	BYTE HB = 0;
	WORD address = 0;
	address += Index_Registers[REGISTER_Y];
	HB = fetch();
	LB = fetch();
	address += (WORD)((WORD)HB << 8) + LB;
	return address;
}

/*
* Function: get_addr_absxy()
* Description: gets the absolute address offset by register X and register Y
* Parameters: None
* Returns: WORD address
* Warnings:
*/
WORD get_addr_absxy() {
	BYTE LB = 0;
	BYTE HB = 0;
	WORD address = 0;
	address += (WORD)((WORD)Index_Registers[REGISTER_Y] << 8) + Index_Registers[REGISTER_X];
	HB = fetch();
	LB = fetch();
	address += (WORD)((WORD)HB << 8) + LB;
	return address;
}

/*
* Function: get_addr_indxy()
* Description: gets the indirect address by 
*------------: getting the absolute address of the address in memory
*------------: then offsetting the result by register X and register Y
* Parameters: None
* Returns: WORD address
* Warnings:
*/
WORD get_addr_indxy() {
	BYTE LB = 0;
	BYTE HB = 0;
	WORD address = 0;
	HB = fetch();//get absolute address
	LB = fetch();
	address += (WORD)((WORD)HB << 8) + LB;
	HB = Memory[address];//use this address to get the indirect address
	LB = Memory[address + 1];
	address += (WORD)((WORD)HB << 8) + LB;
	address += Index_Registers[REGISTER_X];
	address += (WORD)((WORD)Index_Registers[REGISTER_Y] << 8);
	return address;
}
//END ADDRESS RETRIEVAL

//MAIN_OPT
/*
* Function:
* Description:
* Parameters:
* Returns:
* Warnings:
*/
//START LDA_OPT
/*
* Function: LD_REG(WORD address, BYTE REG_TO_LOAD)
* Description: Loads a register with a location in memory
* Parameters: (WORD) address, (BYTE) REG_TO_LOAD
* Returns: None
* Warnings: None
*/
void LD_REG(WORD address, BYTE REG_TO_LOAD){	//, BYTE SRC){//,bool REGTOLOAD_AS_ADDRESS) {
	if ((address >= 0) && (address < MEMORY_SIZE)) {
		//if (REGTOLOAD_AS_ADDRESS) {
		//	Memory[address] = SRC;
		//}
		//else {
			Registers[REG_TO_LOAD] = Memory[address];//load memory to register
		//}
	}
	set_flag_n(Registers[REG_TO_LOAD]);//set flags
	set_flag_z(Registers[REG_TO_LOAD]);
}
//END LDA_OPT

//START STO_OPT
/*
* Function: ST_REG(WORD address, BYTE REG_TO_STO)
* Description: Stores a given register in a given memory location
* Parameters: (WORD) address,(BYTE) REG_TO_STO
* Returns: None
* Warnings:
*/
void ST_REG(WORD address, BYTE REG_TO_STO) {
	if (address >= 0 && address < MEMORY_SIZE) {
		Memory[address] = Registers[REG_TO_STO];//store register in memory
	}

	set_flag_n(Registers[REG_TO_STO]);//set flags
	set_flag_z(Registers[REG_TO_STO]);
}
//END STO_OPT

//START ADD_OPT
/*
* Function: ADD_OPT(BYTE REG_TO_ADD)
* Description: Add register A to a given register, then store the result in register A
* Parameters: (BYTE) REG_TO_ADD
* Returns: None
* Warnings:
*/
void ADD_REG(BYTE REG_TO_ADD) {
	WORD temp_word;
	WORD param1;
	WORD param2;

	param1 = Registers[REGISTER_A];
	param2 = Registers[REG_TO_ADD];
	temp_word = (WORD)param1 + (WORD)param2;//temp_word = (WORD)Registers[REGISTER_A] + (WORD)Registers[REGISTER_B];

	if ((Flags & FLAG_C) != 0) {//check for carry flag, if so add 1
		temp_word++;
	}
	if (temp_word >= 0x100)//if overflowed set flag else unset
	{
		Flags = Flags | FLAG_C;
	}
	else {
		Flags = Flags & (0xFF - FLAG_C);
	}
	//Registers[REGISTER_A] = (BYTE)temp_word;
	Registers[REGISTER_A] = (BYTE)temp_word;// assign to register
	
	set_flag_n((BYTE)temp_word);//set flags
	set_flag_z((BYTE)temp_word);
	set_flag_v(param1, param2, (BYTE)temp_word);
	//set_flag_c(Registers[REGISTER_A]);

	
}
//END ADD_OPT

//START SUB_OPT
/*
* Function: SUB_OPT(BYTE REG_TO_SUB)
* Description: Subtract a given register from register A, then store the result in register A
* Parameters: (BYTE) REG_TO_ADD
* Returns: None
* Warnings:
*/
void SUB_REG(BYTE REG_TO_SUB) {
	WORD temp_word;
	WORD param1;
	WORD param2;

	param1 = Registers[REGISTER_A];
	param2 = Registers[REG_TO_SUB];
	temp_word = (WORD)param1 - (WORD)param2;

	//short temp = 0;
	//temp_word = (WORD)Registers[REGISTER_A] + (WORD)Registers[REGISTER_F];
	if ((Flags & FLAG_C) != 0) {
		temp_word--;
	}
	if (temp_word >= 0x100)//if overflowed set flag else unset
	{
		Flags = Flags | FLAG_C;
	}
	else {
		Flags = Flags & (0xFF - FLAG_C);
	}
	Registers[REGISTER_A] = (BYTE)temp_word; //assign to register

	set_flag_n((BYTE)temp_word);//set flags
	set_flag_z((BYTE)temp_word);
	set_flag_v(param1, -param2, (BYTE)temp_word);
	//set_flag_c(Registers[REGISTER_A]);
	//Registers[REGISTER_A] = (BYTE)temp_word;
}
//END SUB_OPT

//START COMPARE_OPT
/*
* Function: CMP_REG(BYTE REG_TO_CMP)
* Description: Compares a given register to register A (subtraction)
* -----------: then sets flags based on the result.
* Parameters: BYTE REG_TO_CMP
* Returns: None
* Warnings:
*/
void CMP_REG(BYTE REG_TO_CMP) {
	WORD temp_word;
	WORD param1;
	WORD param2;
	
	param1 = (BYTE)Registers[REGISTER_A];
	param2 = (BYTE)Registers[REG_TO_CMP];
	temp_word = (WORD)param1 - (WORD)param2; //compare  the result then set flags based on the result
	
	if (temp_word >= 0x100) {
		Flags = Flags | FLAG_C;
	}
	else {
		Flags = Flags & (0xFF - FLAG_C);
	}

	set_flag_n((WORD)temp_word);
	set_flag_z((WORD)temp_word);
	//set_flag_v(param1, param2, (BYTE)temp_word);
	set_flag_v(param1, -param2, (BYTE)temp_word);
}
//END COMPARE_OPT

//START OR_OPT
/*
* Function: OR(BYTE REG_TO_OR)
* Description: performs a bitwise or on register A and a given register
* -----------: then sets flags based on the result.
* Parameters: BYTE REG_TO_OR
* Returns: None
* Warnings:
*/
void OR(BYTE REG_TO_OR) {
	Registers[REGISTER_A] = Registers[REGISTER_A] | Registers[REG_TO_OR];// logical or then set flags
	set_flag_n(Registers[REGISTER_A]);
	set_flag_z(Registers[REGISTER_A]);
}
//END OR_OPT

//START AND_OPT
/*
* Function: AND(BYTE REG_TO_AND)
* Description: performs a bitwise and on register A and a given register
* -----------: then sets flags based on the result.
* Parameters: BYTE REG_TO_AND
* Returns:
* Warnings:
*/
void AND(BYTE REG_TO_AND) {
	Registers[REGISTER_A] = Registers[REGISTER_A] & Registers[REG_TO_AND];// logical and then set flags
	set_flag_n(Registers[REGISTER_A]);
	set_flag_z(Registers[REGISTER_A]);
}
//END AND_OPT

//START EOR_OPT
/*
* Function: EOR(BYTE REG_TO_EOR)
* Description: performs a bitwise xor on register A and a given register
* -----------: then sets flags based on the result.
* Parameters: BYTE REG_TO_EOR
* Returns: None
* Warnings:
*/
void EOR(BYTE REG_TO_EOR) {
	Registers[REGISTER_A] = Registers[REGISTER_A] ^ Registers[REG_TO_EOR];
	set_flag_n(Registers[REGISTER_A]);
	set_flag_z(Registers[REGISTER_A]);
}
//END EOR_OPT

//START BT_OPT
/*
* Function: BT(BYTE REG_TO_BT)
* Description: Performs a bit test (bitwise and) on Register A and a given register
* -----------: then sets flags based on the result.
* Parameters: BYTE REG_TO_BT
* Returns: None
* Warnings:
*/
void BT(BYTE REG_TO_BT) {
	BYTE data;
	data = Registers[REGISTER_A] & Registers[REG_TO_BT]; //logical and then set flags
	//printf("|a/b %X&%X|", Registers[REGISTER_A], Registers[REGISTER_B]);
	set_flag_n(data);
	set_flag_z(data);
}
//END BT_OPT

//START TST_OPT
/*
* Function: TST(WORD address)
* Description: Bit tests an address in memory (subtract 0 from data in address)
* -----------: then stores the result in the memory location and sets flags based on the result.
* Parameters: (WORD) address
* Returns: None
* Warnings:
*/
void TST(WORD address) {
	WORD temp_word;
	WORD param1;
	WORD param2;
	param1 = (WORD)Memory[address];
	//param2 = Registers[REGISTER_F];
	temp_word = (WORD)param1 - 0x00;
	Memory[address] = (BYTE)temp_word;

	set_flag_n((BYTE)temp_word);
	set_flag_z((BYTE)temp_word);

}
//END TST_OPT

//START INC_OPT
/*
* Function: INC_MEM(WORD address)
* Description: Increments a memory location, then sets flags based on the result
* Parameters: WORD address
* Returns: None
* Warnings:
*/
void INC_MEM(WORD address) {
	if (address >= 0 && address < MEMORY_SIZE) {
		Memory[address]++;
	}
	set_flag_n(Memory[address]);
	set_flag_z(Memory[address]);
}
//END INC_OPT

//START DEC_OPT
/*
* Function: DEC_MEM(WORD address)
* Description: Decrements a memory location, then sets flags based on the result
* Parameters: WORD address
* Returns: None
* Warnings:
*/
void DEC_MEM(WORD address) {
	if (address >= 0 && address < MEMORY_SIZE) {
		Memory[address]--;
	}
	set_flag_n(Memory[address]);
	set_flag_z(Memory[address]);
}
//END DEC_OPT

//START RCR_OPT
/*
* Function: RCR(WORD address)
* Description: Rotates right through carry then sets flag based on result
* Parameters: WORD address
* Returns: None
* Warnings:
*/
void RCR(WORD address) {
	BYTE Saved_Flags;
	if (address >= 0 && address < MEMORY_SIZE) {
		Saved_Flags = Flags;
		if ((Memory[address] & 0x01) == 0x01) {
			Flags = Flags | FLAG_C;
		}
		else
		{
			Flags = Flags & (0xFF - FLAG_C);
		}
		Memory[address] = (Memory[address] >> 1) & 0x7F;
		if ((Saved_Flags&FLAG_C) == FLAG_C) {
			Memory[address] = Memory[address] | 0x80;
		}
	}
	set_flag_n(Memory[address]);
	set_flag_z(Memory[address]);
}
//END RCR_OPT

//START RLC_OPT
/*
* Function: RLC(WORD address)
* Description: Rotates left through carry then sets flag based on result
* Parameters: WORD address
* Returns: None
* Warnings:
*/
void RLC(WORD address) {
	BYTE Saved_Flags;
	if (address >= 0 && address < MEMORY_SIZE) {
		Saved_Flags = Flags;
		if ((Memory[address] & 0x80) == 0x80) {
			Flags = Flags | FLAG_C;
		}
		else
		{
			Flags = Flags & (0xFF - FLAG_C);
		}
		Memory[address] = (Memory[address] << 1) & 0xFE;
		if ((Saved_Flags&FLAG_C) == FLAG_C) {
			Memory[address] = Memory[address] | 0x01;
		}
	}
	set_flag_n(Memory[address]);
	set_flag_z(Memory[address]);
}
//END RLC_OPT

//START ASL_OPT
/*
* Function: ASL(WORD address)
* Description: Arithmetic shift left, shifts bits left by 1 (inc carry)then sets flags based on result
* Parameters: WORD address
* Returns: None
* Warnings:
*/
void ASL(WORD address) {
	WORD temp_word;
	if (address >= 0 && address < MEMORY_SIZE) {
		//Saved_Flags = Flags;

		temp_word = (Memory[address] << 1);
		//if ((Saved_Flags&FLAG_C) == FLAG_C) {
		//	Registers[REGISTER_A] = Registers[REGISTER_A] | 0x01;
		//}
		if (temp_word >= 0x100)//if overflowed set flag
		{
			Flags = Flags | FLAG_C;
		}
		else {
			Flags = Flags & (0xFF - FLAG_C);
		}
		Memory[address] = (BYTE)temp_word;
	}
	set_flag_n(Memory[address]);
	set_flag_z(Memory[address]);
}
//END ASL_OPT

//START SAR_OPT
//NOTE - MAY NOT WORK
/*
* Function: SAR(WORD address)
* Description:Arithmetic shift right, shifts bits right by 1 (inc carry) then sets flags based on result
* Parameters: WORD address
* Returns: None
* Warnings:
*/
void SAR(WORD address) {
	BYTE temp = Memory[address];
	if ((Memory[address] & 0x01) == 0x01) {//underflow set carry
		Flags = Flags | FLAG_C;
	}
	else {
		Flags = Flags & (0xFF - FLAG_C);
	}
	Memory[address] = Memory[address] >> 1;
	if ((temp & 0x80) == 0x80) {
		Memory[address] |= 1 << 7;
	}
		set_flag_n(Memory[address]);
		set_flag_z(Memory[address]);
}
//END SAR_OPT

//START COM_OPT
/*
* Function: COM(WORD address)
* Description: Negates memory address(1s compliment/bitflip) then sets flags based on result
* Parameters: WORD address
* Returns: None
* Warnings:
*/
void COM(WORD address) {
	WORD temp_word;
	temp_word = ~Memory[address];// data;//bit flip temp_word = Memory[address] ^ 0xFF;
								 //Memory[address] = (BYTE)temp_word;

	if (temp_word >= 0x100)//if overflowed set flag
	{
		Flags = Flags | FLAG_C;
	}
	else {
		Flags = Flags & (0xFF - FLAG_C);
	}
	Memory[address] = (BYTE)temp_word;//moved above flags from below
	set_flag_n(Memory[address]);
	set_flag_z(Memory[address]);
}
//END COM_OPT

//START RAL_OPT
/*
* Function: RAL(WORD address)
* Description: rotate arithmetic left on a given menory address without carry
* Parameters: WORD address
* Returns: None
* Warnings:
*/
void RAL(WORD address) {
	WORD temp_word;

	//Saved_Flags = Flags;
	temp_word = (Memory[address] << 1);
	//if ((Saved_Flags&FLAG_C) == FLAG_C) {
	//	Registers[REGISTER_A] = Registers[REGISTER_A] | 0x01;
	//}
	if (temp_word >= 0x100)//if overflowed set flag
	{
		//Flags = Flags | FLAG_C;
		temp_word = temp_word | 0x01;
	}
	//else {
	//	Flags = Flags & (0xFF - FLAG_C);
	//}

	Memory[address] = (BYTE)temp_word;
	set_flag_n(Memory[address]);
	set_flag_z(Memory[address]);
}
//END RAL_OPT

//START ROR_OPT
/*
* Function: ROR(WORD address)
* Description: rotate right without carry on a given memory address
* Parameters: WORD address
* Returns: None
* Warnings:
*/
void ROR(WORD address) {
	WORD temp_word;
	temp_word = (Memory[address] >> 1);
	//if ((Saved_Flags&FLAG_C) == FLAG_C) {
	//	Registers[REGISTER_A] = Registers[REGISTER_A] | 0x01;
	//}
	if ((Memory[address] & 0x01) != 0)//if underflowed set flag
	{
		//Flags = Flags | FLAG_C;
		temp_word = temp_word | 0x80;//set MSB as LSB was 1
	}
	//else {
	//	Flags = Flags & (0xFF - FLAG_C);
	//}

	Memory[address] = (BYTE)temp_word;
	set_flag_n(Memory[address]);
	set_flag_z(Memory[address]);
}
//END ROR_OPT

//START LDX_OPT
/*
* Function: LDX(WORD address)
* Description: load memory into register x from a give memory address
* Parameters: WORD address
* Returns: None
* Warnings:
*/
void LDX(WORD address) {
	//set_flag_

	if (address >= 0 && address < MEMORY_SIZE) {
		Index_Registers[REGISTER_X] = Memory[address];
	}
	set_flag_n(Index_Registers[REGISTER_X]);
	set_flag_z(Index_Registers[REGISTER_X]);
	//address += Index_Registers[]
}
//END LDX_OPT

//START STX_OPT
/*
* Function: STX(WORD address)
* Description: stores register x into memory
* Parameters: WORD address
* Returns: None
* Warnings:
*/
void STX(WORD address) {
	//data = fetch();
	//Memory[data] = Registers[REGISTER_A];
	if (address >= 0 && address < MEMORY_SIZE) {
		Memory[address] = Index_Registers[REGISTER_X];
	}

	set_flag_n(Index_Registers[REGISTER_X]);
	set_flag_z(Index_Registers[REGISTER_X]);
}
//END STX_OPT

//START LODS_OPT
/*
* Function: LODS(WORD address)
* Description: loads a given memory address into the accumulator
* Parameters: WORD address
* Returns: None
* Warnings:
*/
void LODS(WORD address) {
	if (address >= 0 && address < MEMORY_SIZE - 1) {
		StackPointer = (WORD)(Memory[address] << 8);
		StackPointer += Memory[address + 1];
	}
	set_flag_n_word(StackPointer);
	set_flag_z_word(StackPointer);
}
//END LODS_OPT

//START PUSH_OPT
/*
* Function: PUSH(BYTE REG_TO_PUSH)
* Description: push a given register onto the stack
* Parameters: BYTE REG_TO_PUSH
* Returns: None
* Warnings:
*/
void PUSH(BYTE REG_TO_PUSH) {
	if ((StackPointer >= 1) && (StackPointer < MEMORY_SIZE)) {
		Memory[StackPointer] = Registers[REG_TO_PUSH];
		StackPointer--;
	}
}
//END PUSH_OPT

//START POP_OPT
/*
* Function:  POP(BYTE REG_TO_POP)
* Description: pop s the top of the stack to a given register
* Parameters: BYTE REG_TO_POP
* Returns: None
* Warnings:
*/
void POP(BYTE REG_TO_POP) {
	if ((StackPointer >= 0) && (StackPointer < MEMORY_SIZE - 1)) {
		StackPointer++;
		Registers[REG_TO_POP] = Memory[StackPointer];
	}
}
//END POP_OPT

//START MV_OPT
/*
* Function: MV(BYTE REG_TO_MV)
* Description: load a given memory loacatoion into a register
* Parameters: BYTE REG_TO_MV
* Returns: None
* Warnings:
*/
void MV(BYTE REG_TO_MV) {
	Registers[REG_TO_MV] = fetch();// Memory[fetch()];
	set_flag_n(Registers[REG_TO_MV]);
	set_flag_z(Registers[REG_TO_MV]);
}

////END   FUNCTIONS



////
void Group_1(BYTE opcode)
{
	BYTE LB = 0;
	BYTE HB = 0;
	WORD address = 0;
	WORD data = 0;
	WORD temp_word;
	WORD param1;
	WORD param2;
	WORD offset;
	BYTE Saved_Flags;

	//printf("lb/hb %b\%b", );
	printf("|G1|op=%X|", opcode);
	switch(opcode) 
	{
		/*
		* LDA 
		* loads data to register A
		* addressing modes: [# ,abs ,absx ,absy ,absxy, indxy]
		*
		*/

		//BEGIN LDA
		case 0x90: //LDA #
			data = fetch();//fetch data
			Registers[REGISTER_A] = data;
			set_flag_n(Registers[REGISTER_A]);
			set_flag_z(Registers[REGISTER_A]);
			break;

		case 0xA0: //LDA abs
			address = get_addr_abs();
			LD_REG(address, REGISTER_A);
			break;

		case 0xB0: //LDA abs x
			address = get_addr_absx();
			LD_REG(address, REGISTER_A);
			break;

		case 0xC0: //LDA abs y
			address = get_addr_absy();
			LD_REG(address, REGISTER_A);
			break;

		case 0xD0: //LDA abs x y
			address = get_addr_absxy();
			LD_REG(address, REGISTER_A);
			break;

		case 0xE0: //LDA ind x y 
			address = get_addr_indxy();
			LD_REG(address, REGISTER_A);
			break;
		//END LDA

		/*
		* STO
		* stores register a to a memory location
		* addressing modes: [abs, absx, absy, absxy, indxy]
		*
		*/
		//START STO
		case 0xAC://STO abs
			address += get_addr_abs();
			ST_REG(address, REGISTER_A);
			break;

		case 0xBC://STO abs x
			address += get_addr_absx();
			ST_REG(address, REGISTER_A);
			break;

		case 0xCC://STO abs y
			address += get_addr_absy();
			ST_REG(address, REGISTER_A);
			break;

		case 0xDC://STO abs x y
			address += get_addr_absxy();
			ST_REG(address, REGISTER_A);
			break;

		case 0xEC://STO ind x y
			address += get_addr_indxy();
			ST_REG(address, REGISTER_A);
			break;
		//END STO

		/*
		* ADD
		* adds a given register to register a with carry
		* addressing modes: [b,c,d,e,f]
		*
		*/
		//START ADD
		case 0x23://ADD A B
			ADD_REG(REGISTER_B);
			break;

		case 0x33://ADD A C
			ADD_REG(REGISTER_C);
			break;

		case 0x43://ADD A D
			ADD_REG(REGISTER_D);
			break;

		case 0x53://ADD A E
			ADD_REG(REGISTER_E);
			break;

		case 0x63://ADD A F
			ADD_REG(REGISTER_F);
			break;
		//END ADD

		/*
		* SUB
		* subtracts a given register from register a with carry
		* addressing modes: [b,c,d,e,f]
		*
		*/
		//START SUB
		case 0x24://SUB A B
			SUB_REG(REGISTER_B);
			break;

		case 0x34://SUB A C
			SUB_REG(REGISTER_C);
			break;

		case 0x44://SUB A D
			SUB_REG(REGISTER_D);
			break;

		case 0x54://SUB A E
			SUB_REG(REGISTER_E);
			break;

		case 0x64://SUB A F
			SUB_REG(REGISTER_F);
			break;
		//END SUB

		/*
		* CMP
		* compares a given register (by subtraction) to a given register with register a
		* addressing modes: [b,c,d,e,f]
		*
		*/
		//START CMP
		case 0x25://CMP A B
			CMP_REG(REGISTER_B);
			break;

		case 0x35://CMP A C
			CMP_REG(REGISTER_C);
			break;

		case 0x45://CMP A D
			CMP_REG(REGISTER_D);
			break;

		case 0x55://CMP A E
			CMP_REG(REGISTER_E);
			break;

		case 0x65://CMP A F
			CMP_REG(REGISTER_F);
			break;
		//END CMP

		/*
		* OR
		* performs a bitwise OR on a given register with register a
		* addressing modes: [b,c,d,e,f]
		*
		*/
		//START OR
		case 0x26://OR A B
			OR(REGISTER_B);
			break;

		case 0x36://OR A B
			OR(REGISTER_C);
			break;

		case 0x46://OR A D
			OR(REGISTER_D);
			break;

		case 0x56://OR A E
			OR(REGISTER_E);
			break;

		case 0x66://OR A F
			OR(REGISTER_F);
			break;
		//END OR

		/*
		* AND
		* performs a bitwise AND on a given register with register a
		* addressing modes: [b,c,d,e,f]
		*
		*/
		//START AND
		case 0x27://AND A B
			AND(REGISTER_B);
			break;

		case 0x37://AND A C
			AND(REGISTER_C);
			break;

		case 0x47://AND A D
			AND(REGISTER_D);
			break;

		case 0x57://AND A E
			AND(REGISTER_E);
			break;

		case 0x67://AND A F
			AND(REGISTER_F);
			break;
		//END AND

		/*
		* EOR
		* performs a bitwise XOR on a given register with register a
		* addressing modes: [b,c,d,e,f]
		*
		*/
		//START EOR
		case 0x28://EOR A B
			EOR(REGISTER_B);
			break;

		case 0x38://EOR A B
			EOR(REGISTER_C);
			break;

		case 0x48://EOR A D
			EOR(REGISTER_D);
			break;

		case 0x58://EOR A E
			EOR(REGISTER_E);
			break;

		case 0x68://EOR A F
			EOR(REGISTER_F);
			break;
		//END EOR

		/*
		* BT
		* performs a bit test (bitwise AND) on a given register with register a
		* addressing modes: [b,c,d,e,f]
		*
		*/
		//START BT
		case 0x29://BT A B
			BT(REGISTER_B);
			break;

		case 0x39://BT A C
			BT(REGISTER_C);
			break;

		case 0x49://BT A D
			BT(REGISTER_D);
			break;

		case 0x59://BT A E
			BT(REGISTER_E);
			break;

		case 0x69://BT A F
			BT(REGISTER_F);
			break;
		//END BT

		/*
		* ADI
		* adds a literal value to register a with carry
		* addressing modes: [#]
		*
		*/
		//START ADI
		case 0x82://ADI #
			//short temp = 0;
			data = fetch();
			temp_word = (WORD)Registers[REGISTER_A] + data;
			if ((Flags & FLAG_C) != 0) {
				temp_word++;
			}
			if (temp_word >= 0x100)//if overflowed set flag
			{
				Flags = Flags | FLAG_C;
			}
			else {
				Flags = Flags & (0xFF - FLAG_C);
			}
			Registers[REGISTER_A] = (BYTE)temp_word;

			set_flag_v(Registers[REGISTER_A], data, (BYTE)temp_word);
			set_flag_n(Registers[REGISTER_A]);
			set_flag_z(Registers[REGISTER_A]);
			//Registers[REGISTER_A] = (BYTE)temp_word;
			break;
		//END ADI

		/*
		* SBI
		* subtracts a literal value to register a with carry
		* addressing modes: [#]
		*
		*/
		//START SBI
		case 0x83://SBI #
			param1 = Registers[REGISTER_A];
			param2 = fetch();// Registers[REGISTER_B];
			temp_word = (WORD)param1 - (WORD)param2;

			if ((Flags & FLAG_C) != 0) {
				temp_word--;
			}
			if (temp_word >= 0x100)//if overflowed set flag
			{
				Flags = Flags | FLAG_C;
			}
			else 
			{
				Flags = Flags & (0xFF - FLAG_C);
			}
			Registers[REGISTER_A] = (BYTE)temp_word;


			set_flag_n((BYTE)temp_word);
			set_flag_z((BYTE)temp_word);
			//set_flag_v(Registers[REGISTER_A], -data, (BYTE)temp_word);
			set_flag_v(param1, -param2, (BYTE)temp_word);
			//Registers[REGISTER_A] = (BYTE)temp_word;
			break;
		//END SBI

		/*
		* CPI
		* literal value compared to register a
		* addressing modes: [#]
		*
		*/
		//START CPI
		case 0x84://CPI #
			param1 = Registers[REGISTER_A];
			param2 = fetch();
			temp_word = (WORD)param1 - (WORD)param2;
			if (temp_word >= 0x100) {
				Flags = Flags | FLAG_C;
			}
			else {
				Flags = Flags & (0xFF - FLAG_C);
			}
			set_flag_n((BYTE)temp_word);
			set_flag_z((BYTE)temp_word);
			set_flag_v(param1, -param2, (BYTE)temp_word);
			break;
		//END CPI

		/*
		* ORI
		* literal value bitwise OR to register a
		* addressing modes: [#]
		*
		*/
		//START ORI
		case 0x85://ORI #
			Registers[REGISTER_A] = Registers[REGISTER_A] | fetch();
			set_flag_n(Registers[REGISTER_A]);
			set_flag_z(Registers[REGISTER_A]);
			break;
		//END ORI

		/*
		* ANI
		* literal value bitwise AND with register a
		* addressing modes: [#]
		*
		*/
		//START ANI
		case 0x86://ANI #
			Registers[REGISTER_A] = Registers[REGISTER_A] & fetch();
			//printf("|a/b %X&%X|", Registers[REGISTER_A], Registers[REGISTER_B]);
			//Registers[REGISTER_A] = data;
			set_flag_n(Registers[REGISTER_A]);
			set_flag_z(Registers[REGISTER_A]);
			break;
		//END ANI

		/*
		* XRI
		* literal value bitwise XOR with register a
		* addressing modes: [#]
		*
		*/
		//START XRI
		case 0x87://XRI #
			Registers[REGISTER_A] = Registers[REGISTER_A] ^ fetch();
			set_flag_n(Registers[REGISTER_A]);
			set_flag_z(Registers[REGISTER_A]);
			break;
		//END XRI

		/*
		* TST
		* bit tests memory address
		* addressing modes: [abs, absx, absy, absxy]
		*
		*/
		//START TST
		case 0x91://TST abs
			address += get_addr_abs();
			TST(address);
			break;

		case 0xA1://TST abs x
			address += get_addr_absx();
			TST(address);
			break;

		case 0xB1://TST abs y
			address += get_addr_absy();
			TST(address);
			break;

		case 0xC1://TST abs x y
			address += get_addr_absxy();
			TST(address);
			break;
		//END TST
			
		/*
		* TSTA
		* bit tests register a
		* addressing modes: [a]
		*
		*/
		//START TSTA
		case 0xD1://TSTA A
			param1 = Registers[REGISTER_A];
			//param2 = Registers[REGISTER_F];
			temp_word = (WORD)param1 - 0x00;

			Registers[REGISTER_A] = (BYTE)temp_word;

			set_flag_n((BYTE)temp_word);
			set_flag_z((BYTE)temp_word);
			break;
		//END TSTA

		/*
		* INC
		* increments memory address
		* addressing modes: [abs, absx, absy, absxy]
		*
		*/
		//START INC
		case 0x92://INC abs
			address += get_addr_abs();
			INC_MEM(address);
			break;

		case 0xA2://INC abs x
			address += get_addr_absx();
			INC_MEM(address);
			break;

		case 0xB2://INC abs y
			address += get_addr_absy();
			INC_MEM(address);
			break;

		case 0xC2://INC abs x y
			address += get_addr_absxy();
			INC_MEM(address);
			break;
		//END INC

		/*
		* INCA
		* increments register a
		* addressing modes: [a]
		*
		*/
		//START INCA
		case 0xD2://INCA A
			Registers[REGISTER_A]++;
			set_flag_n(Registers[REGISTER_A]);
			set_flag_z(Registers[REGISTER_A]);
			break;
		//END INCA

		/*
		* DEC
		* decrements memory address
		* addressing modes: [abs, absx, absy, absxy]
		*
		*/
		//START DEC
		case 0x93://DEC abs 
			address += get_addr_abs();
			DEC_MEM(address);
			break;

		case 0xA3://DEC abs x
			address += get_addr_absx();
			DEC_MEM(address);
			break;

		case 0xB3://DEC abs y
			address += get_addr_absy();
			DEC_MEM(address);
			break;

		case 0xC3://DEC abs x y
			address += get_addr_absxy();
			DEC_MEM(address);
			break;
		//END DEC

		/*
		* DECA
		* decrements register a
		* addressing modes: [a]
		*
		*/
		//START DECA
		case 0xD3://DECA A
			Registers[REGISTER_A]--;
			set_flag_n(Registers[REGISTER_A]);
			set_flag_z(Registers[REGISTER_A]);
			break;
		//END DECA

		/*
		* RCR
		* rotates right through carry on a memory address
		* addressing modes: [abs, absx, absy, absxy]
		*
		*/
		//START RCR
		case 0x94://RCR abs
			address += get_addr_abs();
			RCR(address);
			break;

		case 0xA4://RCR abs x
			address += get_addr_absx();
			RCR(address);
			break;

		case 0xB4://RCR abs y
			address += get_addr_absy();
			RCR(address);
			break;

		case 0xC4://RCR abs x y 
			address += get_addr_absxy();
			RCR(address);
			break;
		//END RCR

		/*
		* RCRA
		* rotates right through carry on a register a
		* addressing modes: [a]
		*
		*/
		//START RCRA
		case 0xD4://RCRA A
			Saved_Flags = Flags;
			if ((Registers[REGISTER_A] & 0x01) == 0x01) {
				Flags = Flags | FLAG_C;
			}
			else
			{
				Flags = Flags & (0xFF - FLAG_C);
			}
			Registers[REGISTER_A] = (Registers[REGISTER_A] >> 1) & 0x7F;
			if ((Saved_Flags&FLAG_C) == FLAG_C) {
				Registers[REGISTER_A] = Registers[REGISTER_A] | 0x80;
			}
			set_flag_n(Registers[REGISTER_A]);
			set_flag_z(Registers[REGISTER_A]);
			break;
		//END RCRA

		/*
		* RLC
		* rotates left through carry on a memory address
		* addressing modes: [abs, absx, absy, absxy]
		*
		*/
		//START RLC
		case 0x95://RLC abs
			address += get_addr_abs();
			RLC(address);
			break;

		case 0xA5://RLC abs x
			address += get_addr_absx();
			RLC(address);
			break;

		case 0xB5://RLC abs y
			address += get_addr_absy();
			RLC(address);
			break;

		case 0xC5://RLC abs x y 
			address += get_addr_absxy();
			RLC(address);
			break;
		//END RLC

		/*
		* RLCA
		* rotates left through carry on register a
		* addressing modes: [a]
		*
		*/
		//START RLCA
		case 0xD5://RLCA A
			Saved_Flags = Flags;
			if ((Registers[REGISTER_A] & 0x80) == 0x80) {
				Flags = Flags | FLAG_C;
			}
			else
			{
				Flags = Flags & (0xFF - FLAG_C);
			}
			Registers[REGISTER_A] = (Registers[REGISTER_A] << 1) & 0xFE;
			if ((Saved_Flags&FLAG_C) == FLAG_C) {
				Registers[REGISTER_A] = Registers[REGISTER_A] | 0x01;
			}
			set_flag_n(Registers[REGISTER_A]);
			set_flag_z(Registers[REGISTER_A]);
			break;
		//END RLCA

		/*
		* ASL
		* arithmetic shift left on a memory address
		* addressing modes: [abs, absx, absy, absxy]
		*
		*/
		//START ASL
		case 0x96://ASL abs
			address += get_addr_abs();
			ASL(address);
			break;

		case 0xA6://ASL abs x
			address += get_addr_absx();
			ASL(address);
			break;

		case 0xB6://ASL abs y
			address += get_addr_absy();
			ASL(address);
			break;

		case 0xC6://ASL abs x y
			address += get_addr_absxy();
			ASL(address);
			break;
		//END ASL

		/*
		* ASLA
		* arithmetic shift left on register a
		* addressing modes: [a]
		*
		*/
		//START ASLA
		case 0xD6://ASLA A
			//Saved_Flags = Flags;

			temp_word = (Registers[REGISTER_A] << 1);
			//if ((Saved_Flags&FLAG_C) == FLAG_C) {
			//	Registers[REGISTER_A] = Registers[REGISTER_A] | 0x01;
			//}
			if (temp_word >= 0x100)//if overflowed set flag
			{
				Flags = Flags | FLAG_C;
			}
			else {
				Flags = Flags & (0xFF - FLAG_C);
			}
			Registers[REGISTER_A] = (BYTE)temp_word;
			set_flag_n(Registers[REGISTER_A]);
			set_flag_z(Registers[REGISTER_A]);
			break;
		//END ASLA

		/*
		* SAR
		* arithmetic shift right on a memory address
		* addressing modes: [abs, absx, absy, absxy]
		*
		*/
		//START SAR
		case 0x97://SAR abs
			address += get_addr_abs();
			//printf("address[contents] = %X[%X]", address, Memory[address]);
			SAR(address);
			//printf("address[contents] = %X[%X]", address, Memory[address]);
			break;

		case 0xA7://SAR abs x
			address += get_addr_absx();
			SAR(address);
			break;

		case 0xB7://SAR abs y
			address += get_addr_absy();
			SAR(address);
			break;

		case 0xC7://SAR abs x y
			address += get_addr_absxy();
			SAR(address);
			break;
		//END SAR

		/*
		* SARA
		* arithmetic shift right on a memory address
		* addressing modes: [a]
		*
		*/
		//START SARA
		case 0xD7://SARA A
			if ((Registers[REGISTER_A] & 0x01) == 0x01) {
				Flags = Flags | FLAG_C;
			}
			else {
				Flags = Flags & (0xFF - FLAG_C);
			}
			Registers[REGISTER_A] = (Registers[REGISTER_A] >> 1) & 0x7F;

			if ((Flags & FLAG_N) == FLAG_N) {
				Registers[REGISTER_A] = Registers[REGISTER_A] | 0x80;
			}
			set_flag_n(Registers[REGISTER_A]);
			set_flag_z(Registers[REGISTER_A]);
			break;
		//END SARA

		/*
		* COM
		* negate a memory address
		* addressing modes: [abs, absx, absy, absxy]
		*
		*/
		//START COM
		case 0x98://COM abs
			address += get_addr_abs();
			COM(address);
			break;

		case 0xA8://COM abs x
			address += get_addr_absx();
			COM(address);
			break;

		case 0xB8://com abs y
			address += get_addr_absy();
			COM(address);
			break;

		case 0xC8://COM ABS x y
			address += get_addr_absxy();
			COM(address);
			break;
		//END COM

		/*
		* COMA
		* negate register a
		* addressing modes: [a]
		*
		*/
		//START COMA
		case 0xD8://COMA A
			//Registers[REGISTER_A] = Registers[REGISTER_A] ^ 0xFF;
			//data = Registers[REGISTER_A];
			temp_word = ~Registers[REGISTER_A];// data;//bit flip
			//temp_word = Registers[REGISTER_A] ^ 0xFF;
			//Registers[REGISTER_A] = (BYTE)temp_word;
			if (temp_word >= 0x100)//if overflowed set flag
			{
				Flags = Flags | FLAG_C;
			}
			else {
				Flags = Flags & (0xFF - FLAG_C);
			}
			Registers[REGISTER_A] = (BYTE)temp_word;
			set_flag_n(temp_word);
			set_flag_z(temp_word);
			return;
		//END COMA

		/*
		* RAL
		* rotate left without carry on a memory address
		* addressing modes: [abs, absx, absy, absxy]
		*
		*/
		//START RAL
		case 0x99://RAL abs
			address += get_addr_abs();
			RAL(address);
			break;

		case 0xA9://RAL abs x
			address += get_addr_absx();
			RAL(address);
			break;

		case 0xB9://RAL abs y
			address += get_addr_absy();
			RAL(address);
			break;

		case 0xC9://RAL abs x y
			address += get_addr_absxy();
			RAL(address);
			break;
		//END RAL

		/*
		* RALA
		* rotate left without carry on register a
		* addressing modes: [a]
		*
		*/
		//START RALA
		case 0xD9:
			//Saved_Flags = Flags;

			temp_word = (Registers[REGISTER_A] << 1);
			//if ((Saved_Flags&FLAG_C) == FLAG_C) {
			//	Registers[REGISTER_A] = Registers[REGISTER_A] | 0x01;
			//}
			if (temp_word >= 0x100)//if overflowed set flag
			{
				//Flags = Flags | FLAG_C;
				temp_word = temp_word | 0x01;//set LSB as MSB was 1
			}
			//else {
			//	Flags = Flags & (0xFF - FLAG_C);
			//}

			Registers[REGISTER_A] = (BYTE)temp_word;
			set_flag_n(Registers[REGISTER_A]);
			set_flag_z(Registers[REGISTER_A]);
			break;
		//END RALA

		/*
		* ROR
		* rotate right without carry on a memory address
		* addressing modes: [abs, absx, absy, absxy]
		*
		*/
		//START ROR
		case 0x9A://ROR abs
			address += get_addr_abs();
			ROR(address);
			break;

		case 0xAA://ROR abs x
			address += get_addr_absx();
			ROR(address);
			break;

		case 0xBA://ROR abs y
			address += get_addr_absy();
			ROR(address);
			break;

		case 0xCA://ROR abs x y
			address += get_addr_absxy();
			ROR(address);
			break;
		//END ROR

		/*
		* RORA
		* rotate right without carry on register a
		* addressing modes: [a]
		*
		*/
		//START RORA
		case 0xDA://RORA A
			//Saved_Flags = Flags;
			temp_word = (Registers[REGISTER_A] >> 1);
			//if ((Saved_Flags&FLAG_C) == FLAG_C) {
			//	Registers[REGISTER_A] = Registers[REGISTER_A] | 0x01;
			//}
			if ((Registers[REGISTER_A] & 0x01) != 0)//if underflowed set flag
			{
				//Flags = Flags | FLAG_C;
				temp_word = temp_word | 0x80;//set MSB as LSB was 1
			}

			Registers[REGISTER_A] = (BYTE)temp_word;
			set_flag_n(Registers[REGISTER_A]);
			set_flag_z(Registers[REGISTER_A]);
			break;
		//END RORA

		/*
		* LD
		* load register specified to register specified
		* addressing modes: [a,b,c,d,e,f,g - a,b,c,d,e,f,g]
		*
		*/
		//START LD
		////LD IN GROUP 2
		//END LD

		/*
		* LDX
		* loads memory into register x
		* addressing modes: [#, abs, absx, absy, absxy, indxy]
		*
		*/
		//START LDX
		case 0x31://LDX #
			data = fetch();//fetch data
			//printf("|d1=%X|", data);
			Index_Registers[REGISTER_X] = data;
			set_flag_n(Index_Registers[REGISTER_X]);
			set_flag_z(Index_Registers[REGISTER_X]);
			break;

		case 0x41://LDX abs
			address += get_addr_abs();
			LDX(address);
			break;

		case 0x51://LDX abs x
			address += get_addr_absx();
			LDX(address);
			break;

		case 0x61://LDX abs y
			address += get_addr_absy();
			LDX(address);
			break;

		case 0x71://LDX abs x y
			address += get_addr_absxy();
			LDX(address);
			break;

		case 0x81://LDX ind x y 
			address += get_addr_indxy();
			LDX(address);
			break;
		//END LDX

		/*
		* STX
		* stores register x to a memory location
		* addressing modes: [abs, absx, absy, absxy, indxy]
		*
		*/
		//START STX
		case 0x02://STX abs
			address += get_addr_abs();
			STX(address);
			break;

		case 0x12://STX abs x
			address += get_addr_absx();
			STX(address);
			break;

		case 0x22://STX abs y
			address += get_addr_absy();
			STX(address);
			break;

		case 0x32://STX abs x y
			address += get_addr_absxy();
			STX(address);
			break;

		case 0x42://STX ind x y
			address += get_addr_indxy();
			STX(address);
			break;
		//END STX

		/*
		* DEX
		* decrements register x
		* addressing modes: [impl]
		*
		*/
		//START DEX
		case 0xE1://DEX impl
			Index_Registers[REGISTER_X]--;
			set_flag_z(Index_Registers[REGISTER_X]);
			break;
		//END DEX

		/*
		* INX
		* increments register x
		* addressing modes: [impl]
		*
		*/
		//START INX
		case 0xE2://INX impl
			Index_Registers[REGISTER_X]++;
			set_flag_z(Index_Registers[REGISTER_X]);
			break;
		//END INX

		/*
		* MAY
		* move accumulator to register y
		* addressing modes: [impl]
		*
		*/
		//START MAY
		case 0x0C://MAY impl
			Index_Registers[REGISTER_Y] = Registers[REGISTER_A];
			set_flag_n(Index_Registers[REGISTER_Y]);
			break;
		//END MAY

		/*
		* MYA
		* move register y to accumulator
		* addressing modes: [impl]
		*
		*/
		//START MYA
		case 0x0D://MYA impl
			Registers[REGISTER_A] = Index_Registers[REGISTER_Y];
			set_flag_n(Registers[REGISTER_A]);
			set_flag_z(Registers[REGISTER_A]);
			break;
		//END MYA

		/*
		* DEY
		* decrement register y
		* addressing modes: [impl]
		*
		*/
		//START DEY
		case 0xE3://DEY impl
			Index_Registers[REGISTER_Y]--;
			set_flag_z(Index_Registers[REGISTER_Y]);
			break;
		//END DEY

		/*
		* INCY
		* increment register y
		* addressing modes: [impl]
		*
		*/
		//START INCY
		case 0xE4://INCY impl
			Index_Registers[REGISTER_Y]++;
			set_flag_z(Index_Registers[REGISTER_Y]);
			break;
		//END INCY

		/*
		* LODS
		* loads memory into stackpointer
		* addressing modes: [#, abs, absx, absy, absxy, indxy]
		*
		*/
		//START LODS
		case 0x9D://LODS #
			HB = fetch();
			LB = fetch();
			StackPointer = (WORD)((WORD)HB << 8) + LB;
			set_flag_n_word(StackPointer);
			set_flag_z_word(StackPointer);

			break;
		case 0xAD://LODS abs
			address += get_addr_abs();
			LODS(address);
			break;

		case 0xBD://lods abs x
			address += get_addr_absx();
			LODS(address);
			break;

		case 0xCD://lods abs y
			address += get_addr_absy();
			LODS(address);
			break;

		case 0xDD://lods abs x y
			address += get_addr_absxy();
			LODS(address);
			break;

		case 0xED://lods ind abs x y
			address += get_addr_indxy();
			LODS(address);
			break;
		//END LODS

		/*
		* MAS
		* move accumulator to Status registers
		* addressing modes: [impl]
		*
		*/
		//START MAS
		case 0x0E://MAS impl
			Flags = Registers[REGISTER_A];
			break;
		//END MAS

		/*
		* CSA
		* move Status registers to accumulator
		* addressing modes: [impl]
		*
		*/
		//START CSA
		case 0x0F://CSA impl
			Registers[REGISTER_A] = Flags;
			//printf("|%X|", Flags);
			break;
		//END CSA
		
		/*
		* PUSH
		* push register to stack
		* addressing modes: [a,fl,b,c,d,e,f]
		*
		*/
		//START PUSH
		case 0x9E://push A
			PUSH(REGISTER_A);
			break;

		case 0xAE://PUSH FL
			if ((StackPointer >= 1) && (StackPointer < MEMORY_SIZE)) {
				Memory[StackPointer] = Flags;
				StackPointer--;
			}
			break;

		case 0xBE://PUSH B
			PUSH(REGISTER_B);
			break;

		case 0xCE://PUSH C
			PUSH(REGISTER_C);
			break;

		case 0xDE://PUSH D
			PUSH(REGISTER_D);
			break;

		case 0xEE://PUSH E
			PUSH(REGISTER_E);
			break;

		case 0xFE://PUSH F
			PUSH(REGISTER_F);
			break;
		//END PUSH

		/*
		* POP
		* pops top of stack into a given register
		* addressing modes: [a,FL,b,c,d,e,f]
		*
		*/
		//START POP
		case 0x9F://POP A
			//printf("address[contents] = %X[%X]", address, Memory[address]);
			POP(REGISTER_A);
			break;
		case 0xAF:
			if ((StackPointer >= 0) && (StackPointer < MEMORY_SIZE - 1)) {
				StackPointer++;
				Flags = Memory[StackPointer];
			}
			break;
		case 0xBF://POP B
			POP(REGISTER_B);
			break;
		case 0xCF://POP C
			POP(REGISTER_C);
			break;
		case 0xDF://POP D
			POP(REGISTER_D);
			break;
		case 0xEF://POP E
			POP(REGISTER_E);
			break;
		case 0xFF://POP F
			POP(REGISTER_F);
			break;
		//END POP

		/*
		* LX
		* loads memory into register pair
		* addressing modes: [ab #]
		*
		*/
		//START LX
		case 0x9B://LX A,B #
			HB = fetch();
			LB = fetch();
			address += (WORD)((WORD)HB << 8) + LB;

			Registers[REGISTER_B] = HB;// Memory[fetch()];
			Registers[REGISTER_A] = LB;// Memory[fetch()];
			set_flag_n_word(address);
			set_flag_z_word(address);
			break;
		//END LX

		/*
		* JMP
		* loads memory into programcounter
		* addressing modes: [abs]
		*
		*/
		//START JMP
		case 0xEA://JMP impl
			address += get_addr_abs();
			if (address >= 0 && address < MEMORY_SIZE) {
				//ProgramCounter = Memory[address];
				ProgramCounter = address;
			}
			break;
		//END JMP

		/*
		* MV
		* loads memory into register
		* addressing modes: [b #, c #, d #, e #, f #]
		*
		*/
		//START MV
		case 0x07://MV # to B
			MV(REGISTER_B);
			break;

		case 0x08://MV # to C
			MV(REGISTER_C);
			break;

		case 0x09://MV # to D
			MV(REGISTER_D);
			break;

		case 0x0A://MV # to E
			MV(REGISTER_E);
			break;

		case 0x0B://MV # to F
			MV(REGISTER_F);
			break;
		//END MV

		/*
		* JSR
		* jump to subroutine
		* addressing modes: [abs]
		*
		*/
		//START JSR
		case 0xE9://JSR abs
			address = get_addr_abs();

			if ((address >= 0) && (address < MEMORY_SIZE)) {
				HB = (BYTE)((ProgramCounter >> 8) & 0xFF);
				LB = (BYTE)(ProgramCounter & 0xFF);
				Memory[StackPointer] = HB;
				StackPointer--;
				Memory[StackPointer] = LB;
				StackPointer--;
				printf("!result  S+1 S  HB/LB -> %X[%X] %X[%X] %X/%X !", StackPointer+1, Memory[StackPointer+1], StackPointer, Memory[StackPointer],HB,LB);
				ProgramCounter = address;//swapped these
			}
			//ProgramCounter = address;
			break;
		//END JSR

		/*
		* RTN
		* return from subroutine
		* addressing modes: [impl]
		*
		*/
		//START RTN
		case 0xDB://RTN impl
			if ((StackPointer >= 0) && (StackPointer < MEMORY_SIZE - 2)) {
				StackPointer++;
				LB = Memory[StackPointer];
				StackPointer++;
				HB = Memory[StackPointer];
				ProgramCounter = ((WORD)HB << 8) + (WORD)LB;
			}
			break;
		//END RTN

		/*
		* BRA
		* branch always
		* addressing modes: [rel]
		*
		*/
		//START BRA
		case 0xF0://BRA rel
			//HB = fetch();
			LB = fetch();
			offset = (WORD)LB;
			if ((offset & 0x80) != 0) {
				offset += 0xFF00;
			}
			address = ProgramCounter + offset;//CHECK
			//address += (WORD)((WORD)HB << 8) + LB;
			//if (address >= 0 && address < MEMORY_SIZE) {
				//ProgramCounter = Memory[address];
				ProgramCounter = address;
			//}
			break;
		//END BRA

		/*
		* BCC
		* branch on carry clear
		* addressing modes: [rel]
		*
		*/
		//START BCC
		case 0xF1://BCC rel
			//HB = fetch();
			LB = fetch();
			offset = (WORD)LB;
			if ((offset & 0x80) != 0) {
				offset += 0xFF00;
			}
			address = ProgramCounter + offset;//CHECK
			//address += (WORD)((WORD)HB << 8) + LB;
			if (address >= 0 && address < MEMORY_SIZE) {
				//ProgramCounter = Memory[address];
				if ((Flags & FLAG_C) == 0) {
					ProgramCounter = address;
				}
			}
			break;
		//END BCC

		/*
		* BCS
		* branch on carry set
		* addressing modes: [rel]
		*
		*/
		//START BCS
		case 0xF2://BCS rel
			//HB = fetch();
			LB = fetch();
			offset = (WORD)LB;
			if ((offset & 0x80) != 0) {
				offset += 0xFF00;
			}
			address = ProgramCounter + offset;//CHECK
			//address += (WORD)((WORD)HB << 8) + LB;
			if (address >= 0 && address < MEMORY_SIZE) {
				//ProgramCounter = Memory[address];
				if ((Flags & FLAG_C) != 0) {
					ProgramCounter = address;
				}
			}
			break;
		//END BCS

		/*
		* BNE
		* branch on result not zero
		* addressing modes: [rel]
		*
		*/
		//START BNE
		case 0xF3://BNE rel
			//HB = fetch();
			LB = fetch();
			offset = (WORD)LB;
			if ((offset & 0x80) != 0) {
				offset += 0xFF00;
			}
			address = ProgramCounter + offset;//CHECK
											  //address += (WORD)((WORD)HB << 8) + LB;
			if (address >= 0 && address < MEMORY_SIZE) {
				//ProgramCounter = Memory[address];
				if ((Flags & FLAG_Z) == 0) {
					ProgramCounter = address;
				}
			}
			break;
		//END BNE

		/*
		* BEQ
		* branch on result not equal to zero
		* addressing modes: [rel]
		*
		*/
		//START BEQ
		case 0xF4://BEQ rel
			//HB = fetch();
			LB = fetch();
			offset = (WORD)LB;
			if ((offset & 0x80) != 0) {
				offset += 0xFF00;
			}
			address = ProgramCounter + offset;//CHECK
											  //address += (WORD)((WORD)HB << 8) + LB;
			if (address >= 0 && address < MEMORY_SIZE) {
				//ProgramCounter = Memory[address];
				if ((Flags & FLAG_Z) != 0) {
					ProgramCounter = address;
				}
			}
			break;
		//END BEQ

		/*
		* BVC
		* branch on overflow clear
		* addressing modes: [rel]
		*
		*/
		//START BVC
		case 0xF5://BVC rel
			//HB = fetch();
			LB = fetch();
			offset = (WORD)LB;
			if ((offset & 0x80) != 0) {
				offset += 0xFF00;
			}
			address = ProgramCounter + offset;//CHECK
			//address += (WORD)((WORD)HB << 8) + LB;
			//if (address >= 0 && address < MEMORY_SIZE) {
				//ProgramCounter = Memory[address];
				if ((Flags & FLAG_V) == 0) {
					ProgramCounter = address;
				}
			//}
			break;
		//END BVC

		/*
		* BVS
		* branch on overflow set
		* addressing modes: [rel]
		*
		*/
		//START BVS
		case 0xF6://BVS rel
			//HB = fetch();
			LB = fetch();
			offset = (WORD)LB;
			if ((offset & 0x80) != 0) {
				offset += 0xFF00;
			}
			address = ProgramCounter + offset;//CHECK
											  //address += (WORD)((WORD)HB << 8) + LB;
			if (address >= 0 && address < MEMORY_SIZE) {
				//ProgramCounter = Memory[address];
				if ((Flags & FLAG_V) != 0) {
					ProgramCounter = address;
				}
			}
			break;
		//END BVS

		/*
		* BMI
		* branch on negative result
		* addressing modes: [rel]
		*
		*/
		//START BMI
		case 0xF7://BMI rel
			//HB = fetch();
			LB = fetch();
			offset = (WORD)LB;
			if ((offset & 0x80) != 0) {
				offset += 0xFF00;
			}
			address = ProgramCounter + offset;//CHECK
											  //address += (WORD)((WORD)HB << 8) + LB;
			if (address >= 0 && address < MEMORY_SIZE) {
				//ProgramCounter = Memory[address];
				if ((Flags & FLAG_N) != 0) {
					ProgramCounter = address;
				}
			}
			break;
		//END BMI

		/*
		* BRA
		* branch on positive result
		* addressing modes: [rel]
		*
		*/
		//START BPL
		case 0xF8://BPL rel
			//HB = fetch();
			LB = fetch();
			offset = (WORD)LB;
			if ((offset & 0x80) != 0) {
				offset += 0xFF00;
			}
			address = ProgramCounter + offset;//CHECK
											  //address += (WORD)((WORD)HB << 8) + LB;
			if (address >= 0 && address < MEMORY_SIZE) {
				//ProgramCounter = Memory[address];
				if ((Flags & FLAG_N) == 0) {
					ProgramCounter = address;
				}
			}
			break;
		//END BPL

		/*
		* BGE
		* branch branch on result less than or equal to zero
		* addressing modes: [rel]
		*
		*/
		//START BGE
		case 0xF9://BGE rel
			//HB = fetch();
			LB = fetch();
			offset = (WORD)LB;
			if ((offset & 0x80) != 0) {
				offset += 0xFF00;
			}
			address = ProgramCounter + offset;//CHECK
											  //address += (WORD)((WORD)HB << 8) + LB;
			if (address >= 0 && address < MEMORY_SIZE) {
				//ProgramCounter = Memory[address];
				//if ( ((Flags & FLAG_N) != 0) ^ 
				//	 ((Flags & FLAG_V) != 0)) {
				if (!(((Flags & FLAG_N) == 0) ^
					((Flags & FLAG_V) == 0))) {
					ProgramCounter = address;
				}
			}
			break;
		//END BGE

		/*
		* BLE
		* branch on result greater than or equal to zero
		* addressing modes: [rel]
		*
		*/
		//START BLE
		case 0xFA://BLE rel
			//HB = fetch();
			LB = fetch();
			offset = (WORD)LB;
			if ((offset & 0x80) != 0) {
				offset += 0xFF00;
			}
			address = ProgramCounter + offset;//CHECK
											  //address += (WORD)((WORD)HB << 8) + LB;
			if (address >= 0 && address < MEMORY_SIZE) {
				//ProgramCounter = Memory[address];
				if ((((Flags & FLAG_N) == 0) ||
					 ((Flags & FLAG_Z) != 0)) ^
					 ((Flags & FLAG_V) == 0)) {
					ProgramCounter = address;
				}
			}
			break;
		//END BLE

		/*
		* BGT
		* branch on result less than zero
		* addressing modes: [rel]
		*
		*/
		//START BGT
		case 0xFB://BGT rel
			//HB = fetch();
			LB = fetch();
			offset = (WORD)LB;
			if ((offset & 0x80) != 0) {
				offset += 0xFF00;
			}
			address = ProgramCounter + offset;//CHECK
											  //address += (WORD)((WORD)HB << 8) + LB;
			if (address >= 0 && address < MEMORY_SIZE) {
				//ProgramCounter = Memory[address];
				//if (!(((Flags & FLAG_N) != 0) ||
				//	((Flags & FLAG_Z) == 0)) ^
				//	((Flags & FLAG_V) != 0)) {
				if (!((((Flags & FLAG_N) == 0) ||
					   ((Flags & FLAG_Z) != 0)) ^
					   ((Flags & FLAG_V) == 0))) {
					ProgramCounter = address;
				}
			}
			break;
		//END BGT

		/*
		* BLT
		* branch on result greater then zero
		* addressing modes: [rel]
		*
		*/
		//START BLT
		case 0xFC://BLT rel
			//HB = fetch();
			LB = fetch();
			offset = (WORD)LB;
			if ((offset & 0x80) != 0) {
				offset += 0xFF00;
			}
			address = ProgramCounter + offset;//CHECK
											  //address += (WORD)((WORD)HB << 8) + LB;
			if (address >= 0 && address < MEMORY_SIZE) {
				//ProgramCounter = Memory[address];
				if (((Flags & FLAG_N) == 0) ^
					((Flags & FLAG_V) == 0)) {
					ProgramCounter = address;
				}
			}
			break;
		//END BLT

		/*
		* CLC
		* clear carry flag
		* addressing modes: [impl]
		*
		*/
		//START CLC
		case 0x18://CLC impl
			Flags = Flags & (0xFF - FLAG_C);
			break;
		//END CLC

		/*
		* SEC
		* set carry flag
		* addressing modes: [impl]
		*
		*/
		//START SEC
		case 0x19://SEC impl
			Flags = Flags | FLAG_C;
			break;
		//END SEC
		
		/*
		* CLI
		* clear interrupt flag
		* addressing modes: [impl]
		*
		*/
		//START CLI
		case 0x1A://CLI impl
			Flags = Flags & (0xFF - FLAG_I);
			break;
		//END CLI

		/*
		* STI
		* set interrupt flag
		* addressing modes: [impl]
		*
		*/
		//START STI
		case 0x1B://STI impl
			Flags = Flags | FLAG_I;
			break;
		//END STI

		/*
		* STV
		* set overflow flag
		* addressing modes: [impl]
		*
		*/
		//START STV
		case 0x1C://STV impl
			Flags = Flags | FLAG_V;
			break;
		//END STV

		/*
		* CLV
		* clear overflow flag
		* addressing modes: [impl]
		*
		*/
		//START CLV
		case 0x1D://CLV impl
			Flags = Flags & (0xFF - FLAG_V);
			break;
		//END CLV

		/*
		* NOP
		* no operation
		* addressing modes: [impl]
		*
		*/
		//START NOP
		case 0x73://NOP impl
			break;
		//END NOP

		/*
		* HLT
		* halt
		* addressing modes: [impl]
		*
		*/
		//START HLT
		case 0x74://HLT impl
			halt = true;
			//sanity = 1000;
			break;
		//END HLT

		/*
		* SWI
		* software interrupt
		* addressing modes: [impl]
		*
		*/
		//START SWI
		case 0x16://SWI impl
			if ((StackPointer >= 7) && (StackPointer < MEMORY_SIZE)) {
				Memory[StackPointer] = Registers[REGISTER_A];
				StackPointer--;
				Memory[StackPointer] = Registers[REGISTER_B];
				StackPointer--;
				Memory[StackPointer] = Registers[REGISTER_C];
				StackPointer--;
				Memory[StackPointer] = Registers[REGISTER_D];
				StackPointer--;
				Memory[StackPointer] = Registers[REGISTER_E];
				StackPointer--;
				Memory[StackPointer] = Registers[REGISTER_F];
				StackPointer--;
				Memory[StackPointer] = Flags;
				StackPointer--;
			}
		//END SWI

		/*
		* RTI
		* return form interrupt
		* addressing modes: [impl]
		*
		*/
		//START RTI
		case 0x17://SWI impl
			if ((StackPointer >= 0) && (StackPointer < MEMORY_SIZE - 1)) {
				StackPointer++;
				Flags = Memory[StackPointer];
				StackPointer++;
				Registers[REGISTER_F] = Memory[StackPointer];
				StackPointer++;
				Registers[REGISTER_E] = Memory[StackPointer];
				StackPointer++;
				Registers[REGISTER_D] = Memory[StackPointer];
				StackPointer++;
				Registers[REGISTER_C] = Memory[StackPointer];
				StackPointer++;
				Registers[REGISTER_B] = Memory[StackPointer];
				StackPointer++;
				Registers[REGISTER_A] = Memory[StackPointer];
			}
			break;
		//END RTI	
	}
}

void Group_2_Move(BYTE opcode)
{
	//printf("group2");
	printf("|G2|op=%X|", opcode);
	switch(opcode) 
	{
	//START LD
	/*
	* LD
	* transfer one register to another
	* addressing modes: [a, b, c, d, e, f,- a, b, c, d, e, f]
	*
	*/
		//A
	case 0x2A:
		Registers[REGISTER_A] = Registers[REGISTER_A];
		break;
	case 0x3A:
		Registers[REGISTER_A] = Registers[REGISTER_B];
		break;
	case 0x4A:
		Registers[REGISTER_A] = Registers[REGISTER_C];
		break;
	case 0x5A:
		Registers[REGISTER_A] = Registers[REGISTER_D];
		break;
	case 0x6A:
		Registers[REGISTER_A] = Registers[REGISTER_E];
		break;
	case 0x7A:
		Registers[REGISTER_A] = Registers[REGISTER_F];
		break;
		//B
	case 0x2B:
		Registers[REGISTER_B] = Registers[REGISTER_A];
		break;
	case 0x3B:
		Registers[REGISTER_B] = Registers[REGISTER_B];
		break;
	case 0x4B:
		Registers[REGISTER_B] = Registers[REGISTER_C];
		break;
	case 0x5B:
		Registers[REGISTER_B] = Registers[REGISTER_D];
		break;
	case 0x6B:
		Registers[REGISTER_B] = Registers[REGISTER_E];
		break;
	case 0x7B:
		Registers[REGISTER_B] = Registers[REGISTER_F];
		break;
		//C
	case 0x2C:
		Registers[REGISTER_C] = Registers[REGISTER_A];
		break;
	case 0x3C:
		Registers[REGISTER_C] = Registers[REGISTER_B];
		break;
	case 0x4C:
		Registers[REGISTER_C] = Registers[REGISTER_C];
		break;
	case 0x5C:
		Registers[REGISTER_C] = Registers[REGISTER_D];
		break;
	case 0x6C:
		Registers[REGISTER_C] = Registers[REGISTER_E];
		break;
	case 0x7C:
		Registers[REGISTER_C] = Registers[REGISTER_F];
		break;
		//D
	case 0x2D:
		Registers[REGISTER_D] = Registers[REGISTER_A];
		break;
	case 0x3D:
		Registers[REGISTER_D] = Registers[REGISTER_B];
		break;
	case 0x4D:
		Registers[REGISTER_D] = Registers[REGISTER_C];
		break;
	case 0x5D:
		Registers[REGISTER_D] = Registers[REGISTER_D];
		break;
	case 0x6D:
		Registers[REGISTER_D] = Registers[REGISTER_E];
		break;
	case 0x7D:
		Registers[REGISTER_D] = Registers[REGISTER_F];
		break;
		//E
	case 0x2E:
		Registers[REGISTER_E] = Registers[REGISTER_A];
		break;
	case 0x3E:
		Registers[REGISTER_E] = Registers[REGISTER_B];
		break;
	case 0x4E:
		Registers[REGISTER_E] = Registers[REGISTER_C];
		break;
	case 0x5E:
		Registers[REGISTER_E] = Registers[REGISTER_D];
		break;
	case 0x6E:
		Registers[REGISTER_E] = Registers[REGISTER_E];
		break;
	case 0x7E:
		Registers[REGISTER_E] = Registers[REGISTER_F];
		break;
		//F
	case 0x2F:
		Registers[REGISTER_F] = Registers[REGISTER_A];
		break;
	case 0x3F:
		Registers[REGISTER_F] = Registers[REGISTER_B];
		break;
	case 0x4F:
		Registers[REGISTER_F] = Registers[REGISTER_C];
		break;
	case 0x5F:
		Registers[REGISTER_F] = Registers[REGISTER_D];
		break;
	case 0x6F:
		Registers[REGISTER_F] = Registers[REGISTER_E];
		break;
	case 0x7F:
		Registers[REGISTER_F] = Registers[REGISTER_F];
		break;
	//END LD
	}
}


void execute(BYTE opcode)
{	

	//if ((opcode >= 0x2A) && (opcode <= 0x7F))
	if (((opcode >= 0x2A) && (opcode <= 0x2F))
		|| ((opcode >= 0x3A) && (opcode <= 0x3F))
		|| ((opcode >= 0x4A) && (opcode <= 0x4F))
		|| ((opcode >= 0x5A) && (opcode <= 0x5F))
		|| ((opcode >= 0x6A) && (opcode <= 0x6F))
		|| ((opcode >= 0x7A) && (opcode <= 0x7F)))
	{
		Group_2_Move(opcode);
	}
	else
	{
		Group_1(opcode);
	}
}



///////////////////////////////////////////////
//end emulator code

void emulate()
{
	BYTE opcode;
	int sanity;

	sanity = 0;
	ProgramCounter = 0;
	halt = false;
	memory_in_range = true;

	printf("			          A  B  C  D  E  F  X  Y  SP\n");

	while ((!halt) && (memory_in_range)) {
		sanity++;
		if (sanity > 500) halt = true;
		printf("%04X ", ProgramCounter);           // Print current address
		opcode = fetch();
		execute(opcode);

		printf("%s  ", opcode_mneumonics[opcode]);  // Print current opcode

		printf("%02X ", Registers[REGISTER_A]);
		printf("%02X ", Registers[REGISTER_B]);
		printf("%02X ", Registers[REGISTER_C]);
		printf("%02X ", Registers[REGISTER_D]);
		printf("%02X ", Registers[REGISTER_E]);
		printf("%02X ", Registers[REGISTER_F]);
		printf("%02X ", Index_Registers[REGISTER_X]);
		printf("%02X ", Index_Registers[REGISTER_Y]);
		printf("%04X ", StackPointer);              // Print Stack Pointer
		printf ("||0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X||",
			Memory[TEST_ADDRESS_1],
			Memory[TEST_ADDRESS_2],
			Memory[TEST_ADDRESS_3],
			Memory[TEST_ADDRESS_4],
			Memory[TEST_ADDRESS_5],
			Memory[TEST_ADDRESS_6],
			Memory[TEST_ADDRESS_7],
			Memory[TEST_ADDRESS_8],
			Memory[TEST_ADDRESS_9],
			Memory[TEST_ADDRESS_10],
			Memory[TEST_ADDRESS_11],
			Memory[TEST_ADDRESS_12]
			);

		if ((Flags & FLAG_I) == FLAG_I)	
		{
			printf("I=1 ");
		}
		else
		{
			printf("I=0 ");
		}
		if ((Flags & FLAG_V) == FLAG_V)	
		{
			printf("V=1 ");
		}
		else
		{
			printf("V=0 ");
		}
		if ((Flags & FLAG_N) == FLAG_N)	
		{
			printf("N=1 ");
		}
		else
		{
			printf("N=0 ");
		}
		if ((Flags & FLAG_Z) == FLAG_Z)	
		{
			printf("Z=1 ");
		}
		else
		{
			printf("Z=0 ");
		}
		if ((Flags & FLAG_C) == FLAG_C)	
		{
			printf("C=1 ");
		}
		else
		{
			printf("C=0 ");
		}

		printf("\n");  // New line
	}

	printf("\n");  // New line
}


////////////////////////////////////////////////////////////////////////////////
//                            Simulator/Emulator (End)                        //
////////////////////////////////////////////////////////////////////////////////


void initialise_filenames() {
	int i;

	for (i=0; i<MAX_FILENAME_SIZE; i++) {
		hex_file [i] = '\0';
		trc_file [i] = '\0';
	}
}




int find_dot_position(char *filename) {
	int  dot_position;
	int  i;
	char chr;

	dot_position = 0;
	i = 0;
	chr = filename[i];

	while (chr != '\0') {
		if (chr == '.') {
			dot_position = i;
		}
		i++;
		chr = filename[i];
	}

	return (dot_position);
}


int find_end_position(char *filename) {
	int  end_position;
	int  i;
	char chr;

	end_position = 0;
	i = 0;
	chr = filename[i];

	while (chr != '\0') {
		end_position = i;
		i++;
		chr = filename[i];
	}

	return (end_position);
}


bool file_exists(char *filename) {
	bool exists;
	FILE *ifp;

	exists = false;

	if ( ( ifp = fopen( filename, "r" ) ) != NULL ) {
		exists = true;

		fclose(ifp);
	}

	return (exists);
}



void create_file(char *filename) {
	FILE *ofp;

	if ( ( ofp = fopen( filename, "w" ) ) != NULL ) {
		fclose(ofp);
	}
}



bool getline(FILE *fp, char *buffer) {
	bool rc;
	bool collect;
	char c;
	int  i;

	rc = false;
	collect = true;

	i = 0;
	while (collect) {
		c = getc(fp);

		switch (c) {
		case EOF:
			if (i > 0) {
				rc = true;
			}
			collect = false;
			break;

		case '\n':
			if (i > 0) {
				rc = true;
				collect = false;
				buffer[i] = '\0';
			}
			break;

		default:
			buffer[i] = c;
			i++;
			break;
		}
	}

	return (rc);
}






void load_and_run(int args,_TCHAR** argv) {
	char chr;
	int  ln;
	int  dot_position;
	int  end_position;
	long i;
	FILE *ifp;
	long address;
	long load_at;
	int  code;

	// Prompt for the .hex file

	printf("\n");
	printf("Enter the hex filename (.hex): ");

	if(args == 2){
		ln = 0;
		chr = argv[1][ln];
		while (chr != '\0')
		{
			if (ln < MAX_FILENAME_SIZE)
			{
				hex_file [ln] = chr;
				trc_file [ln] = chr;
				ln++;
			}
			chr = argv[1][ln];
		}
	} else {
		ln = 0;
		chr = '\0';
		while (chr != '\n') {
			chr = getchar();

			switch(chr) {
			case '\n':
				break;
			default:
				if (ln < MAX_FILENAME_SIZE)	{
					hex_file [ln] = chr;
					trc_file [ln] = chr;
					ln++;
				}
				break;
			}
		}

	}
	// Tidy up the file names

	dot_position = find_dot_position(hex_file);
	if (dot_position == 0) {
		end_position = find_end_position(hex_file);

		hex_file[end_position + 1] = '.';
		hex_file[end_position + 2] = 'h';
		hex_file[end_position + 3] = 'e';
		hex_file[end_position + 4] = 'x';
		hex_file[end_position + 5] = '\0';
	} else {
		hex_file[dot_position + 0] = '.';
		hex_file[dot_position + 1] = 'h';
		hex_file[dot_position + 2] = 'e';
		hex_file[dot_position + 3] = 'x';
		hex_file[dot_position + 4] = '\0';
	}

	dot_position = find_dot_position(trc_file);
	if (dot_position == 0) {
		end_position = find_end_position(trc_file);

		trc_file[end_position + 1] = '.';
		trc_file[end_position + 2] = 't';
		trc_file[end_position + 3] = 'r';
		trc_file[end_position + 4] = 'c';
		trc_file[end_position + 5] = '\0';
	} else {
		trc_file[dot_position + 0] = '.';
		trc_file[dot_position + 1] = 't';
		trc_file[dot_position + 2] = 'r';
		trc_file[dot_position + 3] = 'c';
		trc_file[dot_position + 4] = '\0';
	}

	if (file_exists(hex_file)) {
		// Clear Registers and Memory

		Registers[REGISTER_A] = 0;
		Registers[REGISTER_B] = 0;
		Registers[REGISTER_C] = 0;
		Registers[REGISTER_D] = 0;
		Registers[REGISTER_E] = 0;
		Registers[REGISTER_F] = 0;
		Index_Registers[REGISTER_X] = 0;
		Index_Registers[REGISTER_Y] = 0;
		Flags = 0;
		ProgramCounter = 0;
		StackPointer = 0;

		for (i=0; i<MEMORY_SIZE; i++) {
			Memory[i] = 0x00;
		}

		// Load hex file

		if ( ( ifp = fopen( hex_file, "r" ) ) != NULL ) {
			printf("Loading file...\n\n");

			load_at = 0;

			while (getline(ifp, InputBuffer)) {
				if (sscanf(InputBuffer, "L=%x", &address) == 1) {
					load_at = address;
				} else if (sscanf(InputBuffer, "%x", &code) == 1) {
					if ((load_at >= 0) && (load_at <= MEMORY_SIZE)) {
						Memory[load_at] = (BYTE)code;
					}
					load_at++;
				} else {
					printf("ERROR> Failed to load instruction: %s \n", InputBuffer);
				}
			}

			fclose(ifp);
		}

		// Emulate

		emulate();
	} else {
		printf("\n");
		printf("ERROR> Input file %s does not exist!\n", hex_file);
		printf("\n");
	}
}

void building(int args,_TCHAR** argv){
	char buffer[1024];
	load_and_run(args,argv);
	sprintf(buffer, "0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X", 
		Memory[TEST_ADDRESS_1],
		Memory[TEST_ADDRESS_2],
		Memory[TEST_ADDRESS_3],
		Memory[TEST_ADDRESS_4], 
		Memory[TEST_ADDRESS_5],
		Memory[TEST_ADDRESS_6], 
		Memory[TEST_ADDRESS_7],
		Memory[TEST_ADDRESS_8], 
		Memory[TEST_ADDRESS_9], 
		Memory[TEST_ADDRESS_10],
		Memory[TEST_ADDRESS_11],
		Memory[TEST_ADDRESS_12]
		);
	sendto(sock, buffer, strlen(buffer), 0, (SOCKADDR *)&server_addr, sizeof(SOCKADDR));
}



void test_and_mark() {
	char buffer[1024];
	bool testing_complete;
	int  len = sizeof(SOCKADDR);
	char chr;
	int  i;
	int  j;
	bool end_of_program;
	long address;
	long load_at;
	int  code;
	int  mark;
	int  passed;

	printf("\n");
	printf("Automatic Testing and Marking\n");
	printf("\n");

	testing_complete = false;

	sprintf(buffer, "Test Student %s", STUDENT_NUMBER);
	sendto(sock, buffer, strlen(buffer), 0, (SOCKADDR *)&server_addr, sizeof(SOCKADDR));

	while (!testing_complete) {
		memset(buffer, '\0', sizeof(buffer));

		if (recvfrom(sock, buffer, sizeof(buffer)-1, 0, (SOCKADDR *)&client_addr, &len) != SOCKET_ERROR) {
			printf("Incoming Data: %s \n", buffer);

			//if (strcmp(buffer, "Testing complete") == 1)
			if (sscanf(buffer, "Testing complete %d", &mark) == 1) {
				testing_complete = true;
				printf("Current mark = %d\n", mark);

			}else if (sscanf(buffer, "Tests passed %d", &passed) == 1) {
				//testing_complete = true;
				printf("Passed = %d\n", passed);

			} else if (strcmp(buffer, "Error") == 0) {
				printf("ERROR> Testing abnormally terminated\n");
				testing_complete = true;
			} else {
				// Clear Registers and Memory

		Registers[REGISTER_A] = 0;
		Registers[REGISTER_B] = 0;
		Registers[REGISTER_C] = 0;
		Registers[REGISTER_D] = 0;
		Registers[REGISTER_E] = 0;
		Registers[REGISTER_F] = 0;
		Index_Registers[REGISTER_X] = 0;
		Index_Registers[REGISTER_Y] = 0;
				Flags = 0;
				ProgramCounter = 0;
				StackPointer = 0;
				for (i=0; i<MEMORY_SIZE; i++) {
					Memory[i] = 0;
				}

				// Load hex file

				i = 0;
				j = 0;
				load_at = 0;
				end_of_program = false;
				FILE *ofp;
				fopen_s(&ofp ,"branch.txt", "a");

				while (!end_of_program) {
					chr = buffer[i];
					switch (chr) {
					case '\0':
						end_of_program = true;

					case ',':
						if (sscanf(InputBuffer, "L=%x", &address) == 1) {
							load_at = address;
						} else if (sscanf(InputBuffer, "%x", &code) == 1) {
							if ((load_at >= 0) && (load_at <= MEMORY_SIZE)) {
								Memory[load_at] = (BYTE)code;
								fprintf(ofp, "%02X\n", (BYTE)code);
							}
							load_at++;
						} else {
							printf("ERROR> Failed to load instruction: %s \n", InputBuffer);
						}
						j = 0;
						break;

					default:
						InputBuffer[j] = chr;
						j++;
						break;
					}
					i++;
				}
				fclose(ofp);
				// Emulate

				if (load_at > 1) {
					emulate();
					// Send and store results
					sprintf(buffer, "%02X%02X %02X%02X %02X%02X %02X%02X %02X%02X %02X%02X", 
						Memory[TEST_ADDRESS_1],
						Memory[TEST_ADDRESS_2],
						Memory[TEST_ADDRESS_3],
						Memory[TEST_ADDRESS_4], 
						Memory[TEST_ADDRESS_5],
						Memory[TEST_ADDRESS_6], 
						Memory[TEST_ADDRESS_7],
						Memory[TEST_ADDRESS_8], 
						Memory[TEST_ADDRESS_9], 
						Memory[TEST_ADDRESS_10],
						Memory[TEST_ADDRESS_11],
						Memory[TEST_ADDRESS_12]
						);
					sendto(sock, buffer, strlen(buffer), 0, (SOCKADDR *)&server_addr, sizeof(SOCKADDR));
				}
			}
		}
	}
}



int _tmain(int argc, _TCHAR* argv[])
{
	char chr;
	char dummy;

	printf("\n");
	printf("Microprocessor Emulator\n");
	printf("UWE Computer and Network Systems Assignment 1\n");
	printf("\n");

	initialise_filenames();

	if (WSAStartup(MAKEWORD(2, 2), &data) != 0) return(0);

	sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);  // Here we create our socket, which will be a UDP socket (SOCK_DGRAM).
	if (!sock) {	
		// Creation failed! 
	}

	memset(&server_addr, 0, sizeof(SOCKADDR_IN));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(IP_ADDRESS_SERVER);
	server_addr.sin_port = htons(PORT_SERVER);

	memset(&client_addr, 0, sizeof(SOCKADDR_IN));
	client_addr.sin_family = AF_INET;
	client_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	client_addr.sin_port = htons(PORT_CLIENT);

	chr = '\0';
	while ((chr != 'e') && (chr != 'E'))
	{
		printf("\n");
		printf("Please select option\n");
		printf("L - Load and run a hex file\n");
		printf("T - Have the server test and mark your emulator\n");
		printf("E - Exit\n");
		if(argc == 2){ building(argc,argv); exit(0);}
		printf("Enter option: ");
		chr = getchar();
		if (chr != 0x0A)
		{
			dummy = getchar();  // read in the <CR>
		}
		printf("\n");

		switch (chr)
		{
		case 'L':
		case 'l':
			load_and_run(argc,argv);
			break;

		case 'T':
		case 't':
			test_and_mark();
			break;

		default:
			break;
		}
	}

	closesocket(sock);
	WSACleanup();


	return 0;
}