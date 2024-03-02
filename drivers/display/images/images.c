#define STB_IMAGE_IMPLEMENTATION
#include "stb_images.h"
#include "vesa.h"
#include "stdint.h"
#include "stdio.h"
#include "images.h"
unsigned int *png_parse(unsigned char *ptr, int size);
int draw_image(unsigned char* image_data,int screen_x, int screen_y, int image_width, int image_height)
{
    printf("Img Width = %d,hight = %d",image_width,image_height);
    for (int y = 0; y < image_height; y++) {
        for (int x = 0; x < image_width; x++) {
            // Calculate the pixel index in the image_data
            int pixel_index = (y * image_width + x) * 3; // Assuming RGB image

            // Extract the color channels from image_data
            int r = image_data[pixel_index];
            int g = image_data[pixel_index + 1];
            int b = image_data[pixel_index + 2];

            // Calculate the screen coordinates for drawing
            int screen_x = x; // Adjust this based on where you want to draw the image
            int screen_y = y; // Adjust this based on where you want to draw the image

            // Calculate the color using VBE_RGB macro
            int color = VBE_RGB(r, g, b);

            // Call your vbe_putpixel function to draw the pixel
            vbe_putpixel(screen_x, screen_y, color);
        }
    
}
return 1;
}
int draw_img(const char* filename, int x, int y) {
    int img_width, img_height, img_channels;
    unsigned char* img_data = stbi_load(filename, &img_width, &img_height, &img_channels, 0);
    if (img_data == NULL) {
        fprintf(stderr, "Failed to load image: %s\n", filename);
        return -1;
    }

    // Draw the image using the provided draw_image function
    unsigned char *formated_data = png_parse(img_data,img_width*img_height*32);
    int result = draw_image(formated_data, x, y, img_width, img_height);

    // Free the image data
    stbi_image_free(img_data);

    return result;
}

unsigned int *png_parse(unsigned char *ptr, int size){
    int i, w, h, f;
    unsigned char *img, *p;
    stbi__context s;
    stbi__result_info ri;
    s.read_from_callbacks = 0;
    s.img_buffer = s.img_buffer_original = ptr;
    s.img_buffer_end = s.img_buffer_original_end = ptr + size;
    ri.bits_per_channel = 8;
    img = p = (unsigned char*)stbi__png_load(&s, (int*)&w, (int*)&h, (int*)&f, 1, &ri);
    unsigned int *data = (unsigned int*)malloc((w*h+2)*sizeof(unsigned int));
    if(!data) { free(img); return NULL; }
    // convert the returned image into frame buffer format
    for(i = 0; i < w * h; i++, p += f)
        switch(f) {
            case 1: data[2 + i] = 0xFF000000 | (p[0] << 16) | (p[0] << 8) | p[0]; break;
            case 2: data[2 + i] = (p[1] << 24) | (p[0] << 16) | (p[0] << 8) | p[0]; break;
            case 3: data[2 + i] = 0xFF000000 | (p[0] << 16) | (p[1] << 8) | p[2]; break;
            case 4: data[2 + i] = (p[3] << 24) | (p[0] << 16) | (p[1] << 8) | p[2]; break;
        }
    free(img);
    data[0] = w;
    data[1] = h;
    return data;
}

int draw_owl()
{
    // draw_image(Flying_Owl,0,0,FLYING_OWL_WIDTH,FLYING_OWL_HEIGHT);
}