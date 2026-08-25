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
 * This library is free software; you can redistribute it and/or modify it   
 * under the terms of version 2 of the GNU Library General Public License    
 * as published by the Free Software Foundation.                             
 *                                                                           
 * This library is distributed in the hope that it will be useful, but       
 * WITHOUT ANY WARRANTY; without even the implied warranty of                
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library 
 * General Public License for more details.                                  
 * To obtain a copy of the GNU Library General Public License, write to the  
 * Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.   
 *                                                                           
 * Any permitted reproduction of these routines, in whole or in part, must   
 * bear this legend.                                                         
 * ----------------------------------------------------------------------------
 * ProSystem.c
 * ----------------------------------------------------------------------------
 */
#include "ProSystem.h"
#include "Mixer.h"
#include "Bios.h"
#include "Cartridge.h"
#include "Maria.h"
#include "Memory.h"
#include "Region.h"
#include "Riot.h"
#include "Sally.h"
#include "Tia.h"
#include "Pokey.h"
#include "BupChip.h"
#include "Ym2151.h"
#include "LightGun.h"

#define PRO_SYSTEM_STATE_HEADER "PRO-SYSTEM STATE"

int prosystem_frequency = 60;
int prosystem_scanlines = 263;
int prosystem_cycles = 0;

uint8_t *prosystem_statePtr;

static void prosystem_Map()
{
   cartridge_Map();

   if (bios_enabled)
      bios_Map();

   memory_Map();  /* high priority = overwrite everything */
}

void prosystem_Reset()
{
   if (!cartridge_IsLoaded())
      return;

   region_Reset();

   sally_Reset();
   maria_Reset();
   memory_Reset();
   riot_Reset();

   mixer_Reset();
   tia_Reset();

   cartridge_Reset();
   prosystem_Map();

   prosystem_cycles = 0;
}

void prosystem_SetRate(int rate)
{
   mixer_rate = rate;

   mixer_SetRate();
   tia_SetRate();
   bupchip_SetRate();
}

void prosystem_Run(int cycles)
{
   prosystem_cycles += cycles;

   riot_Run(cycles);
   tia_Run();
   cartridge_Run(cycles);


   prosystem_cycles += sally_SlowCycles();  /* TIA + RIOT slow access */
   cycles += sally_SlowCycles();

   mixer_Run(cycles);
}

void prosystem_ExecuteFrame(const uint8_t* input)
{
   int cycles_total = 0;
   int scanline_start;

   mixer_Frame();
   tia_Frame();
   cartridge_Frame();


   maria_scanline = maria_displayArea.bottom + 1;  /* vblank start */
   scanline_start = maria_scanline;

   riot_SetInput(input);


   while (1)
   {
      int cycles;
      cycles_total -= prosystem_cycles;  /* debug */


      /* 0 = hsync */
      {
         maria_Scanline();
         pokey_Scanline();
	  }


      /* 0-27 = pre-dma */
      while (prosystem_cycles < 28)
      {
         cycles = sally_Run();
         cycles += (!cycles) ? 28 - prosystem_cycles : 0;  /* wsync */

		 prosystem_Run(cycles);
	  }


      /* 28-x = maria render */
      {
         cycles = maria_Run();
         if (cycles + prosystem_cycles > CYCLES_PER_SCANLINE)
            cycles = CYCLES_PER_SCANLINE - prosystem_cycles;

         prosystem_Run(cycles);
      }


      /* x-453 = sally scanline */
      while (prosystem_cycles < CYCLES_PER_SCANLINE)
      {
         cycles = sally_Run();
         cycles += (!cycles) ? CYCLES_PER_SCANLINE - prosystem_cycles : 0;  /* wsync */

<<<<<<< HEAD
         if(memory_ram[WSYNC] && !(cartridge_flags & CARTRIDGE_WSYNC_MASK))
         {
            prosystem_cycles = 456;
            memory_ram[WSYNC] = false;
            break;
         }
      }
      tia_Process(2);
      if(cartridge_pokey_address)
         pokey_Process(2);

      if(cartridge_bupchip)
      {
         bupchipTickScanlines++;
         if(bupchipTickScanlines == scanlinesPerBupchipTick)
         {
            bupchipTickScanlines = 0;
            bupchip_Process(currentBupchipTick);
            currentBupchipTick++;
         }
      }
   }
}

bool prosystem_Save(char *buffer, bool fast_saves)
{
   uint32_t size = 0;
   uint32_t index;

   for(index = 0; index < 16; index++)
      buffer[size + index] = PRO_SYSTEM_STATE_HEADER[index];
   size += 16;

   buffer[size++] = 1;
   for(index = 0; index < 4; index++)
      buffer[size + index] = 0;
   size += 4;

   for(index = 0; index < 32; index++)
      buffer[size + index] = cartridge_digest[index];
   size += 32;

   buffer[size++] = sally_a;
   buffer[size++] = sally_x;
   buffer[size++] = sally_y;
   buffer[size++] = sally_p;
   buffer[size++] = sally_s;
   buffer[size++] = sally_pc.b.l;
   buffer[size++] = sally_pc.b.h;
   buffer[size++] = cartridge_bank;

   for(index = 0; index < 16384; index++)
      buffer[size + index] = memory_ram[index];
   size += 16384;

   if (fast_saves)
   {
      if (bios_enabled)
      {
         save_uint32_to_buffer(buffer, &size, bios_size);
         for(index = MEMORY_SIZE - bios_size; index <= MEMORY_SIZE; index++)
            buffer[size + index] = memory_ram[index];
         size += bios_size;
      }
 
      for(index = 0; index < TIA_BUFFER_SIZE; index++)
         buffer[size + index] = tia_buffer[index];
      size += TIA_BUFFER_SIZE;

      for(index = 0; index < 2; index++)
      {
         buffer[size++] = tia_volume[index];
         buffer[size++] = tia_counterMax[index];
         buffer[size++] = tia_counter[index];
         buffer[size++] = tia_audc[index];
         buffer[size++] = tia_audf[index];
         buffer[size++] = tia_audv[index];

         save_uint32_to_buffer(buffer, &size, tia_poly4Cntr[index]);
         save_uint32_to_buffer(buffer, &size, tia_poly5Cntr[index]);
         save_uint32_to_buffer(buffer, &size, tia_poly9Cntr[index]);
      }
      buffer[size++] = tia_soundCntr;

      save_uint32_to_buffer(buffer, &size, pokey_soundCntr);

      for(index = 0; index < 4; index++)
      {
         buffer[size++] = pokey_audf[index];
         buffer[size++] = pokey_audc[index];
         buffer[size++] = pokey_output[index];
         buffer[size++] = pokey_outVol[index];
      }
      buffer[size++] = pokey_audctl;

      save_uint32_to_buffer(buffer, &size, pokey_poly17Size);
      save_uint32_to_buffer(buffer, &size, pokey_polyAdjust);
      save_uint32_to_buffer(buffer, &size, pokey_poly04Cntr);
      save_uint32_to_buffer(buffer, &size, pokey_poly05Cntr);
      save_uint32_to_buffer(buffer, &size, pokey_poly17Cntr);

      for(index = 0; index < 4; index++)
      {
         save_uint32_to_buffer(buffer, &size, pokey_divideMax[index]);
         save_uint32_to_buffer(buffer, &size, pokey_divideCount[index]);
=======
		 prosystem_Run(cycles);
>>>>>>> c11344e (Create a.yml)
      }


      tia_ScanlineEnd();
      cartridge_ScanlineEnd();

<<<<<<< HEAD
      save_uint32_to_buffer(buffer, &size, pokey_baseMultiplier);

      buffer[size++] = pokey_skctl;
      buffer[size++] = pokey_filterSample[0];
      buffer[size++] = pokey_filterSample[1];
=======

	  cycles_total += prosystem_cycles;  /* debug */
      prosystem_cycles -= CYCLES_PER_SCANLINE;  /* overflow */

      maria_scanline = (maria_scanline + 1) % prosystem_scanlines;
      if (maria_scanline == scanline_start)
         break;
>>>>>>> c11344e (Create a.yml)
   }


   mixer_FrameEnd();


<<<<<<< HEAD
   for(index = 0; index < 16; index++)
   {
      /* File is not a valid ProSystem save state. */
      if(buffer[offset + index] != PRO_SYSTEM_STATE_HEADER[index])
         return false;
   }
   offset += 16;
   buffer[offset++];

   for(index = 0; index < 4; index++);
   offset += 4;

   for(index = 0; index < 32; index++)
      digest[index] = buffer[offset + index];

   offset += 32;

   /* Does not match loaded cartridge digest? */
   if(strcmp(cartridge_digest, digest) != 0)
      return false;

   sally_a      = buffer[offset++];
   sally_x      = buffer[offset++];
   sally_y      = buffer[offset++];
   sally_p      = buffer[offset++];
   sally_s      = buffer[offset++];
   sally_pc.b.l = buffer[offset++];
   sally_pc.b.h = buffer[offset++];

   cartridge_StoreBank(buffer[offset++]);

   for(index = 0; index < 16384; index++)
      memory_ram[index] = buffer[offset + index];
   offset += 16384;

   if (fast_saves)
   {
      if (bios_enabled)
      {
         bios_size = read_uint32_from_buffer(buffer, &offset);
         for(index = MEMORY_SIZE - bios_size; index <= MEMORY_SIZE; index++)
            memory_ram[index] = buffer[offset + index];
         offset += bios_size;
      }

      for(index = 0; index < TIA_BUFFER_SIZE; index++)
         tia_buffer[index] = buffer[offset + index];
      offset += TIA_BUFFER_SIZE;

      for(index = 0; index < 2; index++)
      {
         tia_volume[index] = buffer[offset++];
         tia_counterMax[index] = buffer[offset++];
         tia_counter[index] = buffer[offset++];
         tia_audc[index] = buffer[offset++];
         tia_audf[index] = buffer[offset++];
         tia_audv[index] = buffer[offset++];
         tia_poly4Cntr[index] = read_uint32_from_buffer(buffer, &offset);
         tia_poly5Cntr[index] = read_uint32_from_buffer(buffer, &offset);
         tia_poly9Cntr[index] = read_uint32_from_buffer(buffer, &offset);
      }
      tia_soundCntr = buffer[offset++];

      pokey_soundCntr = read_uint32_from_buffer(buffer, &offset);

      for(index = 0; index < 4; index++)
      {
         pokey_audf[index] = buffer[offset++];
         pokey_audc[index] = buffer[offset++];
         pokey_output[index] = buffer[offset++];
         pokey_outVol[index] = buffer[offset++];
      }
      pokey_audctl = buffer[offset++];

      pokey_poly17Size = read_uint32_from_buffer(buffer, &offset);
      pokey_polyAdjust = read_uint32_from_buffer(buffer, &offset);
      pokey_poly04Cntr = read_uint32_from_buffer(buffer, &offset);
      pokey_poly05Cntr = read_uint32_from_buffer(buffer, &offset);
      pokey_poly17Cntr = read_uint32_from_buffer(buffer, &offset);

      for(index = 0; index < 4; index++)
      {
         pokey_divideMax[index] = read_uint32_from_buffer(buffer, &offset);
         pokey_divideCount[index] = read_uint32_from_buffer(buffer, &offset);
      }

      pokey_sampleMax = read_uint32_from_buffer(buffer, &offset);

      for(index = 0; index < 2; index++)
      {
         pokey_sampleCount[index] = read_uint32_from_buffer(buffer, &offset);
      }

      pokey_baseMultiplier = read_uint32_from_buffer(buffer, &offset);

      pokey_skctl = buffer[offset++];
      pokey_filterSample[0] = buffer[offset++];
      pokey_filterSample[1] = buffer[offset++];
   }

   if(cartridge_type == CARTRIDGE_TYPE_SUPERCART_RAM)
   {
      for(index = 0; index < 16384; index++)
         memory_ram[16384 + index] = buffer[offset + index];
      offset += 16384; 
   }
   else if(cartridge_type == CARTRIDGE_TYPE_SOUPER)
   {
      cartridge_souper_chr_bank[0] = buffer[offset++];
      cartridge_souper_chr_bank[1] = buffer[offset++];
      cartridge_souper_mode = buffer[offset++];
      cartridge_souper_ram_page_bank[0] = buffer[offset++];
      cartridge_souper_ram_page_bank[1] = buffer[offset++];
      for(index = 0; index < sizeof(memory_souper_ram); index++)
         memory_souper_ram[index] = buffer[offset++];
      bupchip_flags = buffer[offset++];
      bupchip_volume = buffer[offset++];
      bupchip_current_song = buffer[offset++];
      bupchip_StateLoaded();
   }

   return true;
=======
#if 0
{
   int ypos, xpos;
   static int x = 120;
   static int y = 0;
   int color = 128;
   int x_start = x - 3;
   int x_end  = x + 3;
   int y_start = y - 3;
   int y_end = y + 3;
   static int test = 0;

   uint8_t *ptr = maria_surface + (maria_visibleArea.top - maria_displayArea.top) * Rect_GetLength(&maria_visibleArea);
   ptr += maria_visibleArea.left - maria_displayArea.left;

   if (x < 0 && y < 0)  /* off-screen */
      return;

   for (ypos = y_start; ypos <= y_end; ypos++)  /* bounding box */
   {
      if (ypos < 0) continue;
      if (ypos >= 224) continue;

      for (xpos = x_start; xpos <= x_end; xpos++)
      {
         if (xpos < 0) continue;
         if (xpos > 320) continue;

         ptr[ypos * 320 + xpos] = 0xff; //((xpos | ypos) & 1) ? color : 0xff;
      }
   }
   //memset(maria_surface, 0xff, 320 * 120);

   if (++test >= 1)
   {
   test = 0;

#if 0
   x++;
   x %= 320;
#elif 1
   y++;
   y %= 224+4;
   if (y < 220) y = 220;
#endif
   }
}
#endif
>>>>>>> c11344e (Create a.yml)
}

void prosystem_Close(bool persistent_data)
{
   bupchip_Release();
   cartridge_Release(persistent_data);
}

bool prosystem_LoadState(const uint8_t *buffer, bool fast_saves)
{
   char digest[33];

   prosystem_statePtr = buffer;

   if (memcmp(prosystem_statePtr, PRO_SYSTEM_STATE_HEADER, sizeof(PRO_SYSTEM_STATE_HEADER)-1) != 0)
      return false;
   prosystem_statePtr += sizeof(PRO_SYSTEM_STATE_HEADER)-1;

   prosystem_ReadStatePtr((uint8_t *) digest, 32);

   maria_LoadState();
   memory_LoadState();
   riot_LoadState();
   sally_LoadState();
   tia_LoadState();

   cartridge_LoadState();  /* expansion modules */

   return true;
}

int prosystem_SaveState(uint8_t *buffer, bool fast_saves)
{
   prosystem_statePtr = buffer;

   memcpy(prosystem_statePtr, PRO_SYSTEM_STATE_HEADER, sizeof(PRO_SYSTEM_STATE_HEADER)-1);
   prosystem_statePtr += sizeof(PRO_SYSTEM_STATE_HEADER)-1;

   prosystem_WriteStatePtr((uint8_t *) cartridge_digest, 32);

   maria_SaveState();
   memory_SaveState();
   riot_SaveState();
   sally_SaveState();
   tia_SaveState();

   cartridge_SaveState();  /* expansion modules */

   return prosystem_statePtr - buffer;
}

uint8_t prosystem_ReadState8(void)
{
   return (*prosystem_statePtr++);
}

uint16_t prosystem_ReadState16(void)
{
   uint16_t val = 0;

   val |= (*prosystem_statePtr++);
   return ((val << 8) | (*prosystem_statePtr++));
}

uint32_t prosystem_ReadState32(void)
{
   uint32_t val = 0;

   val |= (*prosystem_statePtr++);
   val <<= 8;

   val |= (*prosystem_statePtr++);
   val <<= 8;

   val |= (*prosystem_statePtr++);
   return ((val << 8) | (*prosystem_statePtr++));
}

void prosystem_ReadStatePtr(uint8_t *ptr, uint32_t size)
{
   memcpy(ptr, prosystem_statePtr, size);
   prosystem_statePtr += size;
}

void prosystem_WriteState8(uint8_t val)
{
   (*prosystem_statePtr++) = val;
}

void prosystem_WriteState16(uint16_t val)
{
   (*prosystem_statePtr++) = (val >> 8) & 0xff;
   (*prosystem_statePtr++) = (val >> 0) & 0xff;
}

void prosystem_WriteState32(uint32_t val)
{
   (*prosystem_statePtr++) = (val >> 24) & 0xff;
   (*prosystem_statePtr++) = (val >> 16) & 0xff;
   (*prosystem_statePtr++) = (val >> 8) & 0xff;
   (*prosystem_statePtr++) = (val >> 0) & 0xff;
}

void prosystem_WriteStatePtr(uint8_t *ptr, uint32_t size)
{
   memcpy(prosystem_statePtr, ptr, size);
   prosystem_statePtr += size;
}
