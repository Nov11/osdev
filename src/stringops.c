#include "stringops.h"

//size_t strlen(const char *str) {
//    size_t len = 0;
//    while (str[len])
//        len++;
//    return len;
//}


void memset(void *des, u8int c, u32int count) {
  u8int *ptr = des;
  while (count > 0) {
    *ptr++ = c;
    count--;
  }
}
// Copy len bytes from src to dest.
void memcpy(u8int *dest, const u8int *src, u32int len) {
//    const u8int *sp = (const u8int *)src;
//    u8int *dp = (u8int *)dest;
//    for(; len != 0; len--) *dp++ = *sp++;
  while (len > 0) {
    *dest++ = *src++;
    len--;
  }
}
// Copy the NULL-terminated string src into dest, and
// return dest.
char *strcpy(char *dest, const char *src) {
//  do {
//    *dest++ = *src++;
//  } while (*src != 0);
  char* ret = dest;
  while (src && *src) {
    *dest++ = *src++;
  }
  *dest = 0;
  return ret;
}

int strlen(char *src) {
  int i = 0;
  while (src && *src++)
    i++;
  return i;
}

// Compare two strings. Should return -1 if
// str1 < str2, 0 if they are equal or 1 otherwise.
int strcmp(char *str1, char *str2) {
//  int i = 0;
//  int failed = 0;
//  while(str1[i] != '\0' && str2[i] != '\0')
//  {
//    if(str1[i] != str2[i])
//    {
//      failed = 1;
//      break;
//    }
//    i++;
//  }
//  // why did the loop exit?
//  if( (str1[i] == '\0' && str2[i] != '\0') || (str1[i] != '\0' && str2[i] == '\0') )
//    failed = 1;
//
//  return failed;
  ASSERT(str1 && str2);
  while (*str1 && *str2) {
    if (*str1 < *str2) {
      return -1;
    } else if (*str1 > *str2) {
      return 1;
    }
    str1++;
    str2++;
  }
  if (*str1 == 0 && *str2 == 0) {
    return 0;
  }
  if (*str1 == 0) {
    return -1;
  }
  return 1;
}
