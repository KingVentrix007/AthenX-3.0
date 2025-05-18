#include <maths.h> // For trigonometric functions
#include "login.h"
#define RADAR_RADIUS 50       // Radius of the radar circle
#define RADAR_SWEEP_SPEED 2   // Degrees to move per update
#define RADAR_MIN_ANGLE -45   // Minimum swing angle
#define RADAR_MAX_ANGLE 45    // Maximum swing angle
#define M_PI PI
// Globals for radar state
int radar_angle = RADAR_MIN_ANGLE; // Current angle of the radar sweep
int radar_direction = 1;           // 1 = clockwise, -1 = counterclockwise

void draw_circle(int cx, int cy, int radius, int color);
void draw_line(int x0, int y0, int x1, int y1, int color);

// Draw the radar dish in the score box
void draw_radar_dish(int center_x, int center_y) {
    // Draw the radar circle
    draw_circle(center_x, center_y, RADAR_RADIUS, COLOR_TEXT);

    // Draw a static base or outline for aesthetics
    draw_line(center_x - 10, center_y + RADAR_RADIUS + 5, center_x + 10, center_y + RADAR_RADIUS + 5, COLOR_TEXT);
}

// Update the radar sweep
void update_radar(int center_x, int center_y) {
    static int prev_x = 0, prev_y = 0; // Previous line endpoint

    // Calculate the new line endpoint based on radar_angle
    double radians = radar_angle * (M_PI / 180.0); // Convert degrees to radians
    int new_x = center_x + (int)(RADAR_RADIUS * cos(radians));
    int new_y = center_y + (int)(RADAR_RADIUS * sin(radians));

    // Erase the previous sweep line (using background color)
    if (prev_x && prev_y) {
        draw_line(center_x, center_y, prev_x, prev_y, COLOR_BG);
    }

    // Draw the new sweep line
    draw_line(center_x, center_y, new_x, new_y, COLOR_TEXT);

    // Update the previous endpoint
    prev_x = new_x;
    prev_y = new_y;

    // Update the radar angle for the next frame
    radar_angle += radar_direction * RADAR_SWEEP_SPEED;

    // Reverse direction at swing limits
    if (radar_angle >= RADAR_MAX_ANGLE || radar_angle <= RADAR_MIN_ANGLE) {
        radar_direction *= -1;
    }
}

// Draw a circle (Bresenham's algorithm or midpoint circle algorithm)
void draw_circle(int cx, int cy, int radius, int color) {
    int x = 0, y = radius;
    int d = 3 - 2 * radius;

    while (y >= x) {
        vbe_putpixel(cx + x, cy + y, color);
        vbe_putpixel(cx - x, cy + y, color);
        vbe_putpixel(cx + x, cy - y, color);
        vbe_putpixel(cx - x, cy - y, color);
        vbe_putpixel(cx + y, cy + x, color);
        vbe_putpixel(cx - y, cy + x, color);
        vbe_putpixel(cx + y, cy - x, color);
        vbe_putpixel(cx - y, cy - x, color);
        if (d < 0) {
            d = d + 4 * x + 6;
        } else {
            d = d + 4 * (x - y) + 10;
            y--;
        }
        x++;
    }
}

// Draw a line (Bresenham's line algorithm)
void draw_line(int x0, int y0, int x1, int y1, int color) {
    int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = dx + dy, e2;

    while (1) {
        vbe_putpixel(x0, y0, color);
        if (x0 == x1 && y0 == y1) break;
        e2 = 2 * err;
        if (e2 >= dy) { err += dy; x0 += sx; }
        if (e2 <= dx) { err += dx; y0 += sy; }
    }
}
