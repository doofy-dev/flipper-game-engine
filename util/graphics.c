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
    return data[(y >> 3) * w + x] & (1 << (y & 7));
}

uint8_t *get_buffer(Canvas *const canvas) {
    return canvas->fb.tile_buf_ptr;
}

uint8_t *make_buffer(uint8_t w, uint8_t h) {
    UNUSED(w);
    UNUSED(h);
    return allocate(sizeof(uint8_t) * 1024);
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

void draw_color_as_orig(Canvas *const canvas, Vector *const position, sprite_t *const sprite, bool is_black,
                   PixelColor draw_color, const Vector *scale, float rotation) {
    UNUSED(rotation);
    int16_t new_w = round(sprite->size.x * scale->x);
    int16_t new_h = round(sprite->size.y * scale->y);

    int16_t offsetX = round(position->x - (sprite->anchor.x * (new_w / 2.0)));
    int16_t offsetY = round(position->y - (sprite->anchor.y * (new_h / 2.0)));

    float xInc = sprite->size.x / (float) new_w;
    float yInc = sprite->size.y / (float) new_h;

    new_w = min(new_w, SCREEN_WIDTH);
    new_h = min(new_h, SCREEN_HEIGHT);
    uint16_t test_pixel_x;
    float pixel_x = 0;
    float pixel_y = 0;
    bool isOn;
    int16_t _x, _y;

    int16_t __x;
    int8_t __y;
    for (_x = 0; _x < new_w; _x++) {
        test_pixel_x = floor(pixel_x);
        pixel_y = 0;
        for (_y = 0; _y < new_h; _y++) {
            __x = _x + offsetX;
            __y = _y + offsetY;

            if (__y < SCREEN_HEIGHT && __y >= 0 && __x >= 0 && __x < SCREEN_WIDTH) {
                isOn = test_pixel(sprite->asset->data, test_pixel_x, floor(pixel_y), sprite->size.x) == is_black;
                if (isOn)
                    set_pixel(canvas->fb.tile_buf_ptr, __x, __y, SCREEN_WIDTH, draw_color);
            }
            pixel_y += yInc;
        }
        pixel_x += xInc;
    }
}

void draw_color_as(uint8_t* canvas, Vector *const position, sprite_t *const sprite, bool is_black,
                   PixelColor draw_color, const Vector *scale, float rotation) {
    int16_t new_w = round(sprite->size.x * scale->x);
    int16_t new_h = round(sprite->size.y * scale->y);

    int16_t offsetX = round(position->x - (sprite->anchor.x * (new_w / 2.0)));
    int16_t offsetY = round(position->y - (sprite->anchor.y * (new_h / 2.0)));

    float xInc = sprite->size.x / (float) new_w;
    float yInc = sprite->size.y / (float) new_h;

    new_w = min(new_w, SCREEN_WIDTH);
    new_h = min(new_h, SCREEN_HEIGHT);
    uint16_t test_pixel_x;
    float pixel_x = 0;
    float pixel_y = 0;
    bool isOn;
    int16_t _x, _y;

    int16_t __x, __y;
    float cos_theta = cos(rotation);
    float sin_theta = sin(rotation);

    for (_x = 0; _x < new_w; _x++) {
        test_pixel_x = floor(pixel_x);
        pixel_y = 0;
        for (_y = 0; _y < new_h; _y++) {
            // Perform rotation
            __x = round((_x - new_w / 2.0) * cos_theta - (_y - new_h / 2.0) * sin_theta + new_w / 2.0 + offsetX);
            __y = round((_x - new_w / 2.0) * sin_theta + (_y - new_h / 2.0) * cos_theta + new_h / 2.0 + offsetY);

            if (__y < SCREEN_HEIGHT && __y >= 0 && __x >= 0 && __x < SCREEN_WIDTH) {
                isOn = test_pixel(sprite->asset->data, test_pixel_x, floor(pixel_y), sprite->size.x) == is_black;
                if (isOn)
                    set_pixel(canvas, __x, __y, SCREEN_WIDTH, draw_color);
            }
            pixel_y += yInc;
        }
        pixel_x += xInc;
    }
}

void
draw_buffer_scaled(uint8_t* canvas, Vector *const position, sprite_t *const sprite, Vector *const scale,
                   float rotation) {
    switch (sprite->draw_mode) {
        default:
        case BlackOnly:
            draw_color_as(canvas, position, sprite, true, Black, scale, rotation);
            break;
        case WhiteOnly:
            draw_color_as(canvas, position, sprite, false, White, scale, rotation);
            break;
        case WhiteAsBlack:
            draw_color_as(canvas, position, sprite, false, Black, scale, rotation);
            break;
        case BlackAsWhite:
            draw_color_as(canvas, position, sprite, true, White, scale, rotation);
            break;
        case WhiteAsInverted:
            draw_color_as(canvas, position, sprite, false, Flip, scale, rotation);
            break;
        case BlackAsInverted:
            draw_color_as(canvas, position, sprite, true, Flip, scale, rotation);
            break;
    }
}

void set_pixel(uint8_t *buffer, int16_t x, int16_t y, uint8_t sw, PixelColor draw_mode) {
    uint8_t bit = 1 << (y & 7);
    uint8_t *p = buffer + (y >> 3) * sw + x;

    switch (draw_mode) {
        case Black:
            *p |= bit;
            break;
        case White:
            *p &= ~bit;
            break;
        case Flip:
            *p ^= bit;
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

            if (d->icon == icon)
                return d;
            item = item->next;
        }
    }
    FURI_LOG_I("FlipperGameEngine", "Image loaded #%p", icon);

    ImageAsset *a = allocate(sizeof(ImageAsset));
    a->icon = icon;
    a->loaded = false;
    list_add(images, a);
    return a;
}

void clear_image_assets() {
    if (images == NULL)
        return;

    t_ListItem *item = images->start;
    if (item != NULL) {
        while (item) {
            check_pointer(item);
            t_ListItem *t = item;
            check_pointer(t);
            ImageAsset *d = (ImageAsset *) t->data;
            check_pointer(d);
            check_pointer(d->data);
            if (d->data)
                release(d->data);

            item = item->next;
        }
    }
    list_clear(images);
}
void copy_to_screen_buffer(uint8_t* src, uint8_t* dst){
    for(int i=0;i<1024;i++){
        dst[i]=src[i];
    }
}
void clear_buffer(uint8_t* src){
    for(int i=0;i<1024;i++){
        src[i]=0;
    }
}