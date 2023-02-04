#include "graphics.h"
#include <gui/canvas_i.h>
#include <math.h>
#include <u8g2_glue.h>
#include "util.h"
#include "list.h"
#include <gui/icon_i.h>

List *images = NULL;

bool in_screen(int16_t x, int16_t y) {
    return x >= 0 && x < SCREEN_WIDTH && y >= 0 && y < SCREEN_HEIGHT;
}

bool test_pixel(const uint8_t *data, uint8_t x, uint8_t y, uint8_t w) {
    uint8_t current_bit = (y % 8);
    uint8_t current_row = ((y - current_bit) / 8);
    uint8_t current_value = data[current_row * w + x];

    return current_value & (1 << current_bit);
}

uint8_t *get_buffer(Canvas *const canvas) {
    return canvas->fb.tile_buf_ptr;
}

uint8_t *make_buffer(uint8_t w, uint8_t h) {
    return allocate(sizeof(uint8_t) * 8 * w * ceil(h / 8.0));
}

void clone_buffer(uint8_t *buf, uint8_t *data) {
    for (int i = 0; i < 1024; i++) {
        data[i] = buf[i];
    }
}

bool read_pixel(Canvas *const canvas, int16_t x, int16_t y) {
    if (in_screen(x, y)) {
        return test_pixel(get_buffer(canvas), x, y, SCREEN_WIDTH);
    }
    return false;
}


void draw_default_mode(Canvas *const canvas, Vector position, const sprite_t *sprite, const Vector *scale) {
    float new_w = round(sprite->size.x * scale->x);
    float new_h = round(sprite->size.y * scale->y);

    int16_t offsetX = round(position.x - (sprite->anchor.x * (new_w / 2.0)));
    int16_t offsetY = round(position.y - (sprite->anchor.y * (new_h / 2.0)));

    float xInc = sprite->size.x / (float) new_w;
    float yInc = sprite->size.y / (float) new_h;

    uint8_t test_pixel_x = 0;
    float pixel_x = 0;
    for (int8_t _x = 0; _x < new_w; _x++) {
        test_pixel_x = floor(pixel_x);
        for (int8_t _y = 0; _y < new_h; _y++) {
            test_pixel_x = floor(pixel_x);
            float pixel_y = 0;
            bool isOn = test_pixel(sprite->asset->data, test_pixel_x, floor(pixel_y), sprite->size.x);

            int8_t __x = _x + offsetX;
            int8_t __y = _y + offsetY;
            if (__x >= 0 && __y >= 0 && __y < SCREEN_HEIGHT)
                set_pixel(canvas->fb.tile_buf_ptr, __x, __y, SCREEN_WIDTH, isOn ? Black : White);
            pixel_y += yInc;
        }
        pixel_x += xInc;
    }
}

void draw_color_as(Canvas *const canvas, Vector position, const sprite_t *sprite, bool is_black,
                   PixelColor draw_color, const Vector *scale) {
    float new_w = round(sprite->size.x * scale->x);
    float new_h = round(sprite->size.y * scale->y);

    int16_t offsetX = round(position.x - (sprite->anchor.x * (new_w / 2.0)));
    int16_t offsetY = round(position.y - (sprite->anchor.y * (new_h / 2.0)));

    float xInc = sprite->size.x / (float) new_w;
    float yInc = sprite->size.y / (float) new_h;

    uint8_t test_pixel_x = 0;
    float pixel_x = 0;
    for (int8_t _x = 0; _x < new_w; _x++) {
        test_pixel_x = floor(pixel_x);
        float pixel_y = 0;
        for (int8_t _y = 0; _y < new_h; _y++) {
            bool isOn = test_pixel(sprite->asset->data, test_pixel_x, floor(pixel_y), sprite->size.x);
            int8_t __x = _x + offsetX;
            int8_t __y = _y + offsetY;
            if (isOn == is_black && __x >= 0 && __y >= 0 && __y < SCREEN_HEIGHT)
                set_pixel(canvas->fb.tile_buf_ptr, __x, __y, SCREEN_WIDTH, draw_color);
            pixel_y += yInc;
        }
        pixel_x += xInc;
    }
}

void draw_buffer(Canvas *const canvas, Vector position, const sprite_t *sprite) {
    draw_buffer_scaled(canvas, position, sprite, (Vector) {1, 1});
}

void
draw_buffer_scaled(Canvas *const canvas, Vector position, const sprite_t *sprite, Vector scale) {

    switch (sprite->draw_mode) {
        case Default:
            draw_default_mode(canvas, position, sprite, &scale);
            break;
        case WhiteOnly:
            draw_color_as(canvas, position, sprite, false, White, &scale);
            break;
        case BlackOnly:
            draw_color_as(canvas, position, sprite, true, Black, &scale);
            break;
        case WhiteAsBlack:
            draw_color_as(canvas, position, sprite, false, Black, &scale);
            break;
        case BlackAsWhite:
            draw_color_as(canvas, position, sprite, true, White, &scale);
            break;
        case WhiteAsInverted:
            draw_color_as(canvas, position, sprite, false, Flip, &scale);
            break;
        case BlackAsInverted:
            draw_color_as(canvas, position, sprite, true, Flip, &scale);
            break;
    }
}

void set_pixel(uint8_t *buffer, int16_t x, int16_t y, uint8_t sw, PixelColor draw_mode) {
    uint8_t current_bit = (y % 8);
    uint8_t current_row = ((y - current_bit) / 8);
    uint32_t i = current_row * sw + x;
    uint8_t current_value = buffer[i];
    switch (draw_mode) {
        case Black:
            buffer[i] = current_value | (1 << current_bit);
            break;
        case White:
            buffer[i] = current_value & ~(1 << current_bit);
            break;
        case Flip:
            buffer[i] = current_value ^ (1 << current_bit);
            break;
    }
}


sprite_t load_sprite(const Icon *icon) {
    sprite_t sprite;
    sprite.size = (Vector) {icon_get_width(icon), icon_get_height(icon)};
    sprite.asset = new_image_asset(icon);
    return sprite;
}

void render_sprite(Canvas *const canvas, sprite_t *sprite) {
    FURI_LOG_I("FlipperGameEngine", "Rendering image");

    uint8_t *buf = make_buffer(sprite->size.x, sprite->size.y);
    uint8_t *renderBuf = make_buffer(SCREEN_WIDTH, SCREEN_HEIGHT);
    uint8_t *orig = canvas->fb.tile_buf_ptr;
    canvas->fb.tile_buf_ptr = renderBuf;
    canvas_draw_icon(canvas, 0, 0, sprite->asset->icon);
    canvas->fb.tile_buf_ptr = orig;
    for (uint8_t i = 0; i < sprite->size.x; i++) {
        for (uint8_t j = 0; j <= sprite->size.y; j++) {
            bool isOn = test_pixel(renderBuf, i, j, SCREEN_WIDTH);
            set_pixel(buf, i, j, sprite->size.x, isOn ? Black : White);
        }
    }
    release(renderBuf);
    sprite->asset->data = buf;
    sprite->asset->loaded = true;
}

ImageAsset *new_image_asset(const Icon *icon) {
    if (images == NULL)
        images = make_list(sizeof(Icon));

    t_ListItem *item = images->start;
    if (item != NULL) {
        while (item) {
            t_ListItem *t = item;
            ImageAsset *d = (ImageAsset *) t->data;
            FURI_LOG_I("FlipperGameEngine", "Testing %p and %p", d->icon, icon);

            if (d->icon == icon)
                return d;
            item = item->next;
        }
    }
    FURI_LOG_I("FlipperGameEngine", "new image %p", icon);

    ImageAsset *a = allocate(sizeof(ImageAsset));
    a->icon = icon;
    a->loaded = false;
    list_add(images, a);
    return a;
}

void clear_image_assets(){
    if (images == NULL)
        return;

    t_ListItem *item = images->start;
    if (item != NULL) {
        while (item) {
            t_ListItem *t = item;
            ImageAsset *d = (ImageAsset *) t->data;
            if(d->data)
                release(d->data);

            item = item->next;
        }
    }
    list_clear(images);
}