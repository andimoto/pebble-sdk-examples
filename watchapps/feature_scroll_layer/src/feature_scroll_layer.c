#include "pebble.h"

static Window *s_main_window;

static TextLayer *s_bg_text_layer;

// This is a scroll layer
static ScrollLayer *s_scroll_layer;

static Layer *s_rect_layer;

// We also use a text layer to scroll in the scroll layer
static TextLayer *s_text_layer;

// The scroll layer can other things in it such as an invert layer
// static InverterLayer *s_inverter_layer;

// Lorum ipsum to have something to scroll
// static char s_scroll_text[] = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nam quam tellus, fermentu  m quis vulputate quis, vestibulum interdum sapien. Vestibulum lobortis pellentesque pretium. Quisque ultricies purus e  u orci convallis lacinia. Cras a urna mi. Donec convallis ante id dui dapibus nec ullamcorper erat egestas. Aenean a m  auris a sapien commodo lacinia. Sed posuere mi vel risus congue ornare. Curabitur leo nisi, euismod ut pellentesque se  d, suscipit sit amet lorem. Aliquam eget sem vitae sem aliquam ornare. In sem sapien, imperdiet eget pharetra a, lacin  ia ac justo. Suspendisse at ante nec felis facilisis eleifend.";
static char s_scroll_text[] = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nam quam tellus, fermentu  m quis vulputate quis, vestibulum interdum sapien.";

static void rect_layer_update_callback(Layer *me, GContext *ctx) {
  GRect frame = layer_get_frame(me);

  graphics_context_set_fill_color(ctx, GColorWhite);
  graphics_fill_rect(
    ctx,
    GRect(0,0,frame.size.w, frame.size.h),
    4, GCornersAll
    );
}

// Setup the scroll layer on window load
// We do this here in order to be able to get the max used text size
static void main_window_load(Window *window) {
  window_set_background_color(window, GColorBlack);

  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_frame(window_layer);

  // add bg text
  s_bg_text_layer = text_layer_create(bounds);
  text_layer_set_text(s_bg_text_layer, s_scroll_text);
  text_layer_set_text_color(s_bg_text_layer, GColorWhite);
  text_layer_set_background_color(s_bg_text_layer, GColorBlack);
  text_layer_set_font(s_bg_text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD));

  int padding = 8;

  // GRect rect_layer_bounds = GRect(0, bounds.size.h/2, bounds.size.w, 2000);
  // GRect max_text_bounds = GRect(padding, padding + bounds.size.h/2, bounds.size.w - 2*padding, 2000);

  GRect rect_layer_bounds = GRect(0, bounds.size.h/2, bounds.size.w, 2000);
  GRect max_text_bounds = GRect(padding, padding + bounds.size.h/2, bounds.size.w - 2*padding, 2000);

  // Initialize the scroll layer
  s_scroll_layer = scroll_layer_create(bounds);

  // This binds the scroll layer to the window so that up and down map to scrolling
  // You may use scroll_layer_set_callbacks to add or override interactivity
  scroll_layer_set_click_config_onto_window(s_scroll_layer, window);

  // rect layer
  s_rect_layer = layer_create(bounds);
  layer_set_update_proc(s_rect_layer, rect_layer_update_callback);

  // Initialize the text layer
  s_text_layer = text_layer_create(max_text_bounds);
  text_layer_set_text(s_text_layer, s_scroll_text);
  text_layer_set_background_color(s_text_layer, GColorClear);

  // Change the font to a nice readable one
  // This is system font; you can inspect pebble_fonts.h for all system fonts
  // or you can take a look at feature_custom_font to add your own font
  text_layer_set_font(s_text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));

  // Trim text layer and scroll content to fit text box
  GSize max_size = text_layer_get_content_size(s_text_layer);
  text_layer_set_size(s_text_layer, max_size);
  layer_set_frame(s_rect_layer, GRect(rect_layer_bounds.origin.x, rect_layer_bounds.origin.y, bounds.size.w, max_size.h + 2 * padding));
  scroll_layer_set_content_size(s_scroll_layer, GSize(bounds.size.w, max_text_bounds.origin.y + max_size.h + 2 * padding));

  // Add the layers for display
  // layer_add_child(s_rect_layer, text_layer_get_layer(s_text_layer));
  scroll_layer_add_child(s_scroll_layer, s_rect_layer);
  scroll_layer_add_child(s_scroll_layer, text_layer_get_layer(s_text_layer));

  // The inverter layer will highlight some text
  // s_inverter_layer = inverter_layer_create(GRect(0, 40 + bounds.size.h / 2 + padding, bounds.size.w, 28));
  // scroll_layer_add_child(s_scroll_layer, inverter_layer_get_layer(s_inverter_layer));

  layer_add_child(window_layer, text_layer_get_layer(s_bg_text_layer));
  layer_add_child(window_layer, scroll_layer_get_layer(s_scroll_layer));

}

static void main_window_unload(Window *window) {
  // inverter_layer_destroy(s_inverter_layer);
  text_layer_destroy(s_bg_text_layer);
  text_layer_destroy(s_text_layer);
  scroll_layer_destroy(s_scroll_layer);
}

static void init() {
  s_main_window = window_create();
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload,
  });
  window_stack_push(s_main_window, true);
}

static void deinit() {
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
