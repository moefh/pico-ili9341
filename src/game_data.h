#ifndef GAME_DATA_H_FILE
#define GAME_DATA_H_FILE

#include <stdint.h>
#include "lib/image.h"
#include "lib/font.h"

#define TILE_WIDTH  64
#define TILE_HEIGHT 64

#define CAMERA_TETHER_X  40
#define CAMERA_TETHER_Y  60

enum {
  GAME_DIR_LEFT,
  GAME_DIR_RIGHT,
};

enum {
  GAME_CHAR_STATE_STAND,
  GAME_CHAR_STATE_WALK,
  GAME_CHAR_STATE_JUMP_START,
  GAME_CHAR_STATE_JUMP_END,
};

struct GAME_MAP_TILE {
  uint16_t back;
  uint16_t fore;
  uint16_t block;
};

struct GAME_MAP_POINT {
  uint32_t x;     // fixed 16.16
  uint32_t y;     // fixed 16.16
};

struct GAME_RECT {
  uint8_t x;
  uint8_t y;
  uint8_t width;
  uint8_t height;
};

struct GAME_DATA {
  int16_t camera_x;
  int16_t camera_y;
};

struct GAME_MAP_SPAWN_POINT {
  struct GAME_MAP_POINT pos;
  uint8_t dir;  // 0=left, 1=right
};

struct GAME_MAP {
  const struct IMAGE *tileset;
  const struct GAME_MAP_TILE *tiles;
  const struct GAME_MAP_SPAWN_POINT *spawn_points;
  uint8_t num_spawn_points;
  uint16_t width;
  uint16_t height;
};

struct GAME_SPRITE {
  const struct IMAGE *img;
  int16_t x;
  int16_t y;
  int16_t frame;
};

struct GAME_CHAR_DEF {
  struct GAME_RECT clip;
  uint8_t mirror;
  uint8_t shoot_frame;
  uint8_t num_stand;
  uint8_t stand[4];
  uint8_t num_jump;
  uint8_t jump[4];
  uint8_t num_walk;
  uint8_t walk[64];
};

struct GAME_CHAR {
  const struct GAME_CHAR_DEF *def;
  struct GAME_SPRITE *spr;
  int32_t x;
  int32_t y;
  int32_t dx;
  int32_t dy;
  uint32_t frame;
  uint8_t dir;     // GAME_DIR_xxx
  uint8_t state;   // GAME_CHAR_STATE_xxx
  uint8_t shooting_pose;
};


// const data
extern const struct FONT font6x8;
extern const struct GAME_MAP game_map;
extern const struct IMAGE game_images[];

// mutable data
extern struct GAME_DATA game_data;
extern struct GAME_SPRITE game_sprites[];
extern struct GAME_CHAR game_local_player;

#endif /* GAME_DATA_H_FILE */
