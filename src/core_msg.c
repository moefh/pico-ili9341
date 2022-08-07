
#include "pico/multicore.h"

#include "core_msg.h"

#define MSG_QUEUE_LENGTH 4  // max messages in queue

static queue_t core_msg_queue;

void core_msg_init(void)
{
  queue_init(&core_msg_queue, sizeof(union CORE_MSG), MSG_QUEUE_LENGTH);
}

bool core_msg_can_recv(void)
{
  return queue_is_empty(&core_msg_queue);
}

bool core_msg_can_send(void)
{
  return ! queue_is_full(&core_msg_queue);
}

void core_msg_send(union CORE_MSG *msg)
{
  return queue_add_blocking(&core_msg_queue, msg);
}

void core_msg_recv(union CORE_MSG *msg)
{
  return queue_remove_blocking(&core_msg_queue, msg);
}

bool core_msg_try_send(union CORE_MSG *msg)
{
  return queue_try_add(&core_msg_queue, msg);
}

bool core_msg_try_recv(union CORE_MSG *msg)
{
  return queue_try_remove(&core_msg_queue, msg);
}

void msg_send_framebuffer(uint32_t *framebuffer)
{
  union CORE_MSG msg;
  msg.msg_header.type = CORE_MSG_TYPE_SEND_FRAMEBUFFER;
  msg.send_framebuffer.framebuffer = framebuffer;
  core_msg_send(&msg);
}
