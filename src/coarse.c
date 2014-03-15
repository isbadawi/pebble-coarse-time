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

static language_t get_language_from_config(void) {
  if (!persist_exists(CONFIG_KEY_LANGUAGE)) {
    return LANGUAGE_ENGLISH;
  }
  return persist_read_int(CONFIG_KEY_LANGUAGE);
}

static void update_time(struct tm* time) {
  language_t language = get_language_from_config();
  text_layer_set_text(text_layer, get_time_label(time->tm_hour, language));
}

static void draw_current_time(void) {
  time_t now = time(NULL);
  struct tm *t = localtime(&now);
  update_time(t);
}

static void handle_hour_tick(struct tm *tick_time, TimeUnits units_changed) {
  update_time(tick_time);
}

static void in_received_handler(DictionaryIterator *iter, void *context) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "App message received.");
  Tuple *config_language_tuple = dict_find(iter, CONFIG_KEY_LANGUAGE);

  if (config_language_tuple) {
    uint32_t language = config_language_tuple->value->int32;
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Config language received: %lu", language);
    APP_LOG(APP_LOG_LEVEL_DEBUG, "About to persist!");
    persist_write_int(CONFIG_KEY_LANGUAGE, language);
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Going to refresh screen!");
    draw_current_time();
  }
}

static void in_dropped_handler(AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "App message dropped! Error code: %d", reason);
}

static void app_message_init(void) {
  app_message_register_inbox_received(in_received_handler);
  app_message_register_inbox_dropped(in_dropped_handler);
  app_message_open(64, 64);
}

static TextLayer *create_time_layer(GRect bounds) {
  TextLayer* text_layer = text_layer_create((GRect) {
    .origin = { 0, 54 },
    .size = { bounds.size.w, 72 }
  });
  text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
  text_layer_set_background_color(text_layer, GColorBlack);
  text_layer_set_text_color(text_layer, GColorWhite);
  text_layer_set_font(text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28));
  return text_layer;
};

static void init(void) {
  app_message_init();
  window = window_create();
  window_set_background_color(window, GColorBlack);
  Layer *window_layer = window_get_root_layer(window);

  text_layer = create_time_layer(layer_get_bounds(window_layer));
  layer_add_child(window_layer, text_layer_get_layer(text_layer));

  draw_current_time();

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
