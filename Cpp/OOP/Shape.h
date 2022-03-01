#include <stdint.h>

struct ShapeVtable;

typedef struct
{
    const struct ShapeVtable *vptr;
    int16_t x;
    int16_t y;
} Shape;

typedef struct ShapeVtable
{
    uint32_t (*area)(const Shape *const self);
    void (*draw)(const Shape *const self);
} ShapeVtable;

void Shape_ctor(Shape *const self, int16_t x, int16_t y);
int16_t Shape_getX(const Shape *const self);
int16_t Shape_getY(const Shape *const self);

// The virtual function's calling interface
static inline uint32_t Shape_area(const Shape *const self)
{
    return (*self->vptr->area)(self);
}

static inline void Shape_draw(const Shape *const self)
{
    (*self->vptr->draw)(self);
}
