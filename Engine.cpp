#include "Engine.h"
#include "types/Sprite.h"
#include <notification/notification_messages.h>
#include "Helpers.h"
#include <dolphin/dolphin.h>

Engine *Engine::instance = nullptr;

Engine::~Engine() {
    LOG_D("Stopping render thread");
    check_pointer(active_scene);
    if (active_scene)
        delete active_scene;

    auto *s = sprite_map.start;

    while (s) {
        auto *t = s->next;
        if (s->data) {
            if (s->data->sprite) {
                delete s->data->sprite;
            }
            delete s->data;
        }
        delete s;
        s = t;
    }

    if (loaded) {
        if (buffer_thread_ptr) {
            thread_loop = false;
            furi_thread_join(buffer_thread_ptr);
            furi_thread_free(buffer_thread_ptr);
        }
        if (physics_thread_ptr) {
            physics_loop = false;
            furi_thread_join(physics_thread_ptr);
            furi_thread_free(physics_thread_ptr);
        }
        if (notification_app)
            notification_message_block(notification_app, &sequence_display_backlight_enforce_auto);

    }
    check_pointer(buffer);
    if (buffer)
        delete buffer;

    furi_pubsub_unsubscribe(input, input_subscription);
    canvas = NULL;
    gui_direct_draw_release(gui);
    furi_record_close(RECORD_GUI);
    furi_record_close(RECORD_INPUT_EVENTS);
    furi_mutex_free(render_mutex);
}

void Engine::Stop() {
    processing = false;
    thread_loop = false;
    check_pointer(instance);
//    delete instance;
}

Engine::Engine(const char *name, uint8_t f, bool alwaysOn) {
    APP_NAME = name;
    LOG_D("Starting %s", name);
    if (instance)
        LOG_E("Only one engine instance can be present!");

    instance = this;
    app_name = name;
    fps = f;
    for (uint8_t i = 0; i < 7; i++)
        input_states[i] = InputUp;


    input = (FuriPubSub *) furi_record_open(RECORD_INPUT_EVENTS);
    gui = (Gui *) furi_record_open(RECORD_GUI);
    canvas = gui_direct_draw_acquire(gui);
    input_subscription = furi_pubsub_subscribe(instance->input, &input_callback, this);

    buffer = new RenderBuffer(canvas, true);
    render_mutex = (FuriMutex *) furi_mutex_alloc(FuriMutexTypeNormal);

    if (!render_mutex) {
        loaded = false;
        return;
    }

    active_scene = nullptr;
    notification_app = (NotificationApp *) furi_record_open(RECORD_NOTIFICATION);
    if (alwaysOn)
        notification_message_block(notification_app, &sequence_display_backlight_enforce_on);

    buffer_thread_ptr = furi_thread_alloc_ex(
            "BackBufferThread", 1024, render_thread, buffer);
    physics_thread_ptr = furi_thread_alloc_ex(
            "PhysicsThread", 1024, physics_thread, this);
    loaded = true;
    physics_loop = true;
    processing = true;
}

void Engine::input_callback(const void *value, void *ctx) {
    auto *inst = (Engine *) ctx;
    const auto *event = (const InputEvent *) value;

    if (event->key == InputKeyBack && event->type == InputTypeLong)
        inst->Stop();
    InputState type = InputUp;

    if (event->type == InputTypePress)
        type = InputPress;
    else if (event->type == InputTypeLong)
        type = InputDown;
    else if (event->type == InputTypeRelease)
        type = InputRelease;
    OnInput(inst, event->key, type);

}

void Engine::Start() {
    LOG_D("loop start");
    DOLPHIN_DEED(DolphinDeedPluginGameStart);

    if (!active_scene) {
        LOG_D("loop exit");
        Stop();
        return;
    }
    thread_loop = true;

    furi_thread_start(buffer_thread_ptr);
    furi_thread_start(physics_thread_ptr);
    active_scene->Start();
    vTaskPrioritySet(static_cast<TaskHandle_t>(furi_thread_get_current_id()), FuriThreadPriorityIdle);
    last_tick = furi_get_tick();

    while (processing) {
        uint32_t tick = furi_get_tick();
        if ((tick - last_tick) > UPDATE_FPS) {
            buffer->reset();
            delta_tick = (tick - last_tick);
            active_scene->Update((float) delta_tick / 1000.0f, this);
            buffer->render();
            last_tick = tick;
        }

        //        active_scene->ProcessPhysics(tick);

        for (int i = 0; i < 7; i++)
            if (input_states[i] == InputRelease)
                input_states[i] = InputUp;

        furi_thread_yield();
    }

}

void Engine::SetScene(Scene *scene) {
    if (active_scene) {
        check_pointer(active_scene);
        delete active_scene;
    }

    active_scene = scene;
    if (loaded) {
        active_scene->Start();
    }
}


int32_t Engine::render_thread(void *ctx) {
    auto *b = (RenderBuffer *) ctx;
    double tick = furi_get_tick();
    vTaskPrioritySet(static_cast<TaskHandle_t>(furi_thread_get_current_id()), FuriThreadPriorityIdle);
    while (instance->thread_loop) {
        double t = furi_get_tick();
        if ((t - tick) >= 1000 / Engine::instance->fps) {
            b->draw();
            tick = t;
        }
        furi_thread_yield();
    }
    return 0;
}

Sprite *Engine::LoadSprite(Icon *icon) {
    if (!loaded) {
        LOG_E("Load image after engine initialization!");
        return NULL;
    }

    for(auto *sprite : sprite_map){
        if(sprite->icon == icon)
            return sprite->sprite;
    }

    auto *s = new Sprite(icon, BlackOnly);
    s->render(canvas);

    auto *map = new SpriteMap;
    map->icon = icon;
    map->sprite = s;
    sprite_map.add(map);

    return s;
}

void Engine::QueueSprite(Sprite *s, const Matrix &matrix) {
    buffer->add_to_queue(s, matrix);
}

void Engine::OnInput(Engine *inst, InputKey key, InputState type) {
    inst->input_states[key] = type;
    inst->active_scene->OnInput(key, type);
}

InputState Engine::GetInput(InputKey key) {
    return input_states[key];
}

int32_t Engine::physics_thread(void *ctx) {
    UNUSED(ctx);
    vTaskPrioritySet(static_cast<TaskHandle_t>(furi_thread_get_current_id()), FuriThreadPriorityIdle);
    double tick = furi_get_tick(), currTick;
    float delta;
    while (instance->physics_loop) {
        currTick = furi_get_tick();
        delta = (float)(currTick - tick);
        if (delta >= PHYSICS_TICKRATE) {
            instance->active_scene->ProcessPhysics((float) delta / 1000.0f);
            tick = currTick;
        }
        furi_thread_yield();
    }
    return 0;
}
