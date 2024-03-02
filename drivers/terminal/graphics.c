#include "vesa.h"


/**
 * Function Name: draw_loading_bar
 * Description: Draws a loading bar with specified parameters.
 *
 * Parameters:
 *   current_value (int) - The current value of the loading bar.
 *   total_value (int) - The total value of the loading bar.
 *   x (int) - The x-coordinate of the loading bar.
 *   y (int) - The y-coordinate of the loading bar.
 *   color (int) - The color of the loading bar.
 *   distance_between_bars (int) - The distance between each loading bar.
 */
void draw_loading_bar(int current_value, int total_value, int x, int y, uint32_t color, int distance_between_bars) {
    // Calculate the width of each segment of the loading bar
    // printf("Drawoing bar\n");
    int segment_width = 10;
    int segment_height = 15;

    // Calculate the total width of the loading bar
    int total_width = total_value * (segment_width + distance_between_bars);

    // Calculate the width of the filled part of the loading bar
    int filled_width = current_value * (segment_width + distance_between_bars);

    // Draw each segment of the loading bar
    for (int i = 0; i < total_value; i++) {
        // Calculate the x-coordinate of the current segment
        int segment_x = x + i * (segment_width + distance_between_bars);

        // Set the color of the segment
        if (i < current_value) {
            // Draw filled segment
            for (int dx = 0; dx < segment_width; dx++) {
                for (int dy = 0; dy < segment_height; dy++) {
                    vbe_putpixel(segment_x + dx, y + dy, color);
                }
            }
        } else {
            // Draw empty segment
            for (int dx = 0; dx < segment_width; dx++) {
                for (int dy = 0; dy < segment_height; dy++) {
                    vbe_putpixel(segment_x + dx, y + dy, 0); // Assuming color 0 represents empty
                }
            }
        }
    }
}
