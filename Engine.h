#pragma once

#include <notification/notification.h>
#include <gui/view_port.h>
#include <gui/gui.h>
#include "types/Input.h"
#include "types/Render.h"
#include "Scene.h"
#include "types/RenderBuffer.h"

class Engine {
    const char* app_name;
    Input last_input;
    uint8_t fps;
    InputState input_states[7];
    uint32_t last_tick;
    uint32_t delta_tick;
    NotificationApp *notification_app;
    Gui *gui;
    Canvas *canvas;

    RenderBuffer buffer;

    FuriThread *buffer_thread;
    FuriMutex *render_mutex;

    FuriPubSub* input;
    FuriPubSubSubscription* input_subscription;
    Scene *active_scene;
    bool loaded = false;
    bool processing = false;
    bool thread_loop = false;
    static Engine *instance;

    static void input_callback(const void* value, void *ctx);

public:
    Engine(const char* app_name, uint8_t fps, bool alwaysOn);
    ~Engine();

    void Start();
    void Stop();
    void SetScene(Scene *scene);
    static int32_t render_thread(void *ctx);
};
