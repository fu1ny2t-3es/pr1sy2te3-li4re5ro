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
 * PokeySound is Copyright(c) 1997 by Ron Fries
 *                                                                           
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
 * Pokey.c
 * ----------------------------------------------------------------------------
 */
#include "Pokey.h"
#include "ProSystem.h"
#include "Maria.h"
#include "Cartridge.h"
#include "Mixer.h"
#include "Memory.h"

#define POKEY_NOTPOLY5 0x80
#define POKEY_POLY4 0x40
#define POKEY_PURE 0x20
#define POKEY_VOLUME_ONLY 0x10
#define POKEY_VOLUME_MASK 0x0f

#define POKEY_POLY9 0x80
#define POKEY_CH1_179 0x40
#define POKEY_CH3_179 0x20
#define POKEY_CH1_CH2 0x10
#define POKEY_CH3_CH4 0x08
#define POKEY_CH1_FILTER 0x04
#define POKEY_CH2_FILTER 0x02
#define POKEY_CLOCK_15 0x01

#define POKEY_CLK_64 28
#define POKEY_CLK_15 114

#define POKEY_POLY4_SIZE 0x0F
#define POKEY_POLY5_SIZE 0x1F
#define POKEY_POLY9_SIZE 0x1FF
#define POKEY_POLY17_SIZE 0x1FFFF

#define POKEY_CHANNEL1 0
#define POKEY_CHANNEL2 1
#define POKEY_CHANNEL3 2
#define POKEY_CHANNEL4 3

#define SK_RESET   0x03
#define SK_TWOTONE 0x08

static uint8_t pokey_audf[4];
static uint8_t pokey_audc[4];
static uint8_t pokey_audctl;

static uint8_t pokey_output[4];
static uint8_t pokey_filter[4];

<<<<<<< HEAD
static uint32_t pokey_frequency = 1787520;
static uint32_t pokey_sampleRate = 31440;
uint32_t pokey_soundCntr = 0;
uint8_t pokey_audf[4];
uint8_t pokey_audc[4];
uint8_t pokey_audctl;
uint8_t pokey_output[4];
uint8_t pokey_outVol[4];
static uint8_t pokey_poly04[POKEY_POLY4_SIZE];
static uint8_t pokey_poly05[POKEY_POLY5_SIZE];
static uint8_t pokey_poly09[POKEY_POLY9_SIZE];
static uint8_t pokey_poly17[POKEY_POLY17_SIZE];
uint32_t pokey_poly17Size;
uint32_t pokey_polyAdjust;
uint32_t pokey_poly04Cntr;
uint32_t pokey_poly05Cntr;
uint32_t pokey_poly17Cntr;
uint32_t pokey_divideMax[4];
uint32_t pokey_divideCount[4];
uint32_t pokey_sampleMax;
uint32_t pokey_sampleCount[2];
uint32_t pokey_baseMultiplier;
uint8_t pokey_skctl;
uint8_t pokey_filterSample[2];

/* Generate poly4 or poly5 using XNOR left-shift LFSR (matches MAME poly_init_4_5).
 * Produces maximal-length sequences of period 15 (size=4) or 31 (size=5). */
static void pokey_gen_poly45(uint8_t *poly, int size)
{
   uint32_t mask   = (uint32_t)((1 << size) - 1);
   uint32_t lfsr   = 0;
   int      xorbit = size - 1;
   int      i;
   for(i = 0; i < (int)mask; i++)
   {
      lfsr    = ((lfsr << 1) | (~((lfsr >> 2) ^ (lfsr >> xorbit)) & 1u)) & mask;
      poly[i] = (uint8_t)(lfsr & 1);
   }
}

/* Generate poly9 using XOR right-shift LFSR (matches MAME poly_init_9_17, size=9).
 * Taps at bits 0 and 5; period 511. */
static void pokey_gen_poly9(uint8_t *poly)
{
   uint32_t lfsr = (uint32_t)POKEY_POLY9_SIZE; /* 0x1ff: all 9 bits set */
   uint32_t i, bit;
   for(i = 0; i < POKEY_POLY9_SIZE; i++)
   {
      bit     = ((lfsr >> 0) ^ (lfsr >> 5)) & 1u;
      poly[i] = (uint8_t)(lfsr & 1);
      lfsr    = (lfsr >> 1) | (bit << 8);
   }
}

/* Generate poly17 using MAME's exact poly_init_9_17 algorithm (size=17).
 * Period 131071. Bit 7 is fed by XOR of bits 8 and 13; old bit 0 wraps to bit 16. */
static void pokey_gen_poly17(uint8_t *poly)
{
   uint32_t lfsr = (uint32_t)POKEY_POLY17_SIZE; /* 0x1ffff: all 17 bits set */
   uint32_t i, in8, in;
   for(i = 0; i < POKEY_POLY17_SIZE; i++)
   {
      in8     = ((lfsr >> 8) ^ (lfsr >> 13)) & 1u;
      in      = lfsr & 1u;
      lfsr    = lfsr >> 1;
      lfsr    = (lfsr & 0xff7fu) | (in8 << 7);
      lfsr    = (in << 16) | lfsr;
      poly[i] = (uint8_t)(lfsr & 1);
   }
}
=======
static uint8_t pokey_poly04[POKEY_POLY4_SIZE] = {1,1,0,1,1,1,0,0,0,0,1,0,1,0,0};
static uint8_t pokey_poly05[POKEY_POLY5_SIZE] = {0,0,1,1,0,0,0,1,1,1,1,0,0,1,0,1,0,1,1,0,1,1,1,0,1,0,0,0,0,0,1};
static uint8_t pokey_poly09[POKEY_POLY9_SIZE];
static uint8_t pokey_poly17[POKEY_POLY17_SIZE];

static uint8_t pokey_poly04Cntr;
static uint8_t pokey_poly05Cntr;
static uint16_t pokey_poly09Cntr;
static uint32_t pokey_poly17Cntr;

static uint8_t pokey_divideCount[4];
static uint8_t pokey_borrowCount[4];

static uint8_t pokey_clocks[2];

static uint8_t rand9[POKEY_POLY9_SIZE];
static uint8_t rand17[POKEY_POLY17_SIZE];
static uint32_t r9;
static uint32_t r17;
static uint8_t pokey_skctl;

static uint8_t POT_input[8] = {228, 228, 228, 228, 228, 228, 228, 228};
static int pot_scanline;

static int random_scanline_counter;
static int prev_random_scanline_counter;

static int pokey_cycles;

int16_t pokey_buffer[MAX_SOUND_SAMPLES] = {0};
int pokey_outCount;

static int pokey_lpfCount[4] = {0};
static int pokey_lpfOld[4] = {0};
static int pokey_lpfNew[4] = {0};

/* #define POKEY_LOWPASS 1    /* 1.789 Mhz  = 315 / 88 / 2 */
#define POKEY_LOWPASS 80   /* 22362 */
/* #define POKEY_LOWPASS 95   /* 18839 */
/* #define POKEY_LOWPASS 111  /* 16124 */
/* #define POKEY_LOWPASS 112  /* 15980 */
/* #define POKEY_LOWPASS 120  /* 14914 */
/* #define POKEY_LOWPASS 128  /* 13984 */
/* #define POKEY_LOWPASS 224  /* 7990 */
/* #define POKEY_LOWPASS 1790  /* 1000 */
int pokey_lowpass = POKEY_LOWPASS;

static void rand_init(uint8_t *rng, uint32_t size, uint32_t left, uint32_t right, uint32_t add)
{
   uint32_t mask = (1 << size) - 1;
   uint32_t i, x = 0;

   for (i = 0; i < mask; i++)
   {
      *rng++ = (size == 17) ? x >> 6 : x;  /* bits 6..13 -- bits 0..7 */

      x = ((x << left) + (x >> right) + add) & mask;  /* calculate next bit */
   }
}

static void init_poly09(void)
{
   int mask = (1 << 9) - 1;
   int lfsr = mask;
   int index;

   for (index = 0; index < mask; index++)
   {
      int bin = ((lfsr >> 0) & 1) ^ ((lfsr >> 5) & 1);

      lfsr >>= 1;
      lfsr = (bin << 8) | lfsr;
      pokey_poly09[index] = lfsr & 1;
   }
}

static void init_poly17(void)
{
   int mask = (1 << 17) - 1;
   int lfsr = mask;
   int index;

   for (index = 0; index < mask; index++)
   {
      int bin8 = ((lfsr >> 8) & 1) ^ ((lfsr >> 13) & 1);
      int bin = (lfsr & 1);

      lfsr >>= 1;
      lfsr = (lfsr & 0xFF7F) | (bin8 << 7);
      lfsr = (bin << 16) | lfsr;

      pokey_poly17[index] = lfsr & 1;
   }
}

void pokey_Frame(void)
{
   pokey_outCount = 0;
}

void pokey_SetLowpass(int rate)
{
   pokey_lowpass = rate;
}
>>>>>>> ba98f8e (Create a.yml)

void pokey_Reset(void)
{
   int index;

<<<<<<< HEAD
   pokey_gen_poly45(pokey_poly04, 4);
   pokey_gen_poly45(pokey_poly05, 5);
   pokey_gen_poly9(pokey_poly09);
   pokey_gen_poly17(pokey_poly17);
=======
   init_poly09();
   init_poly17();
>>>>>>> ba98f8e (Create a.yml)

   pokey_poly04Cntr = 0;
   pokey_poly05Cntr = 0;
   pokey_poly09Cntr = 0;
   pokey_poly17Cntr = 0;

   for (index = POKEY_CHANNEL1; index <= POKEY_CHANNEL4; index++)
   {
      pokey_output[index] = 0;
      pokey_audc[index] = 0;
      pokey_audf[index] = 0;
      pokey_filter[index] = (index < 2) ? 1 : 0;
   }

<<<<<<< HEAD
   pokey_audctl = 0;
   pokey_baseMultiplier = POKEY_DIV_64;
   pokey_skctl = 0x03;
   pokey_filterSample[0] = 0;
   pokey_filterSample[1] = 0;
}                           
=======
   for (index = 0; index < 8; index++)
      POT_input[index] = 228;
>>>>>>> ba98f8e (Create a.yml)

   for (index = 0; index < 2; index++)
      pokey_clocks[index] = 0;

   pokey_audctl = 0;

   /* initialize the random arrays */
   rand_init(rand9,   9, 8, 1, 0x00180);
   rand_init(rand17, 17,16, 1, 0x1C000);

   pokey_skctl = SK_RESET;

   r9 = 0;
   r17 = 0;
   random_scanline_counter = 0;
   prev_random_scanline_counter = 0;  

   pokey_cycles = 0;

   memset(&pokey_buffer, 0, sizeof(pokey_buffer));
}

void pokey_Scanline(void)
{
   random_scanline_counter += CYCLES_PER_SCANLINE;

   if (pot_scanline < 228)
      pot_scanline++;
}

uint8_t pokey_Read(uint16_t address) 
{
   uint8_t data = memory_ReadOpenBus(address);

   address &= 0x0F;

   switch (address)
   {
      case POKEY_RANDOM:
      {
         int curr_scanline_counter =  (random_scanline_counter + prosystem_cycles);

         if (pokey_skctl & SK_RESET)
         {
            int adjust = ((curr_scanline_counter - prev_random_scanline_counter) >> 2);

            r9 = ((adjust + r9) % POKEY_POLY9_SIZE);
            r17 = ((adjust + r17) % POKEY_POLY17_SIZE);
         }

         else
         {
            r9 = 0;
            r17 = 0;
         }

         data = ((pokey_audctl & POKEY_POLY9) ? rand9[r9] : rand17[r17]) ^ 0xFF;

         prev_random_scanline_counter = curr_scanline_counter;
         break;
      }
   }

   return data;
}

void pokey_Write(uint16_t address, uint8_t value)
{
   address &= 0x0F;
   //address += 0x450;  /* Pokey 1 */

   switch(address)
   {
      case POKEY_SKCTLS:
         if (pokey_skctl == value) break;  /* no change */
         pokey_skctl = value;

         if (!(value & SK_RESET))  /* reset */
         {
            pokey_clocks[0] = 0;
            pokey_clocks[1] = 0;

            pokey_poly04Cntr = 0;
            pokey_poly05Cntr = 0;
            pokey_poly09Cntr = 0;
            pokey_poly17Cntr = 0;
         }
         break;

      case POKEY_AUDF1:
      case POKEY_AUDF2:
      case POKEY_AUDF3:
      case POKEY_AUDF4:
         pokey_audf[address / 2] = value;
         break;

      case POKEY_AUDC1:
      case POKEY_AUDC2:
      case POKEY_AUDC3:
      case POKEY_AUDC4:
         pokey_audc[address / 2] = value;
         break;

      case POKEY_AUDCTL:
         pokey_audctl = value;
<<<<<<< HEAD
         channelMask = 15;
         if(pokey_audctl & POKEY_POLY9)
            pokey_poly17Size = POKEY_POLY9_SIZE;
         else
            pokey_poly17Size = POKEY_POLY17_SIZE;
         if(pokey_audctl & POKEY_CLOCK_15)
            pokey_baseMultiplier = POKEY_DIV_15;
         else
            pokey_baseMultiplier = POKEY_DIV_64;
         break;

      case POKEY_SKCTL:
         pokey_skctl = value;
         if((pokey_skctl & 0x03) == 0)
         {
            pokey_poly04Cntr = 0;
            pokey_poly05Cntr = 0;
            pokey_poly17Cntr = 0;
         }
         channelMask = 0;
         break;

      default:
         channelMask = 0;
=======
>>>>>>> ba98f8e (Create a.yml)
         break;
   }
}

static void inc_channel(int index, int cycles)
{
<<<<<<< HEAD
   uint8_t* buffer = pokey_buffer + pokey_soundCntr;
#ifdef MSB_FIRST
   uint32_t* sampleCntrPtr = (uint32_t*)((uint8_t*)(&pokey_sampleCount[0]) + 3);
#else
   uint32_t* sampleCntrPtr = (uint32_t*)((uint8_t*)(&pokey_sampleCount[0]) + 1);
#endif
   uint32_t size = length;

   while(length)
   {
      uint8_t channel;
      uint8_t currentValue;
      uint8_t nextEvent = POKEY_SAMPLE;
      uint32_t eventMin = *sampleCntrPtr;

      for(channel = POKEY_CHANNEL1; channel <= POKEY_CHANNEL4; channel++)
      {
         if(pokey_divideCount[channel] <= eventMin)
         {
            eventMin = pokey_divideCount[channel];
            nextEvent = channel;
         }
      }

      for(channel = POKEY_CHANNEL1; channel <= POKEY_CHANNEL4; channel++)
         pokey_divideCount[channel] -= eventMin;

      *sampleCntrPtr -= eventMin;
      pokey_polyAdjust += eventMin;

      if(nextEvent != POKEY_SAMPLE)
      {
         uint8_t filteredOutput;

         /* Advance poly counters by elapsed clocks unless SKCTL resets them */
         if((pokey_skctl & 0x03) != 0)
         {
            pokey_poly04Cntr = (pokey_poly04Cntr + pokey_polyAdjust) % POKEY_POLY4_SIZE;
            pokey_poly05Cntr = (pokey_poly05Cntr + pokey_polyAdjust) % POKEY_POLY5_SIZE;
            pokey_poly17Cntr = (pokey_poly17Cntr + pokey_polyAdjust) % pokey_poly17Size;
         }
         pokey_polyAdjust = 0;
         pokey_divideCount[nextEvent] += pokey_divideMax[nextEvent];

         /* In 16-bit joined mode, the low channel (1 or 3) is the prescaler.
          * It ticks the poly counters above but produces no audio output. */
         if((nextEvent == POKEY_CHANNEL1 && (pokey_audctl & POKEY_CH1_CH2)) ||
            (nextEvent == POKEY_CHANNEL3 && (pokey_audctl & POKEY_CH3_CH4)))
         {
            /* prescaler tick only — skip audio output */
         }
         else
         {
            /* When a filter-clock channel fires, latch the filtered channel's
             * current output and immediately silence it (XOR with itself = 0). */
            if(nextEvent == POKEY_CHANNEL3 && (pokey_audctl & POKEY_CH1_FILTER))
            {
               pokey_filterSample[0] = pokey_output[POKEY_CHANNEL1];
               pokey_outVol[POKEY_CHANNEL1] = 0;
            }
            if(nextEvent == POKEY_CHANNEL4 && (pokey_audctl & POKEY_CH2_FILTER))
            {
               pokey_filterSample[1] = pokey_output[POKEY_CHANNEL2];
               pokey_outVol[POKEY_CHANNEL2] = 0;
            }

            /* Compute this channel's output via poly / pure / volume modes */
            if((pokey_audc[nextEvent] & POKEY_NOTPOLY5) || pokey_poly05[pokey_poly05Cntr])
            {
               if(pokey_audc[nextEvent] & POKEY_PURE)
                  pokey_output[nextEvent] = !pokey_output[nextEvent];
               else if(pokey_audc[nextEvent] & POKEY_POLY4)
                  pokey_output[nextEvent] = pokey_poly04[pokey_poly04Cntr];
               else if(pokey_poly17Size == POKEY_POLY9_SIZE)
                  pokey_output[nextEvent] = pokey_poly09[pokey_poly17Cntr];
               else
                  pokey_output[nextEvent] = pokey_poly17[pokey_poly17Cntr];
            }

            /* Apply high-pass filter for ch1/ch2 if enabled:
             * output is heard only when it differs from the latched filter sample. */
            filteredOutput = pokey_output[nextEvent];
            if(nextEvent == POKEY_CHANNEL1 && (pokey_audctl & POKEY_CH1_FILTER))
               filteredOutput = pokey_output[POKEY_CHANNEL1] ^ pokey_filterSample[0];
            else if(nextEvent == POKEY_CHANNEL2 && (pokey_audctl & POKEY_CH2_FILTER))
               filteredOutput = pokey_output[POKEY_CHANNEL2] ^ pokey_filterSample[1];

            if(filteredOutput)
               pokey_outVol[nextEvent] = pokey_audc[nextEvent] & POKEY_VOLUME_MASK;
            else
               pokey_outVol[nextEvent] = 0;
         }
      }
      else
      {
#ifdef MSB_FIRST
         *(pokey_sampleCount + 1) += pokey_sampleMax;
#else
         *pokey_sampleCount += pokey_sampleMax;
#endif
         currentValue = 0;

         for(channel = POKEY_CHANNEL1; channel <= POKEY_CHANNEL4; channel++)
            currentValue += pokey_outVol[channel];

         currentValue = (currentValue << 2) + 8;
         *buffer++ = currentValue;
         length--;
      }
   }  

   pokey_soundCntr += size;
   if(pokey_soundCntr >= pokey_size)
      pokey_soundCntr = 0;
=======
   if ((--pokey_divideCount[index]) == 0 && pokey_borrowCount[index] == 0)
      pokey_borrowCount[index] = cycles;
>>>>>>> ba98f8e (Create a.yml)
}

static bool check_borrow(int index)
{
   if (pokey_borrowCount[index])
      return (--pokey_borrowCount[index]) == 0;

   return false;
}

static void reset_channel(int index)
{
   pokey_divideCount[index] = pokey_audf[index] + 1;
   pokey_borrowCount[index] = 0;
}

static void process_channel(int index, int mode)
{
   if ((pokey_audc[index] & POKEY_NOTPOLY5) | pokey_poly05[pokey_poly05Cntr])
   {
      switch(mode)
      {
      case 0:
         pokey_output[index] ^= 1;
         break;

      case 1:
         pokey_output[index] = pokey_poly04[pokey_poly04Cntr];
         break;

      case 2:
         pokey_output[index] = pokey_poly09[pokey_poly09Cntr];
         break;

      case 3:
         pokey_output[index] = pokey_poly17[pokey_poly17Cntr];
         break;
      }
   }
}

void pokey_Run(int cycles)
{
   int base_clock = (pokey_audctl & POKEY_CLOCK_15) ? POKEY_CLK_15 : POKEY_CLK_64;
   int joined12 = pokey_audctl & POKEY_CH1_CH2;
   int joined34 = pokey_audctl & POKEY_CH3_CH4;
   int hiclk1 = pokey_audctl & POKEY_CH1_179;
   int hiclk3 = pokey_audctl & POKEY_CH3_179;
   int newvol;
   int index;
   int volonly[4];
   int volmask[4];
   int poly_ticks = 0;
   int volmode[4];


   pokey_cycles += cycles;
   if (pokey_cycles < CYCLES_PER_SCANLINE / 64)  /* 4 = fuzzy, 8 = good, 32 = high, 113 = max */
      return;


   for (index = 0; index < 4; index++)
   {
      volonly[index] = pokey_audc[index] & POKEY_VOLUME_ONLY;
      volmask[index] = pokey_audc[index] & POKEY_VOLUME_MASK;


      if (pokey_lpfCount[index] >= pokey_lowpass)  /* latch new value */
         pokey_lpfOld[index] = pokey_lpfNew[index];

      newvol = ((pokey_output[index] ^ pokey_filter[index]) | volonly[index]) ? volmask[index] : 0;
      pokey_lpfCount[index] = (pokey_lpfNew[index] == newvol) ? pokey_lpfCount[index] : 1;


      if (pokey_audc[index] & POKEY_PURE)
         volmode[index] = 0;
      else if (pokey_audc[index] & POKEY_POLY4)
         volmode[index] = 1;
      else if (pokey_audctl & POKEY_POLY9)
         volmode[index] = 2;
      else
         volmode[index] = 3;
   }

      
   while (pokey_cycles >= 4)  /* Maria 1/4 tick*/
   {
      int clock_triggered = 0;

      pokey_cycles -= 4;

      if (pokey_skctl & SK_RESET)  /* timers running */
      {
         if ((++pokey_clocks[0]) >= POKEY_CLK_64)
         {
            pokey_clocks[0] = 0;

            if (base_clock == POKEY_CLK_64)
               clock_triggered = 1;
         }

         if ((++pokey_clocks[1]) >= POKEY_CLK_15)
         {
            pokey_clocks[1] = 0;

            if (base_clock == POKEY_CLK_15)
               clock_triggered = 1;
         }


         poly_ticks++;


         if (hiclk1 && (joined12 || pokey_audf[0] > 0))  /* ultrasonic speed hack */
            inc_channel(0, joined12 ? 7 : 4);

         if (hiclk3 && (joined34 || pokey_audf[2] > 0))
            inc_channel(2, joined34 ? 7 : 4);


         if (clock_triggered)
         {
            if (!hiclk1)
               inc_channel(0, 1);

            if (!joined12)
               inc_channel(1, 1);

            if (!hiclk3)
               inc_channel(2, 1);

            if (!joined34)
               inc_channel(3, 1);
         }
      }


      if (*((uint32_t *) pokey_borrowCount) == 0)  /* no volume changes */
      {
         pokey_lpfCount[0]++;
         pokey_lpfCount[1]++;
         pokey_lpfCount[2]++;
         pokey_lpfCount[3]++;

         continue;
      }


      pokey_poly04Cntr = (pokey_poly04Cntr + poly_ticks) % POKEY_POLY4_SIZE;
      pokey_poly05Cntr = (pokey_poly05Cntr + poly_ticks) % POKEY_POLY5_SIZE;
      pokey_poly09Cntr = (pokey_poly09Cntr + poly_ticks) % POKEY_POLY9_SIZE;
      pokey_poly17Cntr = (pokey_poly17Cntr + poly_ticks) % POKEY_POLY17_SIZE;
      poly_ticks = 0;


      if (check_borrow(2))  /* ch3 */
      {
         if (joined34)
            inc_channel(3, 1);
         else
            reset_channel(2);

         process_channel(2, volmode[2]);

         pokey_filter[0] = (pokey_audctl & POKEY_CH1_FILTER) ? pokey_output[0] : 1;
      }


      if (check_borrow(3))  /* ch4 */
      {
         if (joined34)
            reset_channel(2);

         reset_channel(3);
         process_channel(3, volmode[3]);

         pokey_filter[1] = (pokey_audctl & POKEY_CH2_FILTER) ? pokey_output[1] : 1;

         /* irq4 */
      }


      if ((pokey_skctl & SK_TWOTONE) && (pokey_borrowCount[1] == 1))  /* ch1 */
         reset_channel(0);
    
      if (check_borrow(0))
      {
         if (joined12)
            inc_channel(1, 1);

         else
            reset_channel(0);

         process_channel(0, volmode[0]);

         /* irq1 */
      }


      if (check_borrow(1))
      {
         if (joined12)
            reset_channel(0);  /* low counter */

         reset_channel(1);
         process_channel(1, volmode[1]);

         /* irq2 */
      }


      for (index = 0; index < 4; index++)
      {
         newvol = 0;
         if (volmask[index])
            newvol = ((pokey_output[index] ^ pokey_filter[index]) | volonly[index]) ? volmask[index] : 0;


         if (pokey_lpfNew[index] == newvol)  /* no frequency change */
            pokey_lpfCount[index]++;

         else
         {
            if (pokey_lpfCount[index] >= pokey_lowpass)  /* latch new value */
               pokey_lpfOld[index] = pokey_lpfNew[index];

            pokey_lpfCount[index] = 1;
            pokey_lpfNew[index] = newvol;
         }
      }
   }


   pokey_poly04Cntr = (pokey_poly04Cntr + poly_ticks) % POKEY_POLY4_SIZE;
   pokey_poly05Cntr = (pokey_poly05Cntr + poly_ticks) % POKEY_POLY5_SIZE;
   pokey_poly09Cntr = (pokey_poly09Cntr + poly_ticks) % POKEY_POLY9_SIZE;
   pokey_poly17Cntr = (pokey_poly17Cntr + poly_ticks) % POKEY_POLY17_SIZE;


#if 0
   for (index = 0; index < 4; index++)
   {
      if (pokey_lpfCount[index] >= pokey_lowpass)  /* latch new value */
         pokey_lpfOld[index] = pokey_lpfNew[index];

      pokey_lpfCount[index] &= 0xfffff;
   }
#endif
}

void pokey_Output(void)
{
   static int max = 0;
   int index;
   int currentValue = 0;
   int active = 2;
   int adjust[3] = { 0x400, 0x300, 0x200 };  /* 10-bit, 9.5-bit, 9-bit expansion */


   if (!cartridge_pokey)
      return;


   for (index = POKEY_CHANNEL1; index <= POKEY_CHANNEL4; index++)  /* 4x 4-bit unsigned */
      currentValue += pokey_lpfOld[index];


   active -= (pokey_audctl & POKEY_CH1_CH2) ? 1 : 0;  /* 16-bit joined timer */
   active -= (pokey_audctl & POKEY_CH3_CH4) ? 1 : 0;

   currentValue *= adjust[active];  /* 15-bit unsigned */

   /* max = (max < currentValue) ? currentValue : max;  /* debug */

   pokey_buffer[pokey_outCount++] = (int16_t) currentValue;
}

void pokey_LoadState(void)
{
   uint8_t index;

   for (index = 0; index < 4; index++)
   {
      pokey_audf[index] = prosystem_ReadState8();
      pokey_audc[index] = prosystem_ReadState8();
      pokey_output[index] = prosystem_ReadState8();
      pokey_filter[index] = prosystem_ReadState8();

      pokey_divideCount[index] = prosystem_ReadState8();
      pokey_borrowCount[index] = prosystem_ReadState8();

      pokey_lpfOld[index] = ((pokey_output[index] ^ pokey_filter[index]) || (pokey_audc[index] & POKEY_VOLUME_ONLY)) ? pokey_audc[index] & POKEY_VOLUME_MASK : 0;
      pokey_lpfNew[index] = pokey_lpfOld[index];
      pokey_lpfCount[index] = 0;
   }

   pokey_audctl = prosystem_ReadState8();

   pokey_poly04Cntr = prosystem_ReadState8();
   pokey_poly05Cntr = prosystem_ReadState8();
   pokey_poly09Cntr = prosystem_ReadState16();
   pokey_poly17Cntr = prosystem_ReadState32();

   pokey_clocks[0] = prosystem_ReadState8();
   pokey_clocks[1] = prosystem_ReadState8();
}

void pokey_SaveState(void)
{
   uint8_t index;

   for (index = 0; index < 4; index++)
   {
      prosystem_WriteState8(pokey_audf[index]);
      prosystem_WriteState8(pokey_audc[index]);
      prosystem_WriteState8(pokey_output[index]);
      prosystem_WriteState8(pokey_filter[index]);

      prosystem_WriteState8(pokey_divideCount[index]);
      prosystem_WriteState8(pokey_borrowCount[index]);
   }

   prosystem_WriteState8(pokey_audctl);

   prosystem_WriteState8(pokey_poly04Cntr);
   prosystem_WriteState8(pokey_poly05Cntr);
   prosystem_WriteState16(pokey_poly09Cntr);
   prosystem_WriteState32(pokey_poly17Cntr);

   prosystem_WriteState8(pokey_clocks[0]);
   prosystem_WriteState8(pokey_clocks[1]);
}
