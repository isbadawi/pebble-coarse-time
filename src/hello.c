#include <pebble.h>

static Window *window;
static TextLayer *text_layer;

static char* get_time_label(int hour) {
  if (21 <= hour || hour <= 4) {
    return "Night";
  }
  if (5 <= hour && hour <= 11) {
    return "Morning";
  }
  if (12 <= hour && hour <= 16) {
    return "Afternoon";
  }
  if (17 <= hour && hour <= 20) {
    return "Evening";
  }
  return NULL;
}

static TextLayer *create_time_layer(GRect bounds) {
  TextLayer* text_layer = text_layer_create((GRect) {
    .origin = { 0, 54 },
    .size = { bounds.size.w, 72 }
  });
  text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
  text_layer_set_background_color(text_layer, GColorBlack);
  text_layer_set_text_color(text_layer, GColorWhite);
  text_layer_set_font(text_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_LIGHT));
  return text_layer;
};

static void update_time(struct tm* time) {
  text_layer_set_text(text_layer, get_time_label(time->tm_hour));
}

static void handle_hour_tick(struct tm *tick_time, TimeUnits units_changed) {
  update_time(tick_time);
}

static void init(void) {
  window = window_create();
  window_set_background_color(window, GColorBlack);
  Layer *window_layer = window_get_root_layer(window);

  text_layer = create_time_layer(layer_get_bounds(window_layer));
  layer_add_child(window_layer, text_layer_get_layer(text_layer));

  time_t now = time(NULL);
  struct tm *t = localtime(&now);
  update_time(t);

  tick_timer_service_subscribe(HOUR_UNIT, handle_hour_tick);
  window_stack_push(window, true);
}

static void deinit(void) {
  text_layer_destroy(text_layer);
  window_destroy(window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
