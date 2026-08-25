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
 * Cartridge.c
 * ----------------------------------------------------------------------------
 */
#include "ProSystem.h"
#include "Cartridge.h"
#include "Memory.h"
#include "Hash.h"
#include "Pokey.h"
#include "BupChip.h"
#include "Mapper.h"
#include "Database.h"
#include "Ym2151.h"
#include "LightGun.h"
#include <streams/file_stream.h>

char cartridge_digest[33];
char cartridge_title[256];

uint8_t cartridge_type;
uint8_t cartridge_region;
<<<<<<< HEAD
uint16_t cartridge_pokey_address;
=======
>>>>>>> c11344e (Create a.yml)
uint8_t cartridge_controller[2];
uint8_t cartridge_bank;
uint8_t cartridge_flags;

uint8_t cartridge_pokey;
uint8_t cartridge_ym2151;
uint8_t cartridge_bupchip;
uint8_t cartridge_xm;
uint8_t cartridge_atarivox;

uint8_t cartridge_bankset;
uint8_t cartridge_exrom;
uint8_t cartridge_exfix;
uint8_t cartridge_exram;
uint8_t cartridge_exram_m2;
uint8_t cartridge_exram_x2;
uint8_t cartridge_exram_a8;

uint8_t* cartridge_buffer = NULL;
uint32_t cartridge_size = 0;

char* cartridge_GetNextNonemptyLine(const char **stream, size_t* size)
{
   while(*size != 0)
   {
      char *line_buffer;
      const char *end;
      const char* line = *stream;
      while(*size > 0 && **stream != '\r' && **stream != '\n')
      {
         (*stream)++;
         (*size)--;
      }

      /* Skip CR/LF. */
      end = *stream;
      while(*size > 0 && (**stream == '\r' || **stream == '\n'))
      {
         (*stream)++;
         (*size)--;
      }

      if(line == end || line[0] == '\n' || line[0] == '\r')
         continue;

      line_buffer = (char*)malloc(end - line + 1);
      memcpy(line_buffer, line, end - line);
      line_buffer[end - line] = '\0';
      return line_buffer;
   }

   return NULL;
}

bool cartridge_ReadFile(uint8_t** outData, size_t* outSize, const char* subpath, const char* relativeTo)
{
   int64_t len    = 0;
   size_t pathLen = strlen(subpath) + strlen(relativeTo) + 1;
   char* path     = (char*)malloc(pathLen + 1);
#ifdef _WIN32
   char pathSeparator = '\\';
#else
   char pathSeparator = '/';
#endif
   sprintf(path, "%s%c%s", relativeTo, pathSeparator, subpath);

   filestream_read_file(path, (void**)outData, &len);
   *outSize = (size_t)len;
   return len > 0;
}

static bool cartridge_HasHeader(const uint8_t* header)
{
   const char HEADER_ID[ ] = {"ATARI7800"};

   return memcmp(HEADER_ID, header+1, 9) == 0;
}

/* ----------------------------------------------------------------------------
 * Header for CC2 hack
 * ----------------------------------------------------------------------------
 */
static bool cartridge_CC2(const uint8_t* header)
{
   unsigned index;
   const char HEADER_ID[ ] = {">>"};

   for (index = 0; index < 2; index++)
   {
      if(HEADER_ID[index] != header[index+1])
         return false;
   }
   return true;
}

static void cartridge_ReadHeader(const uint8_t* header)
{
   uint16_t cardtype;


   cartridge_size  = header[49] << 24;
   cartridge_size |= header[50] << 16;
   cartridge_size |= header[51] << 8;
   cartridge_size |= header[52];


   cardtype = (header[53] << 8) | header[54];

   if (cardtype & 0x0001)  /* pokey 1 */
      cartridge_pokey = POKEY_AT_4000;

   if (cardtype & 0x0002)  /* paging rom */
      cartridge_type = CARTRIDGE_TYPE_SUPERGAME;

   if (cardtype & 0x0004)  /* 16 KB ram */
      cartridge_exram = 1;

   if (cardtype & 0x0008)  /* 16 KB rom */
      cartridge_exrom = 1;

   if (cardtype & 0x0010)  /* last bank - 1 */
      cartridge_exfix = 1;

   if (cardtype & 0x0020)  /* paging ram */
      cartridge_exram_x2 = 1;

   if (cardtype & 0x0040)  /* pokey 1 */
   {
<<<<<<< HEAD
      /* Bit-priority parse of the A78 cart-type bitmask (byte 54).
       * Highest-priority bit wins. Backwards-compatible with the previous
       * value-equality form (each former bucket's dominant bit picks the same
       * branch) and additionally handles real-world carts where bit 4
       * (Bank 6 @ $4000) co-occurs with bit 1 (SuperGame), e.g. value 0x12
       * used by Pac-Man Collection 40th Anniversary Edition. */
      uint8_t b = header[54];
      if(cartridge_size > 131072)
         cartridge_type = CARTRIDGE_TYPE_SUPERCART_LARGE;
      else if(b & 0x10)             /* bit 4: bank 6 fixed at $4000 */
         cartridge_type = CARTRIDGE_TYPE_SUPERCART_ROM;
      else if(b & 0x08)             /* bit 3: ROM at $4000 */
         cartridge_type = CARTRIDGE_TYPE_SUPERCART_ROM;
      else if(b & 0x04)             /* bit 2: RAM at $4000 */
         cartridge_type = CARTRIDGE_TYPE_SUPERCART_RAM;
      else if(b & 0x02)             /* bit 1: SuperGame bankswitched */
         cartridge_type = CARTRIDGE_TYPE_SUPERCART;
      else
         cartridge_type = CARTRIDGE_TYPE_NORMAL;
   }
   else
   {
      if(header[53] == 1)
         cartridge_type = CARTRIDGE_TYPE_ABSOLUTE;
      else if(header[53] == 2)
         cartridge_type = CARTRIDGE_TYPE_ACTIVISION;
      else if(header[53] == 16)
         cartridge_type = CARTRIDGE_TYPE_SOUPER;
      else
         cartridge_type = CARTRIDGE_TYPE_NORMAL;
   }

   /* Detect POKEY address from A78 header bitmask.
    * header[53] = high byte (bits 8-15), header[54] = low byte (bits 0-7).
    * Priority: $800 > $440 > $450 > $4000 (highest-numbered bits checked first). */
   if(header[53] & 0x80)       /* bit 15 = POKEY at $0800 */
      cartridge_pokey_address = 0x0800;
   else if(header[53] & 0x04)  /* bit 10 = POKEY at $0440 */
      cartridge_pokey_address = 0x0440;
   else if(header[54] & 0x40)  /* bit  6 = POKEY at $0450 */
      cartridge_pokey_address = 0x0450;
   else if(header[54] & 0x01)  /* bit  0 = POKEY at $4000 */
      cartridge_pokey_address = 0x4000;
   else
      cartridge_pokey_address = 0;
=======
      cartridge_pokey = POKEY_AT_450;
      cartridge_xm = 1;
   }

   if (cardtype & 0x0080)  /* 2KB mirror ram */
      cartridge_exram_a8 = 1;

   if (cardtype & 0x0100)
      cartridge_type = CARTRIDGE_TYPE_ACTIVISION;

   if (cardtype & 0x0200)
      cartridge_type = CARTRIDGE_TYPE_ABSOLUTE;

   if (cardtype & 0x0400)  /* pokey 2 */
   {
      cartridge_pokey = POKEY_AT_440;
      cartridge_xm = 1;
   }

   if (cardtype & 0x0800)
   {
      cartridge_ym2151 = 1;
      cartridge_xm = 1;
   }

   if (cardtype & 0x1000)
      cartridge_type = CARTRIDGE_TYPE_SOUPER;

   if (cardtype & 0x2000)  /* halt rom */
      cartridge_bankset = 1;

   if (cardtype & 0x4000)  /* halt ram */
      cartridge_exram_m2 = 1;

   if (cardtype & 0x8000)  /* pokey 1 */
      cartridge_pokey = POKEY_AT_800;


>>>>>>> c11344e (Create a.yml)
   cartridge_controller[0] = header[55];
   cartridge_controller[1] = header[56];
   cartridge_region        = header[57] & 1;
   cartridge_flags         = 0;
   cartridge_bupchip       = cartridge_type == CARTRIDGE_TYPE_SOUPER;

   if (header[0] < 3)  /* version 1-2 */
   {
      if (cartridge_type == CARTRIDGE_TYPE_SUPERGAME && !cartridge_exram)  /* v3-4 assumptions */
         cartridge_exfix = 1;
   }
}

uint8_t cartridge_LoadROM(uint32_t address)
{
   if(address >= cartridge_size)
      return 0;
   return cartridge_buffer[address];
}

bool cartridge_LoadFromCDF(const char* data, size_t size,
		const char *workingDir)
{
   static const char *cartridgeTypes[ ] = {
      "EMPTY",
      "SUPER",
      NULL,
      NULL,
      NULL,
      "ABSOLUTE",
      "ACTIVISION",
      "SOUPER",
   };
   int i;
   char* line;
   size_t cart_size;
   if((line = cartridge_GetNextNonemptyLine(&data, &size)) == NULL)
      return false;
   if (strcmp(line, "ProSystem") != 0)
      return false;
   free(line);

   if((line = cartridge_GetNextNonemptyLine(&data, &size)) == NULL)
      return false;
   for(i = 0; i < sizeof(cartridgeTypes) / sizeof(cartridgeTypes[0]); i++)
   {
      if(cartridgeTypes[i] != NULL && strcmp(line, cartridgeTypes[i]) == 0)
      {
         cartridge_type = i;
         break;
      }
   }
   free(line);

   if((line = cartridge_GetNextNonemptyLine(&data, &size)) == NULL)
      return false;
   /* Just ignore the cartridge title in `libretro`. */
   free(line);

   /* Read binary file. */
   if((line = cartridge_GetNextNonemptyLine(&data, &size)) == NULL)
      return false;
   if(!cartridge_ReadFile(&cartridge_buffer, &cart_size, line, workingDir))
      return false;
   free(line);

   cartridge_size = (uint32_t)cart_size;
   hash_Compute(cartridge_digest, cartridge_buffer, cart_size);

   cartridge_bupchip = false;
   if((line = cartridge_GetNextNonemptyLine(&data, &size)))
   {
      cartridge_bupchip = strcmp(line, "CORETONE") == 0;
      free(line);
   }

   if(cartridge_bupchip)
   {
      if(!bupchip_InitFromCDF(&data, &size, workingDir))
      {
         free(cartridge_buffer);
         return false;
      }
   }

   if (!cartridge_Load(false, cartridge_buffer, cart_size))
   {
      free(cartridge_buffer);
      return false;
   }

   return true;
}

bool cartridge_Load(bool persistent_data, const uint8_t* data, uint32_t size)
{
   uint32_t offset     = 0;
   uint8_t header[128] = {0};

   /* Cartridge data is invalid. */
   if(size <= 128)
      return false;

   memcpy(header, data, 128);

   /* Prosystem doesn't support CC2 hacks. */
   if (cartridge_CC2(header))
      return false;


   cartridge_type = 0;
   cartridge_region = 0;

   cartridge_bankset = 0;
   cartridge_exrom = 0;
   cartridge_exfix = 0;
   cartridge_exram = 0;
   cartridge_exram_m2 = 0;
   cartridge_exram_x2 = 0;
   cartridge_exram_a8 = 0;

   cartridge_pokey = POKEY_NONE;
   cartridge_ym2151 = 0;
   cartridge_bupchip = 0;
   cartridge_xm = 0;

   if(cartridge_HasHeader(header))
   {
      cartridge_ReadHeader(header);
      size   -= 128;
      offset  = 128;
   }

   cartridge_size      = size;
   if (persistent_data)
      cartridge_buffer = (uint8_t*)data + offset;
   else
   {
      cartridge_buffer = (uint8_t*)malloc(cartridge_size * sizeof(uint8_t));

      memcpy(cartridge_buffer, data + offset, cartridge_size);
   }

   hash_Compute(cartridge_digest, cartridge_buffer, cartridge_size);

   if (offset == 0)
      database_Load(cartridge_digest);

   return true;
}

void cartridge_Reset(void)
{
   mapper_Reset();


   if (cartridge_pokey)
      pokey_Reset();

   if (cartridge_bupchip)
      bupchip_Reset();

   if (cartridge_ym2151)
      ym2151_Reset();

   if (lightgun_enabled)
      lightgun_Reset();
}

void cartridge_Frame(void)
{
   if (cartridge_pokey)
      pokey_Frame();

   if (cartridge_bupchip)
      bupchip_Frame();

   if (cartridge_ym2151)
      ym2151_Frame();
}

void cartridge_MapBios(void)
{
   mapper_MapBios();
}

void cartridge_Map(void)
{
   mapper_Map();
}

uint8_t cartridge_Read(uint16_t address)
{
   return mapper_Read(address);
}

void cartridge_Write(uint16_t address, uint8_t data)
{
<<<<<<< HEAD
   switch(cartridge_type)
   {
      case CARTRIDGE_TYPE_SUPERCART:
      case CARTRIDGE_TYPE_SUPERCART_RAM:
      case CARTRIDGE_TYPE_SUPERCART_ROM:
         if(address >= 32768 && address < 49152 && data < 9)
            cartridge_StoreBank(data);
         break;
      case CARTRIDGE_TYPE_SUPERCART_LARGE:
         if(address >= 32768 && address < 49152 && data < 9)
            cartridge_StoreBank(data + 1);
         break;
      case CARTRIDGE_TYPE_ABSOLUTE:
         if(address == 32768 && (data == 1 || data == 2))
            cartridge_StoreBank(data - 1);
         break;
      case CARTRIDGE_TYPE_ACTIVISION:
         if(address >= 65408)
            cartridge_StoreBank(address & 7);
         break;
    case CARTRIDGE_TYPE_SOUPER:
         if(address >= 0x4000 && address < 0x8000) {
            memory_souper_ram[memory_souper_GetRamAddress(address)] = data;
            break;
         }

         switch(address)
         {
            case CARTRIDGE_SOUPER_BANK_SEL:
               cartridge_StoreBank(data & 31);
               break;
            case CARTRIDGE_SOUPER_CHR_A_SEL:
               cartridge_souper_StoreChrBank(0, data);
               break;
            case CARTRIDGE_SOUPER_CHR_B_SEL:
               cartridge_souper_StoreChrBank(1, data);
               break;
            case CARTRIDGE_SOUPER_MODE_SEL:
               cartridge_souper_SetMode(data);
               break;
            case CARTRIDGE_SOUPER_EXRAM_V_SEL:
               cartridge_souper_SetRamPageBank(0, data);
               break;
            case CARTRIDGE_SOUPER_EXRAM_D_SEL:
               cartridge_souper_SetRamPageBank(1, data);
               break;
            case CARTRIDGE_SOUPER_AUDIO_CMD:
               bupchip_ProcessAudioCommand(data);
               break;
         }
         break;
   }

   /* Route writes to POKEY at $4000 (cartridge ROM space).
    * Non-$4000 POKEY addresses live in RAM space and are intercepted in memory_Write. */
   if(cartridge_pokey_address == 0x4000 &&
      address >= 0x4000 && address < 0x400a)
   {
      pokey_SetRegister((uint16_t)(POKEY_AUDF1 + (address - 0x4000)), data);
   }
}

void cartridge_StoreBank(uint8_t bank)
{
   switch(cartridge_type)
   {
      case CARTRIDGE_TYPE_SUPERCART:
         cartridge_WriteBank(32768, bank);
         break;
      case CARTRIDGE_TYPE_SUPERCART_RAM:
         cartridge_WriteBank(32768, bank);
         break;
      case CARTRIDGE_TYPE_SUPERCART_ROM:
         cartridge_WriteBank(32768, bank);
         break;
      case CARTRIDGE_TYPE_SUPERCART_LARGE:
         cartridge_WriteBank(32768, bank);        
         break;
      case CARTRIDGE_TYPE_ABSOLUTE:
         cartridge_WriteBank(16384, bank);
         break;
      case CARTRIDGE_TYPE_ACTIVISION:
         cartridge_WriteBank(40960, bank);
         break;
      case CARTRIDGE_TYPE_SOUPER:
         cartridge_WriteBank(32768, bank);
         break;
   }  
=======
   mapper_Write(address, data);
>>>>>>> c11344e (Create a.yml)
}

bool cartridge_IsLoaded(void)
{
  return (cartridge_buffer != NULL) ? true: false;
}

void cartridge_Release(bool persistent_data)
{
   if (!persistent_data)
   {
      if (cartridge_buffer)
         free(cartridge_buffer);
   }
   cartridge_buffer = NULL;
   cartridge_size   = 0;
}

void cartridge_Run(int cycles)
{
   if (cartridge_pokey)
      pokey_Run(cycles);

   if (cartridge_ym2151)
      ym2151_Run(cycles);
}

void cartridge_ScanlineEnd(void)
{
   if (cartridge_bupchip)
      bupchip_ScanlineEnd();
}

void cartridge_LoadState(void)
{
   uint8_t flags;
   uint32_t temp;

   mapper_LoadState();

   while (1)
   {
      flags = prosystem_ReadState8();
      if (flags == 255)
         break;

      switch (flags)
	  {
	  case 0:
         temp = prosystem_ReadState32();
         prosystem_ReadStatePtr(memory_exram, temp);
         break;

	  case 1:
         pokey_LoadState();
         break;

	  case 2:
         bupchip_LoadState();
         break;

	  case 3:
         /* ym2141 */
         break;

	  case 4:
         /* covox */
         break;

	  case 5:
         /* atarivox */
         break;
	  }
   }
}

void cartridge_SaveState(void)
{
   mapper_SaveState();

   if (memory_exram_size)
   {
      prosystem_WriteState8(0);

      prosystem_WriteState32(memory_exram_size);
      prosystem_WriteStatePtr(memory_exram, memory_exram_size);
   }

   if (cartridge_pokey)
   {
      prosystem_WriteState8(1);

      pokey_SaveState();
   }

   if (cartridge_bupchip)
   {
      prosystem_WriteState8(2);

      bupchip_SaveState();
   }

   prosystem_WriteState8(255);
}
