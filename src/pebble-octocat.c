#include <pebble.h>
#include <pebble_fonts.h>
#include "resource_ids.auto.h"

static Window *window;
static BitmapLayer *octocat_layer;
static TextLayer *time_layer;
static char time_buffer[10];
static GBitmap *octocat_icon;

static void draw_current_time(void) {
  clock_copy_time_string(time_buffer, 10);
  // Strip off the " AM" or " PM" in 12h style.
  if (!clock_is_24h_style()) {
    time_buffer[strlen(time_buffer) - 3] = '\0';
  }
  layer_mark_dirty(text_layer_get_layer(time_layer));
}

static void handle_minute_tick(struct tm *tick_time, TimeUnits units_changed) {
  draw_current_time();
}

static void init(void) {
  window = window_create();
  window_set_background_color(window, GColorBlack);

  octocat_layer = bitmap_layer_create(
      GRect((144 - 120) / 2, -1, 120, 120));
  octocat_icon = gbitmap_create_with_resource(RESOURCE_ID_OCTOCAT);

  bitmap_layer_set_bitmap(octocat_layer, octocat_icon);
  bitmap_layer_set_background_color(octocat_layer, GColorWhite);

  time_layer = text_layer_create(GRect(0, 120 - 5, 144, 168 - 120));
  text_layer_set_background_color(time_layer, GColorBlack);
  text_layer_set_text_color(time_layer, GColorWhite);
  text_layer_set_text_alignment(time_layer, GTextAlignmentCenter);
  text_layer_set_font(time_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_LIGHT));
  text_layer_set_text(time_layer, time_buffer);
  draw_current_time();

  layer_add_child(
      window_get_root_layer(window),
      text_layer_get_layer(time_layer));

  layer_add_child(
      window_get_root_layer(window),
      bitmap_layer_get_layer(octocat_layer));

  tick_timer_service_subscribe(MINUTE_UNIT, handle_minute_tick);

  window_stack_push(window, true /* animated */);
}

static void deinit(void) {
  text_layer_destroy(time_layer);
  gbitmap_destroy(octocat_icon);
  bitmap_layer_destroy(octocat_layer);
  window_destroy(window);
}

int main(void) {
  init();
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", window);
  app_event_loop();
  deinit();
}
