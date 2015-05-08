#ifndef __FBINFO_H__
#define __FZINFO_H__

struct fbinfo {
  _u32 version;
  _u32 bpp;
  _u32 size;
  _u32 width;
  _u32 height;
  _u32 red_offset;
  _u32 red_length;
  _u32 blue_offset;
  _u32 blue_length;
  _u32 green_offset;
  _u32 green_length;
  _u32 alpha_offset;
  _u32 alpha_length;
} __attribute__((packed));

#endif

