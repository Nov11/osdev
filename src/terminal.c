

#include "terminal.h"
#include "stringops.h"
#include "common.h"

/* Hardware text mode color constants. */
enum vga_color {
  VGA_COLOR_BLACK = 0,
  VGA_COLOR_BLUE = 1,
  VGA_COLOR_GREEN = 2,
  VGA_COLOR_CYAN = 3,
  VGA_COLOR_RED = 4,
  VGA_COLOR_MAGENTA = 5,
  VGA_COLOR_BROWN = 6,
  VGA_COLOR_LIGHT_GREY = 7,
  VGA_COLOR_DARK_GREY = 8,
  VGA_COLOR_LIGHT_BLUE = 9,
  VGA_COLOR_LIGHT_GREEN = 10,
  VGA_COLOR_LIGHT_CYAN = 11,
  VGA_COLOR_LIGHT_RED = 12,
  VGA_COLOR_LIGHT_MAGENTA = 13,
  VGA_COLOR_LIGHT_BROWN = 14,
  VGA_COLOR_WHITE = 15,
};

static inline uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg) {
  return fg | bg << 4;
}

static inline uint16_t vga_entry(unsigned char uc, uint8_t color) {
  return (uint16_t) uc | (uint16_t) color << 8;
}

static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;

size_t cursor_y;
size_t cursor_x;
uint8_t terminal_color;
uint16_t *const video_memory = (uint16_t *) 0xB8000;

void terminal_initialize(void) {
  cursor_y = 0;
  cursor_x = 0;
  terminal_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
  for (size_t y = 0; y < VGA_HEIGHT; y++) {
    for (size_t x = 0; x < VGA_WIDTH; x++) {
      const size_t index = y * VGA_WIDTH + x;
      video_memory[index] = vga_entry(' ', terminal_color);
    }
  }
}

void terminal_setcolor(uint8_t color) {
  terminal_color = color;
}

void terminal_putentryat(char c, uint8_t color, size_t x, size_t y) {
  const size_t index = y * VGA_WIDTH + x;
  video_memory[index] = vga_entry(c, color);
}

static void move_cursor() {
  uint16_t cursorLocation = cursor_y * 80 + cursor_x;
  outb(0x3d4, 14);//发送高字节
  outb(0x3d5, cursorLocation >> 8);
  outb(0x3d4, 15);//发低字节
  outb(0x3d5, cursorLocation);
}

static void scroll() {
  uint8_t color = vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK); //黑底白字
  uint16_t blank = vga_entry(' ', color);//黑底白字的空格

  if (cursor_y >= 25) {
    int i;
    for (i = 0; i < 24 * 80; i++) {
      video_memory[i] = video_memory[i + 80];
    }

    for (i = 24 * 80; i < 25 * 80; i++) {
      video_memory[i] = blank;
    }

    cursor_y = 24;
  }
}

void monitor_put(char c) {
  uint8_t attribute = 0x0f;

  switch (c) {
  case 0x08://backspace
    if (cursor_x > 0) {
      //delete char use *((cursor_x + cursor_y * 80) * 2) = 0x2f;
      cursor_x -= 1;
    }
    break;
  case 0x09://tab
    cursor_x = (cursor_x + 8) & ~(8 - 1);
    break;
  case '\r'://0x0d cr
    cursor_x = 0;
    break;
  case '\n'://0x0a line feed
    cursor_x = 0;
    cursor_y++;
    break;
  default: {
    uint16_t *loc = video_memory + (cursor_y * 80 + cursor_x); //video_memory is uint16_t, don't *2 here
    *loc = c | attribute << 8;
    cursor_x++;
  }
    break;
  }
  if (cursor_x >= 80) {
    cursor_x = 0;
    cursor_y++;
  }
  scroll();
  move_cursor();
}

void monitor_clear() {
  uint16_t blank = 0x2f;
  int i;

  for (i = 0; i < 25 * 80; i++) {
    video_memory[i] = blank;
  }

  cursor_x = 0;
  cursor_y = 0;
  move_cursor();
}

void monitor_write(const char *c) {
  if (c == NULL) {
    return;
  }
  while (*c) {
    monitor_put(*c++);
  }
}

void monitor_write_hex(u32int n) {
  //32 / 4 = 8
  char hex[9] = {0};
  int index = 7;

  for (index = 7; index >= 0; index--) {
    u32int tmp = n % 16;
    switch (tmp) {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:hex[index] = tmp + '0';
      break;
    case 10:hex[index] = 'A';
      break;
    case 11:hex[index] = 'B';
      break;
    case 12:hex[index] = 'C';
      break;
    case 13:hex[index] = 'D';
      break;
    case 14:hex[index] = 'E';
      break;
    case 15:hex[index] = 'F';
      break;
    }
    if (n < 16) {
      break;
    }
    n = n / 16;
  }
  index = index == -1 ? index + 1 : index;
  monitor_write(&hex[index]);
}

void monitor_write_dec(u32int n) {
  char dec[11] = {0};
  int index = 9;

  for (; index >= 0; index--) {
    u32int tmp = n % 10;

    dec[index] = tmp + '0';
    if (n < 10) {
      break;
    }
    n /= 10;
  }
  index = index == -1 ? 0 : index;
  monitor_write(&dec[index]);
}

void prtf(const char *format, ...) {
  char **arg = (void *) &format;//存format的地址
  char c;

  //arg是format这个参数的下一个参数的地址,也就是字符串后第一个参数的地址
  arg = arg + 1;
  while ((c = *format++) != 0) {
    if (c == '\\') {
      c = *format++;
      switch (c) {
      case 'n':monitor_put('\n');
        break;
      case 't':monitor_put('\t');
        break;
      default:monitor_put('\\');
        monitor_put(c);
      }
    } else if (c == '%') {
      c = *format++;
      switch (c) {
      case 'u':monitor_write_dec(*(u32int *) arg);
        break;
      case 'x':monitor_write_hex(*(u32int *) arg);
        break;
      case 's':monitor_write(*arg);
        break;
      case 'p':monitor_write_hex(*(u32int *) arg);
        break;
      default:monitor_put('%');
        monitor_put(c);
      }
      arg++;
    } else {
      monitor_put(c);
    }
  }
}

void panic(const char *msg, const char *file, u32int line) {
  prtf("%s %s %u\n", msg, file, line);
  while (1);
}

void panic_assert(const char *file, u32int line, const char *des) {
  prtf("%s %u %s\n", file, des, line);
  while (1);
}
