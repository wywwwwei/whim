#include "Shape.h"
#include <assert.h>

// virtual functions
static uint32_t Shape_area_(const Shape *const self);
static void Shape_draw_(const Shape *const self);

void Shape_ctor(Shape *const self, int16_t x, int16_t y)
{
    static const struct ShapeVtable vtable = {
        &Shape_area_,
        &Shape_draw_,
    };
    self->vptr = &vtable;
    self->x = x;
    self->y = y;
}

int16_t Shape_getX(const Shape *const self)
{
    return self->x;
}

int16_t Shape_getY(const Shape *const self)
{
    return self->y;
}

// Shape's virtual function implementation
static uint32_t Shape_area_(const Shape *const self)
{
    assert(0);  // pure virtual
    return 0u;
}

static void Shape_draw_(const Shape *const self)
{
    assert(0); // pure virtual
}