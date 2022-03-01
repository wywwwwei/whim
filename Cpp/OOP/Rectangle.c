#include "Rectangle.h"
#include <stdio.h>

static uint32_t Rectangle_area_(const Shape *const self);
static void Rectangle_draw_(const Shape *const self);

void Rectangle_ctor(Rectangle *const self, int16_t x, int16_t y, uint16_t width, uint16_t height)
{
    static const struct ShapeVtable vtable =
        {
            &Rectangle_area_,
            &Rectangle_draw_,
        };
    Shape_ctor(&self->super, x, y);
    self->super.vptr = &vtable;
    self->width = width;
    self->height = height;
}

// Shape's virtual function implementation
static uint32_t Rectangle_area_(const Shape *const self)
{
    const Rectangle *const self_ = (const Rectangle *)self;
    return (uint32_t)self_->width * (uint32_t)self_->height;
}

static void Rectangle_draw_(const Shape *const self)
{
    const Rectangle *const self_ = (const Rectangle *)self;
    printf("Rectangle width: %d height: %d x: %d y: %d\n", self_->width, self_->height, Shape_getX(self), Shape_getY(self));
}