#include <pebble.h>

typedef enum {
  CONFIG_KEY_LANGUAGE,
  NUM_CONFIG_KEYS
} config_key_t;

typedef enum {
  LANGUAGE_ENGLISH,
  LANGUAGE_FRENCH,
  NUM_LANGUAGES
} language_t;

typedef enum {
  PERIOD_NIGHT,
  PERIOD_MORNING,
  PERIOD_AFTERNOON,
  PERIOD_EVENING,
  NUM_PERIODS
} period_t;

static Window *window;
static TextLayer *text_layer;

static const char* TIME_LABELS[NUM_LANGUAGES][NUM_PERIODS] = {
  // English
  { "Night", "Morning", "Afternoon", "Evening" },
  // French
  { "Nuit", "Matin", "Apres-midi", "Soir" },
};

static const char* get_time_label(int hour, language_t language) {

  if (21 <= hour || hour <= 4) {
    return TIME_LABELS[language][PERIOD_NIGHT];
  }
  if (5 <= hour && hour <= 11) {
    return TIME_LABELS[language][PERIOD_MORNING];
  }
  if (12 <= hour && hour <= 16) {
    return TIME_LABELS[language][PERIOD_AFTERNOON];
  }
  if (17 <= hour && hour <= 20) {
    return TIME_LABELS[language][PERIOD_EVENING];
  }
  return NULL;
}

static void update_time(struct tm* time) {
  text_layer_set_text(text_layer, get_time_label(time->tm_hour, LANGUAGE_ENGLISH));
}

static void handle_hour_tick(struct tm *tick_time, TimeUnits units_changed) {
  update_time(tick_time);
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
