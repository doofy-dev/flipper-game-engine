#include "Engine.h"

Engine *Engine::instance = nullptr;

Engine::~Engine() {
    FURI_LOG_D("FlipperGameEngine", "Stopping render thread");
    if (loaded) {
        if (buffer_thread) {
            thread_loop = false;
            furi_thread_join(buffer_thread);
            furi_thread_free(buffer_thread);
        }
        if (notification_app)
            notification_message_block(notification_app, &sequence_display_backlight_enforce_auto);

        if (active_scene)
            delete active_scene;
    }

    furi_pubsub_unsubscribe(input, input_subscription);
    canvas = NULL;
    gui_direct_draw_release(gui);
    furi_record_close(RECORD_GUI);
    furi_record_close(RECORD_INPUT_EVENTS);
    furi_mutex_free(render_mutex);
}

void Engine::Stop() {
    processing = false;
    release(instance);
}

Engine::Engine(const char *name, uint8_t f, bool alwaysOn) {
    instance = this;
    app_name = name;
    fps = f;
    for (uint8_t i = 0; i < 7; i++)
        input_states[i] = InputUp;


    input = (FuriPubSub *) furi_record_open(RECORD_INPUT_EVENTS);
    gui = (Gui *) furi_record_open(RECORD_GUI);
    canvas = gui_direct_draw_acquire(gui);
    input_subscription = furi_pubsub_subscribe(instance->input, &input_callback, this);

    buffer = Buffer(canvas, gui);
    render_mutex = (FuriMutex *) furi_mutex_alloc(FuriMutexTypeNormal);

    if (!render_mutex) {
        FURI_LOG_E("FlipperGameEngine", "Cannot create mutex!");
        loaded = false;
        return;
    }

    active_scene = nullptr;
    notification_app = (NotificationApp *) furi_record_open(RECORD_NOTIFICATION);
    if (alwaysOn)
        notification_message_block(notification_app, &sequence_display_backlight_enforce_on);

    buffer_thread = furi_thread_alloc_ex(
            "BackBufferThread", 1024, render_thread, render_info);
    loaded = true;
    processing = true;
}

void Engine::input_callback(const void *value, void *ctx) {
    auto *inst = (Engine *) ctx;
    const auto *event = (const InputEvent *) value;

    if (event->key == InputKeyBack && event->type == InputTypeLong)
        inst->Stop();

    if (event->type == InputTypePress)
        inst->input_states[event->key] = InputPress;
    else if (event->type == InputTypeLong)
        inst->input_states[event->key] = InputDown;
    else if (event->type == InputTypeRelease)
        inst->input_states[event->key] = InputRelease;

}

void Engine::Start() {
    if (!active_scene) {
        Stop();
        return;
    }

    furi_thread_start(buffer_thread);
    active_scene->Start();
    vTaskPrioritySet(static_cast<TaskHandle_t>(furi_thread_get_current_id()), FuriThreadPriorityHigh);
    last_tick = furi_get_tick();

    while (processing) {
        buffer.reset();
        uint32_t tick = furi_get_tick();
        delta_tick = (tick - last_tick) / furi_kernel_get_tick_frequency();
        active_scene->Update(delta_tick);

//        active_scene->ProcessPhysics(tick);
        buffer.render();

        for (int i = 0; i < 7; i++)
            if (input_states[i] == InputRelease)
                input_states[i] = InputUp;
        furi_thread_yield();
    }

}

void Engine::SetScene(Scene *scene) {
    if (active_scene)
        delete active_scene;

    active_scene = scene;
    if (loaded) {
        active_scene->Start();
    }
}


int32_t Engine::render_thread(void *ctx) {
    return 0;
}
