#include <pebble.h>

// Sakta: a 24-hour one-hand watchface for Pebble Time 2 (emery, 200x228),
// inspired by the slow Jo 17. Cream dial, numerals 0-23 along the edge with
// 12 at the top and 0 at the bottom, quarter-hour ticks inside the numerals,
// and one thin grey hand that turns once a day.

static Window *s_window;
static Layer  *s_canvas_layer;

// Noon = top = 0; midnight = bottom.
static int32_t minutes_to_angle(int local_min) {
  int shifted = (local_min + 12 * 60) % (24 * 60);
  return (int32_t)((int64_t)TRIG_MAX_ANGLE * shifted / (24 * 60));
}

static GPoint polar(GPoint center, int r, int32_t angle) {
  return GPoint(
    center.x + (r * sin_lookup(angle)) / TRIG_MAX_RATIO,
    center.y - (r * cos_lookup(angle)) / TRIG_MAX_RATIO
  );
}

static void canvas_update_proc(Layer *layer, GContext *ctx) {
  GRect  bounds = layer_get_bounds(layer);
  int    size   = bounds.size.w < bounds.size.h ? bounds.size.w : bounds.size.h;
  int    radius = size / 2;
  GPoint center = GPoint(bounds.size.w / 2, bounds.size.h / 2);

  time_t     now   = time(NULL);
  struct tm *local = localtime(&now);

  // Cream dial over the whole screen.
  graphics_context_set_fill_color(ctx, GColorPastelYellow);
  graphics_fill_rect(ctx, bounds, 0, GCornerNone);

  // Numerals 0-23 along the edge.
  GFont font = fonts_get_system_font(FONT_KEY_GOTHIC_14);
  graphics_context_set_text_color(ctx, GColorBlack);
  for (int h = 0; h < 24; h++) {
    GPoint pos = polar(center, radius - 9, minutes_to_angle(h * 60));
    char num_str[3];
    snprintf(num_str, sizeof(num_str), "%d", h);
    graphics_draw_text(ctx, num_str, font, GRect(pos.x - 10, pos.y - 9, 20, 16),
                       GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
  }

  // Quarter-hour ticks inside the numerals: long on the hour, medium on the
  // half hour, short on the quarters.
  int tick_outer = radius - 19;
  for (int i = 0; i < 96; i++) {
    int32_t angle = minutes_to_angle(i * 15);
    bool on_hour = (i % 4 == 0);
    bool on_half = (i % 2 == 0);
    int  len     = on_hour ? 13 : on_half ? 9 : 5;
    graphics_context_set_stroke_color(ctx, GColorBlack);
    graphics_context_set_stroke_width(ctx, on_hour ? 2 : 1);
    graphics_draw_line(ctx, polar(center, tick_outer, angle),
                            polar(center, tick_outer - len, angle));
  }

  // Thin tapered grey hand with a short tail and a round hub.
  int32_t angle = minutes_to_angle(local->tm_hour * 60 + local->tm_min);
  int32_t perp  = angle + TRIG_MAX_ANGLE / 4;
  GPoint tip    = polar(center, tick_outer - 2, angle);
  GPoint base_l = polar(center, 3, perp);
  GPoint base_r = polar(center, -3, perp);
  GPoint tail   = polar(center, -16, angle);
  GPoint pts[]  = { tip, base_l, tail, base_r };
  GPath *hand   = gpath_create(&(GPathInfo){ .num_points = 4, .points = pts });
  graphics_context_set_fill_color(ctx, GColorDarkGray);
  gpath_draw_filled(ctx, hand);
  gpath_destroy(hand);

  graphics_context_set_stroke_color(ctx, GColorDarkGray);
  graphics_context_set_stroke_width(ctx, 1);
  graphics_draw_line(ctx, center, tip);

  graphics_context_set_fill_color(ctx, GColorDarkGray);
  graphics_fill_circle(ctx, center, 7);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  layer_mark_dirty(s_canvas_layer);
}

static void prv_window_load(Window *window) {
  Layer *root = window_get_root_layer(window);
  s_canvas_layer = layer_create(layer_get_bounds(root));
  layer_set_update_proc(s_canvas_layer, canvas_update_proc);
  layer_add_child(root, s_canvas_layer);
}

static void prv_window_unload(Window *window) {
  layer_destroy(s_canvas_layer);
}

static void prv_init(void) {
  s_window = window_create();
  window_set_window_handlers(s_window, (WindowHandlers) {
    .load   = prv_window_load,
    .unload = prv_window_unload,
  });
  window_stack_push(s_window, true);
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
}

static void prv_deinit(void) {
  tick_timer_service_unsubscribe();
  window_destroy(s_window);
}

int main(void) {
  prv_init();
  app_event_loop();
  prv_deinit();
}
