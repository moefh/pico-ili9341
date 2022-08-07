/* game_data.c */

#include "game_data.h"
#include "data/font6x8.h"
#include "data/spr_loserboy.h"
#include "data/spr_castle3.h"


const struct IMAGE game_images[] = {
#define ADD_IMAGE(name) { img_##name##_width, img_##name##_height, img_##name##_stride, (void*)img_##name##_data }
  ADD_IMAGE(castle3),
  ADD_IMAGE(loserboy),
#undef ADD_IMAGE
};

const struct FONT font6x8 = {
  .w = font6x8_width,
  .h = font6x8_height,
  .first_char = font6x8_first_char,
  .num_chars = font6x8_num_chars,
  .data = font6x8_data,
};

static struct GAME_MAP_TILE game_map_tiles[] = {
  {0x0000,0xffff,0xffff}, {0x0000,0xffff,0xffff}, {0x0000,0xffff,0xffff}, {0x0000,0xffff,0xffff}, {0x0000,0xffff,0xffff}, {0x0000,0xffff,0xffff}, {0x0000,0xffff,0xffff}, {0x0000,0xffff,0xffff},
  {0x0000,0xffff,0xffff}, {0x0000,0xffff,0xffff}, {0x0000,0xffff,0xffff}, {0x0000,0xffff,0xffff}, {0x0000,0xffff,0xffff}, {0x0000,0xffff,0xffff}, {0x0000,0xffff,0xffff}, {0x0000,0xffff,0xffff},

  {0x0000,0xffff,0xffff}, {0x001e,0xffff,0xffff}, {0x001f,0xffff,0xffff}, {0x0000,0xffff,0xffff}, {0x0000,0xffff,0xffff}, {0x0000,0xffff,0xffff}, {0x0000,0xffff,0xffff}, {0x0000,0xffff,0xffff},
  {0x0000,0xffff,0xffff}, {0x0002,0xffff,0xffff}, {0x0000,0xffff,0xffff}, {0x0000,0xffff,0xffff}, {0x0000,0xffff,0xffff}, {0x0000,0xffff,0xffff}, {0x0000,0xffff,0xffff}, {0x0000,0xffff,0xffff},

  {0x0000,0xffff,0xffff}, {0x0020,0xffff,0xffff}, {0x0021,0xffff,0xffff}, {0x0000,0xffff,0xffff}, {0x0002,0xffff,0xffff}, {0x0000,0xffff,0xffff}, {0x0000,0xffff,0xffff}, {0x0000,0xffff,0xffff},
  {0x0000,0x0007,0xffff}, {0x0027,0x0008,0xffff}, {0x0000,0x0009,0xffff}, {0x0000,0xffff,0xffff}, {0x0000,0xffff,0xffff}, {0x0000,0xffff,0xffff}, {0x003d,0xffff,0xffff}, {0x0000,0xffff,0xffff},

  {0x0000,0xffff,0xffff}, {0x0022,0xffff,0xffff}, {0x0023,0xffff,0xffff}, {0x0012,0x0018,0xffff}, {0x0013,0x0019,0xffff}, {0x0014,0x001a,0xffff}, {0x0000,0xffff,0xffff}, {0x0000,0xffff,0xffff},
  {0x0000,0xffff,0xffff}, {0x0028,0xffff,0xffff}, {0x0000,0xffff,0xffff}, {0x0000,0xffff,0xffff}, {0x0000,0xffff,0xffff}, {0x0000,0x000d,0xffff}, {0x0000,0x000e,0xffff}, {0x0000,0x000e,0xffff},

  {0xffff,0x0001,0xffff}, {0xffff,0x0001,0xffff}, {0xffff,0x0001,0xffff}, {0x0024,0xffff,0xffff}, {0x0025,0xffff,0xffff}, {0x0026,0xffff,0xffff}, {0xffff,0x0001,0xffff}, {0xffff,0x0001,0xffff},
  {0xffff,0x0001,0xffff}, {0xffff,0x0001,0xffff}, {0xffff,0x0001,0xffff}, {0xffff,0x0001,0xffff}, {0xffff,0x0001,0xffff}, {0xffff,0x0001,0xffff}, {0xffff,0x0001,0xffff}, {0xffff,0x0001,0xffff},
};

const struct GAME_MAP game_map = {
  .tileset = &game_images[0],
  .tiles = game_map_tiles,
  .spawn_points = 0,
  .num_spawn_points = 0,
  .width = 16,
  .height = 5,
};


const struct GAME_CHAR_DEF game_loserboy_def = {
  .clip = { 15, 5, 31, 35 },
  .mirror = 11,
  .shoot_frame = 22,
  .num_stand = 1,
  .stand = { 10 },
  .num_jump = 1,
  .jump = { 4 },
  .num_walk = 36,
  .walk = {
    5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 8, 8, 7, 7, 6, 6, 5, 5,
    0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 3, 3, 2, 2, 1, 1, 0, 0,
  },
};

struct GAME_DATA game_data;

struct GAME_SPRITE game_sprites[] = {
  { &game_images[1], 64, 64, 0, },   // local player
};

struct GAME_CHAR game_local_player = {
  .def = &game_loserboy_def,
  .spr = &game_sprites[0],
};