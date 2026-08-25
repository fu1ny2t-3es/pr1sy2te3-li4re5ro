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
 * Database.c
 * ----------------------------------------------------------------------------
 */
#include "Database.h"
#include "Cartridge.h"

#ifdef WIN32
  #define strcasecmp stricmp  
  #define strncasecmp strnicmp
#endif

#define MAPPER_EXRAM 0x0001  /* 16K ram */
#define MAPPER_EXRAM_A8 0x0002  /* 2K mirror */
#define MAPPER_EXRAM_M2 0x0004  /* halt ram */
#define MAPPER_EXROM 0x0008  /* extra 16K rom */
#define MAPPER_EXFIX 0x0010  /* lastbank-1 rom */
#define MAPPER_EXRAM_X2 0x0020  /* page ram */
#define MAPPER_BANKSET 0x0040  /* halt rom */

#define AUDIO_POKEY_440 0x0001
#define AUDIO_POKEY_450 0x0002
#define AUDIO_POKEY_440_450 0x0004
#define AUDIO_POKEY_800 0x0008
#define AUDIO_POKEY_4000 0x0010
#define AUDIO_ADPCM_420 0x0020
#define AUDIO_COVOX_430 0x0040
#define AUDIO_YM2151_460 0x0080
#define AUDIO_BUPCHIP 0x0100
#define AUDIO_POKEY1_IRQ 0x0200
#define AUDIO_POKEY2_IRQ 0x0400
#define AUDIO_YM2151_IRQ 0x0800

void database_Initialize(void) { }

typedef struct cartridge_db
{
   char digest[256];
   char title[256];
   uint8_t type;
<<<<<<< HEAD
   bool pokey;         /* legacy: true = POKEY at $4000 */
   uint8_t controller1;
   uint8_t controller2;
   uint8_t region;
   uint32_t flags;
   int8_t crossx;
   int8_t crossy;
   uint8_t hblank;
   uint16_t pokey_address; /* explicit POKEY address; 0 falls back to pokey bool */
=======
   uint32_t mapper;
   uint32_t audio;
>>>>>>> c11344e (Create a.yml)
} cartridge_db_t;


/* auto-detect fail */
static const struct cartridge_db db_list[] = 
{
   {
      "8fc3a695eaea3984912d98ed4a543376",
      "Ballblazer",
      CARTRIDGE_TYPE_LINEAR,
      0,
      AUDIO_POKEY_4000
   },
   {
      "b558814d54904ce0582e2f6a801d03af",
      "Ballblazer (Europe)",
      CARTRIDGE_TYPE_LINEAR,
      0,
      AUDIO_POKEY_4000
   },
   {
      "2e8e28f6ad8b9b9267d518d880c73ebb",
      "Commando",
      CARTRIDGE_TYPE_SUPERGAME,
      0,
      AUDIO_POKEY_4000
   },
   {
      "55da6c6c3974d013f517e725aa60f48e",
      "Commando (Europe)",
      CARTRIDGE_TYPE_SUPERGAME,
      0,
      AUDIO_POKEY_4000
   },
   {
      "543484c00ba233736bcaba2da20eeea9",
      "Double Dragon",
      CARTRIDGE_TYPE_ACTIVISION,
   },
   {
      "de2ebafcf0e37aaa9d0e9525a7f4dd62",
      "Double Dragon (Europe)",
      CARTRIDGE_TYPE_ACTIVISION,
   },
   {
      "2251a6a0f3aec84cc0aff66fc9fa91e8",
      "F-18 Hornet",
      CARTRIDGE_TYPE_ABSOLUTE,
   },
   {
      "e7709da8e49d3767301947a0a0b9d2e6",
      "F-18 Hornet (Europe)",
      CARTRIDGE_TYPE_ABSOLUTE,
   },
   {
      "baebc9246c087e893dfa489632157180",
      "Impossible Mission",
      CARTRIDGE_TYPE_SUPERGAME,
      MAPPER_EXRAM
   },
   {
      "80dead01ea2db5045f6f4443faa6fce8",
      "Impossible Mission (Europe)",
      CARTRIDGE_TYPE_SUPERGAME,
      MAPPER_EXRAM
   },
   {
      "045fd12050b7f2b842d5970f2414e912",
      "Jinks",
      CARTRIDGE_TYPE_SUPERGAME,
      MAPPER_EXRAM
   },
   {
      "dfb86f4d06f05ad00cf418f0a59a24f7",
      "Jinks (Europe)",
      CARTRIDGE_TYPE_SUPERGAME,
      MAPPER_EXRAM
   },
   {
      "86546808dc60961cdb1b20e761c50ab1",
      "Plutos",
      CARTRIDGE_TYPE_SUPERGAME,
      MAPPER_EXRAM
   },
   {
      "1745feadabb24e7cefc375904c73fa4c",
      "Possible Mission",
      CARTRIDGE_TYPE_SUPERGAME,
      MAPPER_EXRAM
   },
   {
      "ac03806cef2558fc795a7d5d8dba7bc0",
      "Rampage",
      CARTRIDGE_TYPE_ACTIVISION
   },
   {
<<<<<<< HEAD
      "00688131ee74f6c84048c7fa5c71114a",         /* digest */
      "Bentley Bear's Crystal Quest",             /* title */
      2,                                          /* type: SUPERCART_LARGE */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      0,                                          /* region */
      0,                                          /* flags */
      0,                                          /* crossx */
      0,                                          /* crossy */
      0,                                          /* hblank */
      0x0450                                      /* pokey_address */
   },
   {
      "5a09946e57dbe30408a8f253a28d07db",         /* digest */
      "Centipede",                                /* title */
      0,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      0,                                          /* region */
      0,                                          /* flags */
      0,                                          /* crossx */
      0,                                          /* crossy */
      0                                           /* hblank */
=======
      "8f7eb10ad0bd75474abf0c6c36c08486",
      "Rescue On Fractalus",
      CARTRIDGE_TYPE_LINEAR,
      MAPPER_EXRAM_A8
>>>>>>> c11344e (Create a.yml)
   },
   {
      "592be737ce78a17a572d3bbd527c7a61",
      "Rikki & Vikki (R12)",
      CARTRIDGE_TYPE_SOUPER,
      0,
      AUDIO_BUPCHIP
   },
   {
      "79d3fb83577cd3fd8d1542f58353cfcd",
      "Rikki & Vikki (R13)",
      CARTRIDGE_TYPE_SOUPER,
      0,
      AUDIO_BUPCHIP
   },
   {
      "b3bc889e9cc498636990c5a4d980e85c",
      "Rikki & Vikki (R14)",
      CARTRIDGE_TYPE_SOUPER,
      0,
      AUDIO_BUPCHIP
   },
   {
      "2d643ac548c40e58c99d0fe433ba4ba0",
      "Sirius",
      CARTRIDGE_TYPE_SUPERGAME,
      MAPPER_EXRAM
   },
   {
      "cbb0746192540a13b4c7775c7ce2021f",
      "Summer Games",
      CARTRIDGE_TYPE_SUPERGAME,
      MAPPER_EXRAM
   },
   {
      "8d64763db3100aadc552db5e6868506a",
      "Tower Toppler",
      CARTRIDGE_TYPE_SUPERGAME,
      MAPPER_EXRAM
   },
   {
      "32a37244a9c6cc928dcdf02b45365aa8",
      "Tower Toppler (Europe)",
      CARTRIDGE_TYPE_SUPERGAME,
      MAPPER_EXRAM
   },
   {
      "3799d72f78dda2ee87b0ef8bf7b91186",
      "Winter Games",
      CARTRIDGE_TYPE_SUPERGAME,
      MAPPER_EXRAM
   },
<<<<<<< HEAD
   {
      "63db371d67a98daec547b2abd5e7aa95",         /* digest */
      "Crossbow",                                 /* title */
      2,                                          /* type */
      false,                                      /* pokey */
      3,                                          /* controller 1 */
      3,                                          /* controller 2 */
      1,                                          /* region */
      0,                                          /* flags */
      15,                                         /* crossx */
      5,                                          /* crossy */
      0                                           /* hblank */
   },
   {
      "179b76ff729d4849b8f66a502398acae",         /* digest */
      "Dark Chambers",                            /* title */
      1,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      0,                                          /* region */
      0,                                          /* flags */
      0,                                          /* crossx */
      0,                                          /* crossy */
      0                                           /* hblank */
   },
   {
      "a2b8e2f159642c4b91de82e9a2928494",         /* digest */
      "Dark Chambers",                            /* title */
      1,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      1,                                          /* region */
      0,                                          /* flags */
      0,                                          /* crossx */
      0,                                          /* crossy */
      0                                           /* hblank */
   },
   {
      "95ac811c7d27af0032ba090f28c107bd",         /* digest */
      "Desert Falcon",                            /* title */
      0,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      0,                                          /* region */
      0,                                          /* flags */
      0,                                          /* crossx */
      0,                                          /* crossy */
      0                                           /* hblank */
   },
   {
      "2d5d99b993a885b063f9f22ce5e6523d",         /* digest */
      "Desert Falcon",                            /* title */
      0,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      1,                                          /* region */
      0,                                          /* flags */
      0,                                          /* crossx */
      0,                                          /* crossy */
      0                                           /* hblank */
   },
   {
      "731879ea82fc0ca245e39e036fe293e6",         /* digest */
      "Dig Dug",                                  /* title */
      0,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      0,                                          /* region */
      0,                                          /* flags */
      0,                                          /* crossx */
      0,                                          /* crossy */
      0                                           /* hblank */
   },
   {
      "408dca9fc40e2b5d805f403fa0509436",         /* digest */
      "Dig Dug",                                  /* title */
      0,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      1,                                          /* region */
      0,                                          /* flags */
      0,                                          /* crossx */
      0,                                          /* crossy */
      0                                           /* hblank */
   },
   {
      "5e332fbfc1e0fc74223d2e73271ce650",         /* digest */
      "Donkey Kong Jr",                           /* title */
      0,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      0,                                          /* region */
      0,                                          /* flags */
      0,                                          /* crossx */
      0,                                          /* crossy */
      0                                           /* hblank */
   },
   {
      "4dc5f88243250461bd61053b13777060",         /* digest */
      "Donkey Kong Jr",                           /* title */
      0,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      1,                                          /* region */
      0,                                          /* flags */
      0,                                          /* crossx */
      0,                                          /* crossy */
      0                                           /* hblank */
   },
   {
      "19f1ee292a23636bd57d408b62de79c7",         /* digest */
      "Donkey Kong",                              /* title */
      0,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      0,                                          /* region */
      0,                                          /* flags */
      0,                                          /* crossx */
      0,                                          /* crossy */
      0                                           /* hblank */
   },
   {
      "8e96ef14ce9b5d84bcbc996b66d6d4c7",         /* digest */
      "Donkey Kong",                              /* title */
      0,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      1,                                          /* region */
      0,                                          /* flags */
      0,                                          /* crossx */
      0,                                          /* crossy */
      0                                           /* hblank */
   },
   {
      "543484c00ba233736bcaba2da20eeea9",         /* digest */
      "Double Dragon",                            /* title */
      6,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      0,                                          /* region */
      0,                                          /* flags */
      0,                                          /* crossx */
      0,                                          /* crossy */
      0                                           /* hblank */
   },
   {
      "de2ebafcf0e37aaa9d0e9525a7f4dd62",         /* digest */
      "Double Dragon",                            /* title */
      6,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      1,                                          /* region */
      0,                                          /* flags */
      0,                                          /* crossx */
      0,                                          /* crossy */
      0                                           /* hblank */
   },
   {
      "cb6f27c7bcbe65b60595d6f54bd39348",         /* digest */
      "Double Dragon (Sprite & Color Hack RC4)",  /* title */
      6,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      0,                                          /* region */
      0,                                          /* flags */
      0,                                          /* crossx */
      0,                                          /* crossy */
      0                                           /* hblank */
   },
   {
      "59ca19c4b024fdfcd25b75ac836c7749",         /* digest */
      "Double Dragon (Sprite & Color Hack RC7)",  /* title */
      6,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      0,                                          /* region */
      0,                                          /* flags */
      0,                                          /* crossx */
      0,                                          /* crossy */
      0                                           /* hblank */
   },
   {
      "098b209aac126f2c2edbc982df09cd1b",         /* digest */
      "Double Dragon (Sprite & Color Hack RC7b)", /* title */
      6,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      0,                                          /* region */
      0,                                          /* flags */
      0,                                          /* crossx */
      0,                                          /* crossy */
      0                                           /* hblank */
   },
   {
      "2251a6a0f3aec84cc0aff66fc9fa91e8",         /* digest */
      "F-18 Hornet",                              /* title */
      5,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      0,                                          /* region */
      0,                                          /* flags */
      0,                                          /* crossx */
      0,                                          /* crossy */
      0                                           /* hblank */
   },
   {
      "e7709da8e49d3767301947a0a0b9d2e6",         /* digest */
      "F-18 Hornet",                              /* title */
      5,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      1,                                          /* region */
      0,                                          /* flags */
      0,                                          /* crossx */
      0,                                          /* crossy */
      0                                           /* hblank */
   },
   {
      "d25d5d19188e9f149977c49eb0367cd1",         /* digest */
      "Fatal Run",                                /* title */
      4,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      0,                                          /* region */
      0,                                          /* flags */
      0,                                          /* crossx */
      0,                                          /* crossy */
      0                                           /* hblank */
   },
   {
      "23505651ac2e47f3637152066c3aa62f",         /* digest */
      "Fatal Run",                                /* title */
      4,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      1,                                          /* region */
      0,                                          /* flags */
      0,                                          /* crossx */
      0,                                          /* crossy */
      0                                           /* hblank */
   },
   {
      "07dbbfe612a0a28e283c01545e59f25e",         /* digest */
      "Fight Night",                              /* title */
      4,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      0,                                          /* region */
      0,                                          /* flags */
      0,                                          /* crossx */
      0,                                          /* crossy */
      0                                           /* hblank */
   },
   {
      "e80f24e953563e6b61556737d67d3836",         /* digest */
      "Fight Night",                              /* title */
      4,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      1,                                          /* region */
      0,                                          /* flags */
      0,                                          /* crossx */
      0,                                          /* crossy */
      0                                           /* hblank */
   },
   {
      "cf76b00244105b8e03cdc37677ec1073",         /* digest */
      "Food Fight",                               /* title */
      0,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      0,                                          /* region */
      0,                                          /* flags */
      0,                                          /* crossx */
      0,                                          /* crossy */
      0                                           /* hblank */
   },
   {
      "de0d4f5a9bf1c1bddee3ed2f7ec51209",         /* digest */
      "Food Fight",                               /* title */
      0,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      1,                                          /* region */
      0,                                          /* flags */
      0,                                          /* crossx */
      0,                                          /* crossy */
      0                                           /* hblank */
   },
   {
      "fb8d803b328b2e442548f7799cfa9a4a",         /* digest */
      "Galaga",                                   /* title */
      0,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      0,                                          /* region */
      0,                                          /* flags */
      0,                                          /* crossx */
      0,                                          /* crossy */
      0                                           /* hblank */
   },
   {
      "f5dc7dc8e38072d3d65bd90a660148ce",         /* digest */
      "Galaga",                                   /* title */
      0,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      1,                                          /* region */
      0,                                          /* flags */
      0,                                          /* crossx */
      0,                                          /* crossy */
      0                                           /* hblank */
   },
   {
      "06204dadc975be5e5e37e7cc66f984cf",         /* digest */
      "Gato",                                     /* title */
      0,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      0,                                          /* region */
      0,                                          /* flags */
      0,                                          /* crossx */
      0,                                          /* crossy */
      0                                           /* hblank */
   },
   {
      "fd9e78e201b6baafddfd3e1fbfe6ba31",         /* digest */
      "Hat Trick",                                /* title */
      0,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      0,                                          /* region */
      0,                                          /* flags */
      0,                                          /* crossx */
      0,                                          /* crossy */
      0                                           /* hblank */
   },
   {
      "0baec96787ce17f390e204de1a136e59",         /* digest */
      "Hat Trick",                                /* title */
      0,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      1,                                          /* region */
      0,                                          /* flags */
      0,                                          /* crossx */
      0,                                          /* crossy */
      0                                           /* hblank */
   },
   {
      "c3672482ca93f70eafd9134b936c3feb",         /* digest */
      "Ikari Warriors",                           /* title */
      4,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      0,                                          /* region */
      0,                                          /* flags */
      0,                                          /* crossx */
      0,                                          /* crossy */
      0                                           /* hblank */
   },
   {
      "8c2c2a1ea6e9a928a44c3151ba5c1ce3",         /* digest */
      "Ikari Warriors",                           /* title */
      4,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      1,                                          /* region */
      0,                                          /* flags */
      0,                                          /* crossx */
      0,                                          /* crossy */
      0                                           /* hblank */
   },
   {
      "baebc9246c087e893dfa489632157180",         /* digest */
      "Impossible Mission",                       /* title */
      3,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      0,                                          /* region */
      0,                                          /* flags */
      0,                                          /* crossx */
      0,                                          /* crossy */
      0                                           /* hblank */
   },
   {
      "80dead01ea2db5045f6f4443faa6fce8",         /* digest */
      "Impossible Mission",                       /* title */
      3,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      1,                                          /* region */
      0,                                          /* flags */
      0,                                          /* crossx */
      0,                                          /* crossy */
      0                                           /* hblank */
   },
   {
      "045fd12050b7f2b842d5970f2414e912",         /* digest */
      "Jinks",                                    /* title */
      3,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      0,                                          /* region */
      0,                                          /* flags */
      0,                                          /* crossx */
      0,                                          /* crossy */
      0                                           /* hblank */
   },
   {
      "dfb86f4d06f05ad00cf418f0a59a24f7",         /* digest */
      "Jinks",                                    /* title */
      3,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      1,                                          /* region */
      0,                                          /* flags */
      0,                                          /* crossx */
      0,                                          /* crossy */
      0                                           /* hblank */
   },
   {
      "f18b3b897a25ab3885b43b4bd141b396",         /* digest */
      "Joust",                                    /* title */
      0,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      0,                                          /* region */
      0,                                          /* flags */
      0,                                          /* crossx */
      0,                                          /* crossy */
      0                                           /* hblank */
   },
   {
      "f2dae0264a4b4a73762b9d7177e989f6",         /* digest */
      "Joust",                                    /* title */
      0,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      1,                                          /* region */
      0,                                          /* flags */
      0,                                          /* crossx */
      0,                                          /* crossy */
      0                                           /* hblank */
   },
   {
      "c3a5a8692a423d43d9d28dd5b7d109d9",         /* digest */
      "Karateka",                                 /* title */
      0,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      0,                                          /* region */
      0,                                          /* flags */
      0,                                          /* crossx */
      0,                                          /* crossy */
      0                                           /* hblank */
   },
   {
      "5e0a1e832bbcea6facb832fde23a440a",         /* digest */
      "Karateka",                                 /* title */
      4,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      1,                                          /* region */
      0,                                          /* flags */
      0,                                          /* crossx */
      0,                                          /* crossy */
      0                                           /* hblank */
   },
   {
      "17b3b764d33eae9b5260f01df7bb9d2f",         /* digest */
      "Klax",                                     /* title */
      4,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      0,                                          /* region */
      0,                                          /* flags */
      0,                                          /* crossx */
      0,                                          /* crossy */
      0                                           /* hblank */
   },
   {
      "f57d0af323d4e173fb49ed447f0563d7",         /* digest */
      "Kung Fu Master",                           /* title */
      0,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      0,                                          /* region */
      2,                                          /* flags */
      0,                                          /* crossx */
      0,                                          /* crossy */
      0                                           /* hblank */
   },
   {
      "2931b75811ad03f3ac9330838f3d231b",         /* digest */
      "Kung Fu Master",                           /* title */
      0,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      1,                                          /* region */
      0,                                          /* flags */
      0,                                          /* crossx */
      0,                                          /* crossy */
      0                                           /* hblank */
   },
   {
      "41b1dfe4174ad5434f1ef1aeeac90592",         /* digest */
      "Kung Fu Master (New Sprites v5)",          /* title */
      0,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      0,                                          /* region */
      2,                                          /* flags */
      0,                                          /* crossx */
      0,                                          /* crossy */
      0                                           /* hblank */
   },
   {
      "4fc0b16e3a5c463790a04fb42a1e4c40",         /* digest */
      "Kung Fu Master (RC2)",                     /* title */
      0,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      0,                                          /* region */
      2,                                          /* flags */
      0,                                          /* crossx */
      0,                                          /* crossy */
      0                                           /* hblank */
   },
   {
      "431ca060201ee1f9eb49d44962874049",         /* digest */
      "Mario Bros.",                              /* title */
      0,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      0,                                          /* region */
      0,                                          /* flags */
      0,                                          /* crossx */
      0,                                          /* crossy */
      0                                           /* hblank */
   },
   {
      "d2e861306be78e44248bb71d7475d8a3",         /* digest */
      "Mario Bros.",                              /* title */
      0,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      1,                                          /* region */
      0,                                          /* flags */
      0,                                          /* crossx */
      0,                                          /* crossy */
      0                                           /* hblank */
   },
   {
      "37b5692e33a98115e574185fa8398c22",         /* digest */
      "Mat Mania Challenge",                      /* title */
      4,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      0,                                          /* region */
      0,                                          /* flags */
      0,                                          /* crossx */
      0,                                          /* crossy */
      0                                           /* hblank */
   },
   {
      "6819c37b96063b024898a19dbae2df54",         /* digest */
      "Mat Mania Challenge",                      /* title */
      4,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      1,                                          /* region */
      0,                                          /* flags */
      0,                                          /* crossx */
      0,                                          /* crossy */
      0                                           /* hblank */
   },
   {
      "f2f5e5841e4dda89a2faf8933dc33ea6",         /* digest */
      "Mean 18 Ultimate Golf",                    /* title */
      4,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      0,                                          /* region */
      0,                                          /* flags */
      0,                                          /* crossx */
      0,                                          /* crossy */
      0                                           /* hblank */
   },
   {
      "2e9dbad6c0fa381a6cd1bb9abf98a104",         /* digest */
      "Mean 18 Ultimate Golf",                    /* title */
      4,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      1,                                          /* region */
      0,                                          /* flags */
      0,                                          /* crossx */
      0,                                          /* crossy */
      0                                           /* hblank */
   },
   {
      "bedc30ec43587e0c98fc38c39c1ef9d0",         /* digest */
      "Meltdown",                                 /* title */
      4,                                          /* type */
      false,                                      /* pokey */
      2,                                          /* controller 1 */
      2,                                          /* controller 2 */
      0,                                          /* region */
      0,                                          /* flags */
      0,                                          /* crossx */
      20,                                         /* crossy */
      0                                           /* hblank */
   },
   {
      "c80155d7eec9e3dcb79aa6b83c9ccd1e",         /* digest */
      "Meltdown",                                 /* title */
      4,                                          /* type */
      false,                                      /* pokey */
      2,                                          /* controller 1 */
      2,                                          /* controller 2 */
      1,                                          /* region */
      0,                                          /* flags */
      0,                                          /* crossx */
      10,                                         /* crossy */
      0                                           /* hblank */
   },
   {
      "bc1e905db1008493a9632aa83ab4682b",         /* digest */
      "Midnight Mutants",                         /* title */
      4,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      0,                                          /* region */
      0,                                          /* flags */
      0,                                          /* crossx */
      0,                                          /* crossy */
      20                                          /* hblank */
   },
   {
      "6794ea31570eba0b88a0bf1ead3f3f1b",         /* digest */
      "Midnight Mutants",                         /* title */
      4,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      1,                                          /* region */
      0,                                          /* flags */
      0,                                          /* crossx */
      0,                                          /* crossy */
      20                                          /* hblank */
   },
   {
      "017066f522908081ec3ee624f5e4a8aa",         /* digest */
      "Missing in Action",                        /* title */
      2,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      0,                                          /* region */
      3,                                          /* flags */
      0,                                          /* crossx */
      0,                                          /* crossy */
      0                                           /* hblank */
   },
   {
      "3bc8f554cf86f8132a623cc2201a564b",         /* digest */
      "Motor Psycho",                             /* title */
      4,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      0,                                          /* region */
      0,                                          /* flags */
      0,                                          /* crossx */
      0,                                          /* crossy */
      0                                           /* hblank */
   },
   {
      "5330bfe428a6b601b7e76c2cfc4cd049",         /* digest */
      "Motor Psycho",                             /* title */
      4,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      1,                                          /* region */
      0,                                          /* flags */
      0,                                          /* crossx */
      0,                                          /* crossy */
      0                                           /* hblank */
   },
   {
      "fc0ea52a9fac557251b65ee680d951e5",         /* digest */
      "Ms. Pac-Man",                              /* title */
      0,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      0,                                          /* region */
      0,                                          /* flags */
      0,                                          /* crossx */
      0,                                          /* crossy */
      0                                           /* hblank */
   },
   {
      "56469e8c5ff8983c6cb8dadc64eb0363",         /* digest */
      "Ms. Pac-Man",                              /* title */
      0,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      1,                                          /* region */
      0,                                          /* flags */
      0,                                          /* crossx */
      0,                                          /* crossy */
      0                                           /* hblank */
   },
   {
      "220121f771fc4b98cef97dc040e8d378",         /* digest */
      "Ninja Golf",                               /* title */
      4,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      0,                                          /* region */
      0,                                          /* flags */
      0,                                          /* crossx */
      0,                                          /* crossy */
      0                                           /* hblank */
   },
   {
      "ea0c859aa54fe5eaf4c1f327fab06221",         /* digest */
      "Ninja Golf",                               /* title */
      4,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      1,                                          /* region */
      0,                                          /* flags */
      0,                                          /* crossx */
      0,                                          /* crossy */
      0                                           /* hblank */
   },
   {
      "74569571a208f8b0b1ccfb22d7c914e1",         /* digest */
      "One On One",                               /* title */
      0,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      0,                                          /* region */
      1,                                          /* flags */
      0,                                          /* crossx */
      0,                                          /* crossy */
      0                                           /* hblank */
   },
   {
      "8dba0425f0262e5704581d8757a1a6e3",         /* digest */
      "One On One",                               /* title */
      0,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      1,                                          /* region */
      1,                                          /* flags */
      0,                                          /* crossx */
      0,                                          /* crossy */
      0                                           /* hblank */
   },
   {
      "1a5207870dec6fae9111cb747e20d8e3",         /* digest */
      "Pete Rose Baseball",                       /* title */
      0,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      0,                                          /* region */
      0,                                          /* flags */
      0,                                          /* crossx */
      0,                                          /* crossy */
      0                                           /* hblank */
   },
   {
      "ec206c8db4316eb1ebce9fc960da7d8f",         /* digest */
      "Pit Fighter (Overdump)",                   /* title */
      4,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      0,                                          /* region */
      0,                                          /* flags */
      0,                                          /* crossx */
      0,                                          /* crossy */
      0                                           /* hblank */
   },
   {
      "05f43244465943ce819780a71a5b572a",         /* digest */
      "Pit Fighter",                              /* title */
      4,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      0,                                          /* region */
      0,                                          /* flags */
      0,                                          /* crossx */
      0,                                          /* crossy */
      0                                           /* hblank */
   },
   {
      "33aea1e2b6634a1dec8c7006d9afda22",         /* digest */
      "Planet Smashers",                          /* title */
      4,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      0,                                          /* region */
      0,                                          /* flags */
      0,                                          /* crossx */
      0,                                          /* crossy */
      0                                           /* hblank */
   },
   {
      "2837a8fd49b7fc7ccd70fd45b69c5099",         /* digest */
      "Planet Smashers",                          /* title */
      4,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      1,                                          /* region */
      0,                                          /* flags */
      0,                                          /* crossx */
      0,                                          /* crossy */
      0                                           /* hblank */
   },
   {
      "86546808dc60961cdb1b20e761c50ab1",         /* digest */
      "Plutos",                                   /* title */
      3,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      0,                                          /* region */
      0,                                          /* flags */
      0,                                          /* crossx */
      0,                                          /* crossy */
      50                                          /* hblank */
   },
   {
      "584582bb09ee8122e7fc09dc7d1ed813",         /* digest */
      "Pole Position II",                         /* title */
      0,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      0,                                          /* region */
      0,                                          /* flags */
      0,                                          /* crossx */
      0,                                          /* crossy */
      0                                           /* hblank */
   },
   {
      "865457e0e0f48253b08f77b9e18f93b2",         /* digest */
      "Pole Position II",                         /* title */
      0,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      1,                                          /* region */
      0,                                          /* flags */
      0,                                          /* crossx */
      0,                                          /* crossy */
      0                                           /* hblank */
   },
   {
      "1745feadabb24e7cefc375904c73fa4c",         /* digest */
      "Possible Mission",                         /* title */
      3,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      0,                                          /* region */
      0,                                          /* flags */
      0,                                          /* crossx */
      0,                                          /* crossy */
      0                                           /* hblank */
   },
   {
      "ac03806cef2558fc795a7d5d8dba7bc0",         /* digest */
      "Rampage",                                  /* title */
      6,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      0,                                          /* region */
      0,                                          /* flags */
      0,                                          /* crossx */
      0,                                          /* crossy */
      0                                           /* hblank */
   },
   {
      "442761655bb25ddfe5f7ab16bf591c6f",         /* digest */
      "Rampart",                                  /* title */
      1,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      0,                                          /* region */
      0,                                          /* flags */
      0,                                          /* crossx */
      0,                                          /* crossy */
      0                                           /* hblank */
   },
   {
      "bfad016d6e77eaccec74c0340aded8b9",         /* digest */
      "Realsports Baseball (Overdump)",           /* title */
      1,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      0,                                          /* region */
      0,                                          /* flags */
      0,                                          /* crossx */
      0,                                          /* crossy */
      0                                           /* hblank */
   },
   {
      "383ed9bd1efb9b6cb3388a777678c928",         /* digest */
      "Realsports Baseball",                      /* title */
      1,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      0,                                          /* region */
      0,                                          /* flags */
      0,                                          /* crossx */
      0,                                          /* crossy */
      0                                           /* hblank */
   },
   {
      "8f7eb10ad0bd75474abf0c6c36c08486",         /* digest */
      "Rescue On Fractalus",                      /* title */
      0,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      0,                                          /* region */
      0,                                          /* flags */
      0,                                          /* crossx */
      0,                                          /* crossy */
      0                                           /* hblank */
   },
   {
      "b3bc889e9cc498636990c5a4d980e85c",         /* digest */
      "Rikki & Vikki",                            /* title */
      7,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      0,                                          /* region */
      0,                                          /* flags */
      0,                                          /* crossx */
      0,                                          /* crossy */
      0                                           /* hblank */
   },
   {
      "66ecaafe1b82ae68ffc96267aaf7a4d7",         /* digest */
      "Robotron",                                 /* title */
      0,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      0,                                          /* region */
      0,                                          /* flags */
      0,                                          /* crossx */
      0,                                          /* crossy */
      0                                           /* hblank */
   },
   {
      "980c35ae9625773a450aa7ef51751c04",         /* digest */
      "Scrapyard Dog",                            /* title */
      4,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      0,                                          /* region */
      0,                                          /* flags */
      0,                                          /* crossx */
      0,                                          /* crossy */
      0                                           /* hblank */
   },
   {
      "53db322c201323fe2ca8f074c0a2bf86",         /* digest */
      "Scrapyard Dog",                            /* title */
      4,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      1,                                          /* region */
      0,                                          /* flags */
      0,                                          /* crossx */
      0,                                          /* crossy */
      0                                           /* hblank */
   },
   {
      "b697d9c2d1b9f6cb21041286d1bbfa7f",         /* digest */
      "Sentinel",                                 /* title */
      4,                                          /* type */
      false,                                      /* pokey */
      2,                                          /* controller 1 */
      2,                                          /* controller 2 */
      0,                                          /* region */
      0,                                          /* flags */
      13,                                         /* crossx */
      -13,                                        /* crossy */
      0                                           /* hblank */
   },
   {
      "5469b4de0608f23a5c4f98f331c9e75f",         /* digest */
      "Sentinel",                                 /* title */
      4,                                          /* type */
      false,                                      /* pokey */
      2,                                          /* controller 1 */
      2,                                          /* controller 2 */
      1,                                          /* region */
      0,                                          /* flags */
      20,                                         /* crossx */
      25,                                         /* crossy */
      0                                           /* hblank */
   },
   {
      "2d643ac548c40e58c99d0fe433ba4ba0",         /* digest */
      "Sirius",                                   /* title */
      3,                                          /* type */
      false,                                      /* pokey */
      2,                                          /* controller 1 */
      2,                                          /* controller 2 */
      0,                                          /* region */
      0,                                          /* flags */
      0,                                          /* crossx */
      0,                                          /* crossy */
      0                                           /* hblank */
   },
   {
      "cbb0746192540a13b4c7775c7ce2021f",         /* digest */
      "Summer Games",                             /* title */
      3,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      0,                                          /* region */
      0,                                          /* flags */
      0,                                          /* crossx */
      0,                                          /* crossy */
      0                                           /* hblank */
   },
   {
      "cc18e3b37a507c4217eb6cb1de8c8538",         /* digest */
      "Super Huey UH-IX",                         /* title */
      0,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      0,                                          /* region */
      0,                                          /* flags */
      0,                                          /* crossx */
      0,                                          /* crossy */
      0                                           /* hblank */
   },
   {
      "162f9c953f0657689cc74ab20b40280f",         /* digest */
      "Super Huey UH-IX",                         /* title */
      0,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      1,                                          /* region */
      0,                                          /* flags */
      0,                                          /* crossx */
      0,                                          /* crossy */
      0                                           /* hblank */
   },
   {
      "59b5793bece1c80f77b55d60fb39cb94",         /* digest */
      "Super Skatebordin'",                       /* title */
      0,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      0,                                          /* region */
      0,                                          /* flags */
      0,                                          /* crossx */
      0,                                          /* crossy */
      0                                           /* hblank */
   },
   {
      "95d7c321dce8f57623a9c5b4947bb375",         /* digest */
      "Super Skatebordin'",                       /* title */
      0,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      1,                                          /* region */
      0,                                          /* flags */
      0,                                          /* crossx */
      0,                                          /* crossy */
      0                                           /* hblank */
   },
   {
      "44f862bca77d68b56b32534eda5c198d",         /* digest */
      "Tank Command (Overdump)",                  /* title */
      1,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      0,                                          /* region */
      0,                                          /* flags */
      0,                                          /* crossx */
      0,                                          /* crossy */
      0                                           /* hblank */
   },
   {
      "5c4f752371a523f15e9980fea73b874d",         /* digest */
      "Tank Command",                             /* title */
      1,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      0,                                          /* region */
      0,                                          /* flags */
      0,                                          /* crossx */
      0,                                          /* crossy */
      0                                           /* hblank */
   },
   {
      "5d9ac066e685432825d968f166b042ec",         /* digest */
      "Tiger-Heli",                               /* title */
      2,                                          /* type: SUPERCART_LARGE */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      0,                                          /* region */
      0,                                          /* flags */
      0,                                          /* crossx */
      0,                                          /* crossy */
      0,                                          /* hblank */
      0x0450                                      /* pokey_address */
   },
   {
      "1af475ff6429a160752b592f0f92b287",         /* digest */
      "Title Match Pro Wrestling",                /* title */
      0,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      0,                                          /* region */
      0,                                          /* flags */
      0,                                          /* crossx */
      0,                                          /* crossy */
      0                                           /* hblank */
   },
   {
      "3bb9c8d9adc912dd7f8471c97445cd8d",         /* digest */
      "Title Match Pro Wrestling",                /* title */
      0,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      1,                                          /* region */
      0,                                          /* flags */
      0,                                          /* crossx */
      0,                                          /* crossy */
      0                                           /* hblank */
   },
   {
      "c3903ab01a51222a52197dbfe6538ecf",         /* digest */
      "Tomcat F-14 Simulator",                    /* title */
      0,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      0,                                          /* region */
      0,                                          /* flags */
      0,                                          /* crossx */
      0,                                          /* crossy */
      0                                           /* hblank */
   },
   {
      "682338364243b023ecc9d24f0abfc9a7",         /* digest */
      "Tomcat F-14 Simulator",                    /* title */
      0,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      1,                                          /* region */
      0,                                          /* flags */
      0,                                          /* crossx */
      0,                                          /* crossy */
      0                                           /* hblank */
   },
   {
      "208ef955fa90a29815eb097bce89bace",         /* digest */
      "Touchdown Football",                       /* title */
      4,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      0,                                          /* region */
      0,                                          /* flags */
      0,                                          /* crossx */
      0,                                          /* crossy */
      0                                           /* hblank */
   },
   {
      "d12e665347f354048b9d13092f7868c9",         /* digest */
      "Tower Toppler (Overdump)",                 /* title */
      3,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      0,                                          /* region */
      0,                                          /* flags */
      0,                                          /* crossx */
      0,                                          /* crossy */
      0                                           /* hblank */
   },
   {
      "8d64763db3100aadc552db5e6868506a",         /* digest */
      "Tower Toppler",                            /* title */
      3,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      0,                                          /* region */
      0,                                          /* flags */
      0,                                          /* crossx */
      0,                                          /* crossy */
      0                                           /* hblank */
   },
   {
      "32a37244a9c6cc928dcdf02b45365aa8",         /* digest */
      "Tower Toppler",                            /* title */
      3,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      1,                                          /* region */
      0,                                          /* flags */
      0,                                          /* crossx */
      0,                                          /* crossy */
      0                                           /* hblank */
   },
   {
      "acf63758ecf3f3dd03e9d654ae6b69b7",         /* digest */
      "Water Ski (Overdump)",                     /* title */
      1,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      0,                                          /* region */
      0,                                          /* flags */
      0,                                          /* crossx */
      0,                                          /* crossy */
      0                                           /* hblank */
   },
   {
      "427cb05d0a1abb068998e2760d77f4fb",         /* digest */
      "Water Ski",                                /* title */
      1,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      0,                                          /* region */
      0,                                          /* flags */
      0,                                          /* crossx */
      0,                                          /* crossy */
      0                                           /* hblank */
   },
   {
      "3799d72f78dda2ee87b0ef8bf7b91186",         /* digest */
      "Winter Games",                             /* title */
      3,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      0,                                          /* region */
      0,                                          /* flags */
      0,                                          /* crossx */
      0,                                          /* crossy */
      0                                           /* hblank */
   },
   {
      "05fb699db9eef564e2fe45c568746dbc",         /* digest */
      "Xenophobe",                                /* title */
      4,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      0,                                          /* region */
      0,                                          /* flags */
      0,                                          /* crossx */
      0,                                          /* crossy */
      0                                           /* hblank */
   },
   {
      "70937c3184f0be33d06f7f4382ca54de",         /* digest */
      "Xenophobe",                                /* title */
      4,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      1,                                          /* region */
      0,                                          /* flags */
      0,                                          /* crossx */
      0,                                          /* crossy */
      0                                           /* hblank */
   },
   {
      "d7dc17379aa25e5ae3c14b9e780c6f6d",         /* digest */
      "Xevious",                                  /* title */
      0,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      0,                                          /* region */
      0,                                          /* flags */
      0,                                          /* crossx */
      0,                                          /* crossy */
      0                                           /* hblank */
   },
   {
      "b1a9f196ce5f47ca8caf8fa7bc4ca46c",         /* digest */
      "Xevious",                                  /* title */
      0,                                          /* type */
      false,                                      /* pokey */
      1,                                          /* controller 1 */
      1,                                          /* controller 2 */
      1,                                          /* region */
      0,                                          /* flags */
      0,                                          /* crossx */
      0,                                          /* crossy */
      0                                           /* hblank */
   }
=======
>>>>>>> c11344e (Create a.yml)
};

static void fixup(int index)
{
   uint32_t flags;


   cartridge_type = db_list[index].type;


   flags = db_list[index].mapper;

   if (flags & MAPPER_BANKSET)
      cartridge_bankset = 1;

   if (flags & MAPPER_EXFIX)
      cartridge_exfix = 1;

   if (flags & MAPPER_EXROM)
      cartridge_exrom = 1;

   if (flags & MAPPER_EXRAM)
      cartridge_exram = 1;

   if (flags & MAPPER_EXRAM_M2)
      cartridge_exram_m2 = 1;

   if (flags & MAPPER_EXRAM_A8)
      cartridge_exram_a8 = 1;

   if (flags & MAPPER_EXRAM_X2)
      cartridge_exram_x2 = 1;



   flags = db_list[index].audio;

   if (flags & AUDIO_POKEY_440)
      cartridge_pokey = POKEY_AT_440;

   if (flags & AUDIO_POKEY_450)
      cartridge_pokey = POKEY_AT_450;

   if (flags & AUDIO_POKEY_440_450)
      cartridge_pokey = POKEY_AT_440_450;

   if (flags & AUDIO_POKEY_800)
      cartridge_pokey = POKEY_AT_800;

   if (flags & AUDIO_POKEY_4000)
      cartridge_pokey = POKEY_AT_4000;

   if (flags & AUDIO_YM2151_460)
      cartridge_ym2151 = YM2151_AT_460;

   if (flags & AUDIO_BUPCHIP)
      cartridge_bupchip = 1;


   if (strstr(db_list[index].title, "(PAL)") ||
	   strstr(db_list[index].title, "(Europe)"))
      cartridge_region = 1;
}

static void detect(void)
{
   if (strstr(cartridge_title, "(PAL)") ||
	   strstr(cartridge_title, "(Europe)"))
      cartridge_region = 1;


   if (cartridge_size < 0xe000)
   {
      cartridge_type = CARTRIDGE_TYPE_LINEAR;
      return;
   }


   cartridge_type = CARTRIDGE_TYPE_SUPERGAME;

   cartridge_exfix = 1;  /* common default */

   if (cartridge_size == 0x20000 + 0x4000)
      cartridge_exrom = 1;

   /* ignore exram map as causes problems enough times */
}

void database_Load(const char *digest)
{
   unsigned i;
   size_t len = sizeof(db_list) / sizeof(db_list[0]);

   for (i = 0; i < len; i++)
   {
      if (!strcasecmp(db_list[i].digest, digest))
      {
<<<<<<< HEAD
         cartridge_type          = db_list[i].type;
         if(db_list[i].pokey_address)
            cartridge_pokey_address = db_list[i].pokey_address;
         else
            cartridge_pokey_address = db_list[i].pokey ? 0x4000 : 0;
         cartridge_controller[0] = db_list[i].controller1;
         cartridge_controller[1] = db_list[i].controller2;
         cartridge_region        = db_list[i].region;
         cartridge_flags         = db_list[i].flags;
=======
         fixup(i);
>>>>>>> c11344e (Create a.yml)
         return;
      }
   }

   detect();
}
