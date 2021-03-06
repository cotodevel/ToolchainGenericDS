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


#include "typedefsTGDS.h"
#include "dsregs.h"
#include "dsregs_asm.h"

#include "dmaTGDS.h"

//DMA: 

void dmaFill(sint32 dmachannel,uint32 value, uint32 dest, uint32 mode){
#ifdef ARM7	
	DMAXSAD(dmachannel) = (uint32)&value;
#endif
#ifdef ARM9
	DMAXFILL(dmachannel) = (vuint32)value;
	DMAXSAD(dmachannel) = (uint32)&DMAXFILL(dmachannel);
#endif

    DMAXDAD(dmachannel) = (uint32)dest;
	DMAXCNT(dmachannel) = mode;
	while(DMAXCNT(dmachannel) & DMAENABLED);
}

void dmaFillWord(sint32 dmachannel,uint32 value, uint32 dest, uint32 word_count){
	dmaFill(dmachannel,value,dest,(DMAFIXED_SRC | DMAINCR_DEST | DMA32BIT | DMASTART_INMEDIATE | DMAENABLED | (word_count>>2)));
}

void dmaFillHalfWord(sint32 dmachannel,uint32 value, uint32 dest, uint32 word_count){
	dmaFill(dmachannel,value,dest,(DMAFIXED_SRC | DMAINCR_DEST | DMA16BIT | DMASTART_INMEDIATE | DMAENABLED | (word_count>>1)));
}

void dmaTransfer(sint32 dmachannel, uint32 source, uint32 dest, uint32 mode){
	DMAXSAD(dmachannel)= source;
	DMAXDAD(dmachannel)= dest;
	DMAXCNT(dmachannel)= mode;
	while(DMAXCNT(dmachannel) & DMAENABLED);
}

void dmaTransferHalfWord(sint32 dmachannel, uint32 source, uint32 dest, uint32 word_count){
	dmaTransfer(dmachannel, source, dest, (DMAINCR_SRC | DMAINCR_DEST | DMA16BIT | DMASTART_INMEDIATE | DMAENABLED | (word_count>>1)));
}

void dmaTransferWord(sint32 dmachannel, uint32 source, uint32 dest, uint32 word_count){
	dmaTransfer(dmachannel, source, dest, (DMAINCR_SRC | DMAINCR_DEST | DMA32BIT | DMASTART_INMEDIATE | DMAENABLED | (word_count>>2)));
}