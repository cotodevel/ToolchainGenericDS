/*

			Copyright (C) 2017  Coto
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301
USA

*/

//Coto: these are my FIFO handling libs. Works fine with NIFI (trust me this is very tricky to do without falling into freezes).
//Use it at your will, just make sure you read WELL the descriptions below.


#include "common_shared.h"
#include "InterruptsARMCores_h.h"
#include "ipc.h"


#ifdef ARM7
#include <string.h>
#endif

#ifdef ARM9
#include <stdbool.h>

#include "dsregs.h"
#include "dsregs_asm.h"
#include "InterruptsARMCores_h.h"
#include "wifi_arm9.h"

#endif







//Software FIFO calls, Rely on Hardware FIFO calls so it doesnt matter if they are in different maps 
#ifdef ARM9
__attribute__((section(".dtcm")))
#endif    
volatile int FIFO_SOFT_PTR = 0;
#ifdef ARM9
__attribute__((section(".dtcm")))
#endif    
volatile uint32 FIFO_BUF_SOFT[FIFO_NDS_HW_SIZE/4];

//useful for checking if something is pending
int GetSoftFIFOCount(){
	return FIFO_SOFT_PTR;
}

//GetSoftFIFO: Stores up to FIFO_NDS_HW_SIZE. Exposed to usercode for fetching up to 64 bytes (in 4 bytes each) sent from other core, until it returns false (empty buffer).

//Example: 
//uint32 n = 0;
//while(GetSoftFIFO(&n)== true){
//	//n has 4 bytes from the other ARM Core.
//}
#ifdef ARM9
__attribute__((section(".itcm")))
#endif    
bool GetSoftFIFO(uint32 * var)
{
	if(FIFO_SOFT_PTR >= 1){
		FIFO_SOFT_PTR--;
		*var = (uint32)FIFO_BUF_SOFT[FIFO_SOFT_PTR];
		FIFO_BUF_SOFT[FIFO_SOFT_PTR] = (uint32)0;
		return true;
	}
	else
		return false;
}

//SetSoftFIFO == false means FULL
#ifdef ARM9
__attribute__((section(".itcm")))
#endif    
//returns ammount of inserted uint32 blocks into FIFO hardware regs
bool SetSoftFIFO(uint32 value)
{
	if(FIFO_SOFT_PTR < (int)(FIFO_NDS_HW_SIZE/4)){
		FIFO_BUF_SOFT[FIFO_SOFT_PTR] = value;
		FIFO_SOFT_PTR++;
		return true;
	}
	else
		return false;
}


//Software FIFO Handler receiver, add it wherever its required. (This is a FIFO that runs on software logic, rather than NDS FIFO).
//Supports multiple arguments, in FIFO format.
#ifdef ARM9
__attribute__((section(".itcm")))
#endif
void Handle_SoftFIFORECV()
{
	uint32 msg = 0;
	
	while(GetSoftFIFO((uint32*)&msg) == true){
		
		//process incoming packages
		switch(msg){
			
			
		}
		
	}
}

//Software FIFO Sender. Send from external ARM Core, receive from the above function. Uses hardware FIFO.
#ifdef ARM9
__attribute__((section(".itcm")))
#endif
void SoftFIFOSEND(uint32 value0,uint32 value1,uint32 value2,uint32 value3){
	//todo: needs hardware IPC FIFO implementation.
}

//64K Shared buffer. ARM9 defines it. To be used with IPC FIFO Hardware uint32 * buffer_shared arg
#ifdef ARM9
volatile uint8 arm7arm9sharedBuffer[ARM7ARM9SHAREDBUFFERSIZE];
#endif

#ifdef ARM9
__attribute__((section(".itcm")))
#endif
void SendMultipleWordACK(uint32 data0, uint32 data1, uint32 data2, uint32 * buffer_shared){
	SendMultipleWordByFifo(data0, data1, data2, buffer_shared);
}

//command0, command1, command2, buffer to pass to other core.
//IPC FIFO non blocking
#ifdef ARM9
__attribute__((section(".itcm")))
#endif
void SendMultipleWordByFifo(uint32 data0, uint32 data1, uint32 data2, uint32 * buffer_shared)
{
	volatile uint32 * data0ptr = (uint32*)&MyIPC->IPC_FIFOMSG[0];
	volatile uint32 * data1ptr = (uint32*)&MyIPC->IPC_FIFOMSG[1];
	volatile uint32 * data2ptr = (uint32*)&MyIPC->IPC_FIFOMSG[2];
	volatile uint32 * data3ptr = (uint32*)&MyIPC->IPC_FIFOMSG[3];
	
	*data0ptr = (uint32)data0;
	*data1ptr = (uint32)data1;
	*data2ptr = (uint32)data2;
	*data3ptr = (uint32)(uint32*)buffer_shared;
	
	REG_IPC_FIFO_TX =	(uint32)FIFO_IPC_MESSAGE;
}

//FIFO HANDLER INIT
#ifdef ARM9
__attribute__((section(".itcm")))
#endif
void HandleFifoEmpty(){
	HandleFifoEmptyWeakRef((uint32)0,(uint32)0,(uint32)0,(uint32)0);
}

//FIFO HANDLER INIT
#ifdef ARM9
__attribute__((section(".itcm")))
#endif
void HandleFifoNotEmpty(){
	
	volatile uint32 cmd1 = 0,cmd2 = 0,cmd3 = 0,cmd4 = 0,cmd5 = 0,cmd6 = 0,cmd7 = 0,cmd8 = 0,cmd9 = 0,cmd10 = 0,cmd11 = 0,cmd12 = 0,cmd13 = 0,cmd14 = 0,cmd15 = 0;
	
	while(!(REG_IPC_FIFO_CR & RECV_FIFO_IPC_EMPTY)){
		cmd1 = REG_IPC_FIFO_RX;
		
		if((uint32)FIFO_IPC_MESSAGE == (uint32)cmd1){
			
			volatile uint32 * data0ptr = (uint32*)&MyIPC->IPC_FIFOMSG[0];
			volatile uint32 * data1ptr = (uint32*)&MyIPC->IPC_FIFOMSG[1];
			volatile uint32 * data2ptr = (uint32*)&MyIPC->IPC_FIFOMSG[2];
			volatile uint32 * data3ptr = (uint32*)&MyIPC->IPC_FIFOMSG[3];
			
			volatile uint32 data0 = *data0ptr;
			volatile uint32 data1 = *data1ptr;
			volatile uint32 data2 = *data2ptr;
			volatile uint32 data3 = *data3ptr;
			
			HandleFifoNotEmptyWeakRef(data0,data1,data2,data3);
			
			*data0ptr = (uint32)0;
			*data1ptr = (uint32)0;
			*data2ptr = (uint32)0;
			*data3ptr = (uint32)0;
			
		}
		
		//clear fifo inmediately
		REG_IPC_FIFO_CR |= (1<<3);
	}
	
}


void setARM7ARM9SharedBuffer(uint32 * shared_buffer_address){
	volatile uint32 * ptr = (uint32*)&MyIPC->arm7arm9sharedBuffer;
	*ptr = (uint32)(uint32*)shared_buffer_address;
}

uint32 * getARM7ARM9SharedBuffer(){
	volatile uint32 * ptr = (uint32*)&MyIPC->arm7arm9sharedBuffer;
	return (uint32)(*ptr);
}


//FIFO HANDLER END


//Hardware Shared Functions
#ifdef ARM9
__attribute__((section(".itcm")))
#endif
void writemap_ext_armcore(uint32 address, uint32 value, uint32 mode){
	//todo.
}

//writemap_ext_armcore(REG_POWERCNT_ADDR, (uint32)value, WRITE_VALUE_16);

void powerON(uint16 values){
	#ifdef ARM7
	REG_POWERCNT |= values;
	#endif
	
	#ifdef ARM9
	if(!(values & POWERMAN_ARM9)){
		//SendArm7Command(FIFO_POWERCNT_ON, (uint32)values, 0x00000000, 0x00000000);
		SendMultipleWordACK(FIFO_POWERCNT_ON, (uint32)values, 0, 0);
	}
	else{
		REG_POWERCNT |= values;
	}
	#endif
}

void powerOFF(uint16 values){
	#ifdef ARM7
	REG_POWERCNT &= ~values;
	#endif
	
	#ifdef ARM9
	if(!(values & POWERMAN_ARM9)){
		//SendArm7Command(FIFO_POWERCNT_OFF, (uint32)values, 0x00000000, 0x00000000);
		SendMultipleWordACK(FIFO_POWERCNT_OFF, (uint32)values, 0, 0);
	}
	else{
		REG_POWERCNT &= ~values;
	}
	#endif
}