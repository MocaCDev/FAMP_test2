#ifndef FAMP_colors
#define FAMP_colors

/* Color attributes for `print` */
enum color_attr
{
    black           = 0x00,
    blue            = 0x01,
    green           = 0x02,
    cyan            = 0x03,
    red             = 0x04,
    magenta         = 0x05,
    brown           = 0x06,
    light_grey      = 0x07,
    dark_grey       = 0x08,
    light_blue      = 0x09,
    lime_green      = 0x0A,
    light_cyan      = 0x0B,
    light_red       = 0x0C,
    light_magenta   = 0x0D,
    yellow          = 0x0E,
    white           = 0x0F,
};

#define create_color_comb(background, foreground) \
    (background << 4) | (foreground & 0x0F)

#endif