#ifndef CORE_MSG_H_FILE
#define CORE_MSG_H_FILE

#include <stdint.h>
#include <stdbool.h>
#include "pico/util/queue.h"

// message types
enum CORE_MSG_TYPE {
  CORE_MSG_TYPE_SEND_FRAMEBUFFER,
};

struct CORE_MSG_HEADER {
  enum CORE_MSG_TYPE type;
};

struct CORE_MSG_SEND_FRAMEBUFFER {
  struct CORE_MSG_HEADER msg_header;
  uint32_t *framebuffer;
};

union CORE_MSG {
  struct CORE_MSG_HEADER msg_header;
  struct CORE_MSG_SEND_FRAMEBUFFER send_framebuffer;
};

#ifdef __cplusplus
extern "C" {
#endif

void core_msg_init(void);
bool core_msg_can_recv(void);
bool core_msg_can_send(void);
void core_msg_send(union CORE_MSG *msg);
void core_msg_recv(union CORE_MSG *msg);
bool core_msg_try_send(union CORE_MSG *msg);
bool core_msg_try_recv(union CORE_MSG *msg);

void msg_send_framebuffer(uint32_t *framebuffer);

#ifdef __cplusplus
}
#endif

#endif /* CORE_MSG_H_FILE */
