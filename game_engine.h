#pragma once
#include <input/input.h>
#include <furi.h>
#include <dolphin/dolphin.h>
#include <gui/view_port.h>
#include <gui/gui.h>
#include <gui/elements.h>
#include <notification/notification.h>
#include <notification/notification_messages.h>
#include "objects.h"
#include "util/list.h"

typedef enum {
    InputDown, InputUp, InputPress
} InputState;

typedef struct {
    InputKey inputKey;
    InputType inputType;
} LastInput;

typedef enum {
    EventTick, EventInput
} EventType;

typedef struct {
    EventType type;
    InputEvent inputEvent;
} AppEvent;

typedef struct {
    sprite_t *image;
    Vector render_scale;
    Vector position;
} RenderInfo;

typedef struct {
    RenderInfo render_list[64];
    uint8_t render_count;
} RenderQueue;

typedef struct {
    const char* app_name;
    InputState inputState[7];
    LastInput lastInput;
    unsigned int last_tick;
    unsigned int tick_delta;
    FuriMessageQueue *event_queue;
    void *game_state;
    ValueMutex render_mutex;
    RenderQueue *render_queue;
    NotificationApp *notification_app;
    ViewPort *viewport;
    FuriTimer *timer;
    Gui *gui;
    uint8_t *prev_frame;
    bool loaded;
    Scene *scene;
    bool processing;
} EngineState;

typedef struct {
    const char* app_name;
    size_t state_size;

    void (*init_state)(void *state);

    bool always_on_screen;
    uint32_t tick_rate;
} SetupState;


int32_t setup_engine(SetupState state);

void start_loop();

void set_scene(Scene *s);

unsigned int delta_tick();

unsigned int frame_tick();

void exit_app();

