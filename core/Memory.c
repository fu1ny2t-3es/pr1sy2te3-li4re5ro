/* ----------------------------------------------------------------------------
 *   ___  ___  ___  ___       ___  ____  ___  _  _
 *  /__/ /__/ /  / /__  /__/ /__    /   /_   / |/ /
 * /    / \  /__/ ___/ ___/ ___/   /   /__  /    /  emulator
 *
 * ----------------------------------------------------------------------------
 * Copyright 2005 Greg Stanton
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 * ----------------------------------------------------------------------------
 * Memory.c
 * ----------------------------------------------------------------------------
 */
#include <stdlib.h>
#include "ProSystem.h"
#include "Memory.h"
#include "Equates.h"
#include "Bios.h"
#include "Cartridge.h"
#include "Pokey.h"
#include "Tia.h"
#include "Riot.h"
#include "Maria.h"
#include "Mapper.h"

uint8_t memory_ram[MEMORY_SIZE] = {0};

uint8_t memory_exram[MEMORY_EXRAM_SIZE] = {0};
uint32_t memory_exram_size = 0;

uint8_t memory_nvram[MEMORY_NVRAM_SIZE] = {0};
uint32_t memory_nvram_size = 0;


uint8_t memory_ReadOpenBus(uint16_t address)
{
   return address & 0xff;
}

void memory_WriteOpenBus(uint16_t address, uint8_t data)
{
   return;
}

void memory_Map(void)
{
   sally_SetRead(0x40, 0x100, memory_ram + 0x2040);
   maria_SetRead(0x40, 0x100, memory_ram + 0x2040);
   sally_SetRead(0x140, 0x200, memory_ram + 0x2140);
   maria_SetRead(0x140, 0x200, memory_ram + 0x2140);

   sally_SetRead(0x240, 0x280, memory_ram + 0x2040);
   maria_SetRead(0x240, 0x280, memory_ram + 0x2040);
   sally_SetRead(0x340, 0x380, memory_ram + 0x2140);
   maria_SetRead(0x340, 0x380, memory_ram + 0x2140);

   sally_SetRead(0x1800, 0x2800, memory_ram + 0x1800);
   maria_SetRead(0x1800, 0x2800, memory_ram + 0x1800);

   sally_SetWrite(0x40, 0x100, memory_ram + 0x2040);
   sally_SetWrite(0x140, 0x200, memory_ram + 0x2140);
   sally_SetWrite(0x240, 0x280, memory_ram + 0x2040);
   sally_SetWrite(0x340, 0x380, memory_ram + 0x2140);

   sally_SetWrite(0x1800, 0x2800, memory_ram + 0x1800);
}

void memory_Reset(void)
{
   memset(memory_ram + 0x1800, 0, 0x1000);
}

uint8_t memory_Read(uint16_t address)
{
   int offset = address & 0xff;

   switch (address >> 8)  /* bank */
   {
   case 0:
   case 1:
      if (offset >= 0x20)
         return maria_Read(offset);

      return tia_Read(offset);


   case 2:
   case 3:
      if (offset >= 0x80)
         return riot_Read(offset);

      if (offset >= 0x20)
         return maria_Read(offset);

      return tia_Read(offset);


   case 4:
   case 5:
      if (offset < 0x80)
         return cartridge_Read(address);

      break;


   default:
      return cartridge_Read(address);
   }

   return memory_ReadOpenBus(address);
}

void memory_Write(uint16_t address, uint8_t data)
{
   int offset = address & 0xff;

   switch (address >> 8)
   {
<<<<<<< HEAD
      switch(address)
      {
         case WSYNC:
            if(!(cartridge_flags & 128))
               memory_ram[WSYNC] = true;
            break;
         case INPTCTRL:
            if(data == 22 && cartridge_IsLoaded()) 
               cartridge_Store(); 
            else if(data == 2 && bios_enabled)
               bios_Store();
            break;
         case INPT0:
         case INPT1:
         case INPT2:
         case INPT3:
         case INPT4:
         case INPT5:
            break;
         case AUDC0:
            tia_SetRegister(AUDC0, data);
            break;
         case AUDC1:
            tia_SetRegister(AUDC1, data);
            break;
         case AUDF0:
            tia_SetRegister(AUDF0, data);
            break;
         case AUDF1:
            tia_SetRegister(AUDF1, data);
            break;
         case AUDV0:
            tia_SetRegister(AUDV0, data);
            break;
         case AUDV1:
            tia_SetRegister(AUDV1, data);
            break;
         case SWCHA:	/*gdement:  Writing here actually writes to DRA inside the RIOT chip.
                       This value only indirectly affects output of SWCHA.  Ditto for SWCHB.*/
            riot_SetDRA(data);
            break;
         case SWCHB:
            riot_SetDRB(data);
            break;
         case TIM1T:
         case TIM1T | 0x8:
            riot_SetTimer(TIM1T, data);
            break;
         case TIM8T:
         case TIM8T | 0x8:
            riot_SetTimer(TIM8T, data);
            break;
         case TIM64T:
         case TIM64T | 0x8:
            riot_SetTimer(TIM64T, data);
            break;
         case T1024T:
         case T1024T | 0x8:
            riot_SetTimer(T1024T, data);
            break;
         default:
            if(cartridge_type == CARTRIDGE_TYPE_SOUPER && address >= 0x4000 && address < 0x8000)
            {
               memory_souper_ram[memory_souper_GetRamAddress(address)] = data;
               break;
            }
            /* Route writes to POKEY mapped below $4000 ($0440, $0450, $0800).
             * Writes to POKEY at $4000 are handled in cartridge_Write (ROM space). */
            if(cartridge_pokey_address && cartridge_pokey_address < 0x4000 &&
               address >= cartridge_pokey_address &&
               address < (uint16_t)(cartridge_pokey_address + 10))
            {
               pokey_SetRegister((uint16_t)(POKEY_AUDF1 + (address - cartridge_pokey_address)), data);
               break;
            }
            memory_ram[address] = data;
            if(address >= 8256 && address <= 8447)
               memory_ram[address - 8192] = data;
            else if(address >= 8512 && address <= 8702)
               memory_ram[address - 8192] = data;
            else if(address >= 64 && address <= 255)
               memory_ram[address + 8192] = data;
            else if(address >= 320 && address <= 511)
               memory_ram[address + 8192] = data;
            break;
            /*TODO: gdement:  test here for debug port.  Don't put it in the switch because that will change behavior.*/
      }
   }
   else
=======
   case 0:
   case 1:
      if (offset >= 0x20)
         maria_Write(offset, data);

	  else
         tia_Write(offset, data);

      break;


   case 2:
   case 3:
      if (offset >= 0x80)
         riot_Write(offset, data);

      else if (offset >= 0x20)
         maria_Write(offset, data);

      else
         tia_Write(offset, data);

      break;


   case 4:
   case 5:
      if (offset < 0x80)
         cartridge_Write(address, data);

      break;


   default:
>>>>>>> ba98f8e (Create a.yml)
      cartridge_Write(address, data);
   }
}

void memory_LoadState(void)
{
   prosystem_ReadStatePtr(memory_ram + 0x1800, 0x1000);
}

void memory_SaveState(void)
{
   prosystem_WriteStatePtr(memory_ram + 0x1800, 0x1000);
}
