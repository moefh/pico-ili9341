/* screen.c */

#include <string.h>
#include "pico/stdlib.h"

#include "screen.h"
#include "game_data.h"
#include "lib/ili9341.h"

void screen_init(void)
{
  screen.width = 320;
  screen.height = 240;
  font_set_font(&font6x8);
  font_set_color(0xff);

  game_local_player.x = 90;
  game_local_player.y = 220;
  game_local_player.state = GAME_CHAR_STATE_WALK;
  game_local_player.dir = GAME_DIR_RIGHT;
}

static int count_fps(int cur_millis, int last_millis)
{
  static int last_fps;
  static int fps_frame_count;
  
  if (cur_millis/1000 != last_millis/1000) {
    last_fps = fps_frame_count;
    fps_frame_count = 1;
  } else {
    fps_frame_count++;
  }
  return last_fps;
}

static void update_sprite(struct GAME_CHAR *ch)
{
  int frame_base = 0;
  switch (ch->state) {
  case GAME_CHAR_STATE_STAND:    frame_base = ch->def->stand[(ch->frame>>1) % ch->def->num_stand]; break;
  case GAME_CHAR_STATE_WALK:     frame_base = ch->def->walk [(ch->frame>>1) % ch->def->num_walk];  break;
  case GAME_CHAR_STATE_JUMP_START:
  case GAME_CHAR_STATE_JUMP_END: frame_base = ch->def->jump [(ch->frame>>1) % ch->def->num_jump];  break;
  default:                       frame_base = 0; break;
  }
  ch->spr->frame = frame_base + ((ch->dir==GAME_DIR_LEFT) ? ch->def->mirror : 0) + ((ch->shooting_pose>0) ? ch->def->shoot_frame : 0);
  ch->spr->x = ch->x + 1 + ((ch->dir == GAME_DIR_RIGHT) ? -ch->def->clip.x : ch->def->clip.x + ch->def->clip.width - ch->spr->img->width - 1);
  ch->spr->y = ch->y + 1 - ch->def->clip.y;
}

static void camera_follow_character(const struct GAME_CHAR *ch)
{
  int x = ch->x + ch->def->clip.width/2;
  int y = ch->y + ch->def->clip.height/2;
  if (game_data.camera_x < x - CAMERA_TETHER_X) game_data.camera_x = x - CAMERA_TETHER_X;
  if (game_data.camera_x > x + CAMERA_TETHER_X) game_data.camera_x = x + CAMERA_TETHER_X;
  if (game_data.camera_y < y - CAMERA_TETHER_Y) game_data.camera_y = y - CAMERA_TETHER_Y;
  if (game_data.camera_y > y + CAMERA_TETHER_Y) game_data.camera_y = y + CAMERA_TETHER_Y;
}

static void move_character(void)
{
  if (game_local_player.dir == GAME_DIR_LEFT) {
    game_local_player.x -= 3;
    if (game_local_player.x < TILE_WIDTH) {
      game_local_player.dir = GAME_DIR_RIGHT;
    }
  } else {
    game_local_player.x += 3;
    if (game_local_player.x >= (game_map.width-1)*TILE_WIDTH) {
      game_local_player.dir = GAME_DIR_LEFT;
    }
  }
  game_local_player.frame++;
  update_sprite(&game_local_player);
  camera_follow_character(&game_local_player);
}

static void get_screen_pos(int *r_screen_x, int *r_screen_y)
{
  int screen_x = game_data.camera_x - screen.width/2;
  int screen_y = game_data.camera_y - screen.height/2;

  if (screen_x < 0) {
    screen_x = 0;
  } else if (screen_x >= game_map.width*TILE_WIDTH - screen.width) {
    screen_x = game_map.width*TILE_WIDTH - screen.width - 1;
  }

  if (screen_y < 0) {
    screen_y = 0;
  } else if (screen_y >= game_map.height*TILE_HEIGHT - screen.height) {
    screen_y = game_map.height*TILE_HEIGHT - screen.height - 1;
  }

  *r_screen_x = screen_x;
  *r_screen_y = screen_y;
}

static void render_game_frame(void)
{
  int screen_x, screen_y;
  get_screen_pos(&screen_x, &screen_y);
  
  int tile_x_first = screen_x/TILE_WIDTH;
  int tile_x_last = (screen_x+screen.width)/TILE_WIDTH;
  int tile_y_first = screen_y/TILE_HEIGHT;
  int tile_y_last = (screen_y+screen.height)/TILE_HEIGHT;

  int x_pos_start = -(screen_x%TILE_WIDTH);
  int y_pos_start = -(screen_y%TILE_HEIGHT);

  int y_pos;

  //background tiles
  y_pos = y_pos_start;
  for (int tile_y = tile_y_first; tile_y <= tile_y_last; tile_y++) {
    int x_pos = x_pos_start;
    const struct GAME_MAP_TILE *tiles = &game_map.tiles[tile_y*game_map.width];
    for (int tile_x = tile_x_first; tile_x <= tile_x_last; tile_x++) {
      int tile_num = tiles[tile_x].back;
      if (tile_num != 0xffff) {
        image_draw_frame(game_map.tileset, tile_num, x_pos, y_pos, false);
      }
      x_pos += TILE_WIDTH;
    }
    y_pos += TILE_HEIGHT;
  }
  
  // sprites
  for (int i = 0; i < 1; i++) {
    if (! game_sprites[i].img) continue;
    int spr_x = game_sprites[i].x - screen_x;
    int spr_y = game_sprites[i].y - screen_y;
    if (spr_x <= -game_sprites[i].img->width) continue;
    if (spr_y <= -game_sprites[i].img->height) continue;
    if (spr_x >= screen.width || spr_y >= screen.height) continue;
    image_draw_frame(game_sprites[i].img, game_sprites[i].frame, spr_x, spr_y, true);
  }

  // foreground tiles
  y_pos = y_pos_start;
  for (int tile_y = tile_y_first; tile_y <= tile_y_last; tile_y++) {
    int x_pos = x_pos_start;
    const struct GAME_MAP_TILE *tiles = &game_map.tiles[tile_y*game_map.width];
    for (int tile_x = tile_x_first; tile_x <= tile_x_last; tile_x++) {
      int tile_num = tiles[tile_x].fore;
      if (tile_num != 0xffff) {
        image_draw_frame(game_map.tileset, tile_num, x_pos, y_pos, true);
      }
      x_pos += TILE_WIDTH;
    }
    y_pos += TILE_HEIGHT;
  }
  
  font_align(FONT_ALIGN_RIGHT);
  font_move(screen.width-10, 10); font_printf("x = %-4d", game_sprites[0].x);
  font_move(screen.width-10, 20); font_printf("y = %-4d", game_sprites[0].y);
  font_move(screen.width-10, 30); font_printf("frame = %-4d", game_sprites[0].frame);
}

void screen_render(void)
{
  static int last_millis;

  move_character();
  render_game_frame();

  int cur_millis = to_ms_since_boot(get_absolute_time());
  font_align(FONT_ALIGN_LEFT);
  font_move(10, 10); font_printf("%d fps", count_fps(cur_millis, last_millis));
  last_millis = cur_millis;
}
