#include "game_engine.h"
#include "util/graphics.h"
#include "util/util.h"

EngineState *engineState;
size_t start_heap=0;


void update_tree(List *items, RenderQueue *render_state, Vector s);

static void update(RenderQueue *queue) {
    queue->render_count = 0;
    if(engineState->scene)
        update_tree(engineState->scene->entities, queue, (Vector){1,1});
}

static void render(Canvas *const canvas, void *ctx) {

    const RenderQueue *queue = acquire_mutex((ValueMutex *) ctx, 25);
    if (queue == NULL) {
        return;
    }
    for (int i = 0; i < queue->render_count; i++) {
        RenderInfo s = queue->render_list[i];
        if (s.image == NULL) {
            FURI_LOG_E("FlipperGameEngine", "Image render failed because sprite is null (ADDR: %p)", s.image);
            continue;
        }

        if(s.image->asset->loaded==false)
            render_sprite(canvas, s.image);

//        draw_buffer_scaled(canvas, s.position, s.image, s.render_scale);
        draw_buffer_scaled(canvas, s.position, s.image, s.render_scale);
//        canvas_draw_bitmap(canvas, s.position.x+100-s.image->size.x/2, s.position.y-s.image->size.y/2, s.image->size.x, s.image->size.y, s.image->data);
//        draw_pixels(canvas, s.image->data, s.position.x, s.position.y, s.image->size.x, s.image->size.y, Default);
    }
    release_mutex((ValueMutex *) ctx, queue);
}


static void handle_input(InputEvent *input_event, FuriMessageQueue *event_queue) {
    furi_assert(event_queue);
    AppEvent event = {.type = EventInput, .inputEvent = *input_event};
    furi_message_queue_put(event_queue, &event, FuriWaitForever);
}

static void update_timer(FuriMessageQueue *event_queue) {
    furi_assert(event_queue);
    AppEvent event = {.type = EventTick};
    furi_message_queue_put(event_queue, &event, 0);
}

int32_t setup_engine(SetupState state) {
    start_heap=memmgr_get_free_heap();
    FURI_LOG_I("FlipperGameEngine", "Starting engine");
    engineState = allocate(sizeof(EngineState));
    engineState->app_name = state.app_name;
    engineState->event_queue = furi_message_queue_alloc(8, sizeof(AppEvent));

    engineState->game_state = allocate(state.state_size);
    engineState->render_queue = allocate(sizeof(RenderQueue));
    engineState->render_queue->render_count = 0;
    state.init_state(engineState->game_state);
    if (!init_mutex(&(engineState->render_mutex), engineState->render_queue, sizeof(RenderQueue))) {
        FURI_LOG_E("FlipperGameEngine", "Cannot create mutex!");
        engineState->loaded = false;
        return 255;
    }


    engineState->scene = NULL;
    engineState->notification_app = furi_record_open(RECORD_NOTIFICATION);
    if (state.always_on_screen)
        notification_message_block(engineState->notification_app, &sequence_display_backlight_enforce_on);

    engineState->viewport = view_port_alloc();
    view_port_draw_callback_set(engineState->viewport, render, &(engineState->render_mutex));
    view_port_input_callback_set(engineState->viewport, handle_input, (engineState->event_queue));

    engineState->timer = furi_timer_alloc(update_timer, FuriTimerTypePeriodic, (engineState->event_queue));

    furi_timer_start(engineState->timer, furi_kernel_get_tick_frequency() / state.tick_rate);

    engineState->gui = furi_record_open("gui");

    gui_add_view_port(engineState->gui, engineState->viewport, GuiLayerFullscreen);

    engineState->loaded = true;

    return 0;
}

void cleanup() {
    FURI_LOG_I("FlipperGameEngine", "Cleaning up data");
    if (engineState->loaded) {
        if(engineState->scene)
            clear_scene(engineState->scene);
        notification_message_block(engineState->notification_app, &sequence_display_backlight_enforce_auto);
        furi_timer_free(engineState->timer);
        view_port_enabled_set(engineState->viewport, false);
        gui_remove_view_port(engineState->gui, engineState->viewport);
        furi_record_close(RECORD_GUI);
        furi_record_close(RECORD_NOTIFICATION);
        view_port_free(engineState->viewport);
        delete_mutex(&(engineState->render_mutex));
        clear_image_assets();
    }

    if (engineState->prev_frame != NULL)
        release(engineState->prev_frame);
    release(engineState->render_queue);
    release(engineState->game_state);
    furi_message_queue_free(engineState->event_queue);
    release(engineState);
}

void start_loop() {
    FURI_LOG_I("FlipperGameEngine", "Loop start");
    if (!engineState->loaded) {
        FURI_LOG_E("FlipperGameEngine", "Cannot start game, load failed!");
        return;
    }
    engineState->render_queue->render_count = 0;

    AppEvent event;
    engineState->processing = true;

    FURI_LOG_I("FlipperGameEngine", "Entering main loop");
    while (engineState->processing) {

        FuriStatus event_status = furi_message_queue_get(engineState->event_queue, &event, 25);

        RenderQueue *render_state = (RenderQueue *) acquire_mutex_block(&(engineState->render_mutex));
        unsigned int tick = furi_get_tick();


        engineState->tick_delta = tick - engineState->last_tick;
        engineState->last_tick = tick;
        if (render_state == NULL) {
            FURI_LOG_E("FlipperGameEngine", "render state mutex failed");
            continue;
        }
        if (event_status == FuriStatusOk) {

            if (event.type == EventInput) {
                if (event.inputEvent.key == InputKeyBack && event.inputEvent.type == InputTypeLong)
                    engineState->processing = false;

                engineState->lastInput.inputType = event.inputEvent.type;
                engineState->lastInput.inputKey = event.inputEvent.key;
                if (event.inputEvent.type == InputTypePress) {
                    engineState->inputState[event.inputEvent.key] = InputDown;
                } else if (event.inputEvent.type == InputTypeLong) {
                    engineState->inputState[event.inputEvent.key] = InputPress;
                } else if (event.inputEvent.type == InputTypeRelease) {
                    engineState->inputState[event.inputEvent.key] = InputUp;
                }
            }
        } else if (event.type == EventTick) {
            update(render_state);
            engineState->lastInput.inputKey = InputKeyMAX;
            engineState->lastInput.inputType = InputTypeMAX;
        }/* else {
            FURI_LOG_D("FlipperGameEngine", "osMessageQueue: event timeout");
        }*/
        if (engineState->render_queue->render_count > 0)
            view_port_update(engineState->viewport);
        release_mutex(&(engineState->render_mutex), render_state);
    }
    FURI_LOG_I("FlipperGameEngine", "Exiting main loop");
    cleanup();
    if((start_heap-memmgr_get_free_heap()) != 0)
        FURI_LOG_E("FlipperGameEngine", "Possible memory leak! Diff %zu", start_heap - memmgr_get_free_heap());
}

void init_tree(List *items) {
    t_ListItem *curr = items->start;
    if (!curr) return;
    while (curr) {
        entity_t *e = (entity_t *) curr->data;
        if (e->enabled) {
            t_ListItem *component = e->components->start;
            while (component) {
                component_t *c = (component_t *) component->data;
                c->start(&(c->componentInfo), engineState->game_state);
                component = component->next;
            }
            init_tree(e->transform.children);
        }
        curr = curr->next;
    }
}

void set_scene(Scene *s) {
    FURI_LOG_I("FlipperGameEngine", "Opening scene: %s", s->name);
    if (engineState->scene)
        clear_scene(engineState->scene);
    engineState->scene = s;

    FURI_LOG_I("FlipperGameEngine", "Initializing entities");
    init_tree(s->entities);

}

void update_tree(List *items, RenderQueue *render_state, Vector scale) {

    t_ListItem *curr = items->start;
    if (!curr) return;
    while (curr) {
        entity_t *e = (entity_t *) curr->data;
        Vector new_scale=vector_mul_components(scale, e->transform.scale);
        if (e->enabled) {
            t_ListItem *component = e->components->start;
            while (component) {
                component_t *c = (component_t *) component->data;
                c->update(&(c->componentInfo), engineState->game_state);
                component = component->next;
            }
            if (e->transform.dirty) {
                update_transform(&(e->transform));
            }
            update_tree(e->transform.children, render_state, new_scale);

            if (e->draw) {
                if (render_state->render_count < 63) {
                    render_state->render_list[render_state->render_count].image = &(e->sprite);
                    render_state->render_list[render_state->render_count].render_scale = new_scale;
                    render_state->render_list[render_state->render_count].position = get_matrix_translation(&(e->transform.modelMatrix));
                    render_state->render_count++;
                }
            }
        }

        curr = curr->next;
    }
}

void exit_app() {
    engineState->processing = false;
}

unsigned int delta_tick() {
    return engineState->tick_delta / furi_kernel_get_tick_frequency();
}

unsigned int frame_tick() {
    return engineState->last_tick;
}
