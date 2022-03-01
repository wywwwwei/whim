#include "Shape.h"

typedef struct
{
    Shape super;
    uint16_t width;
    uint16_t height;
} Rectangle;

void Rectangle_ctor(Rectangle *const self, int16_t x, int16_t y, uint16_t width, uint16_t height);