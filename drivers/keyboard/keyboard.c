#include "keyboard.h"
#include "stdbool.h"
#include "isr.h"
#include "stream.h"
#include "io_ports.h"
arrow_callback_t arrow_callbacks[256]; // Array to hold callbacks for scan codes (assuming scan codes are 8-bit)
extern bool io_stream_active;
static bool g_caps_lock = false;
static bool g_shift_pressed = false;
char *g_ch = 0, g_scan_code = 0;
char g_scan_code_chars[128] = {
    0, 27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0,
    '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0, '*', 0, ' ',
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '-', 0, 0, 0, '+', 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0
};

static int get_scancode() {
    int i, scancode = 0;

    // get scancode until status is on(key pressed)
    for (i = 1000; i > 0; i++) {
        // Check if scan code is ready
        if ((inportb(KEYBOARD_STATUS_PORT) & 1) == 0) continue;
        // Get the scan code
        scancode = inportb(KEYBOARD_DATA_PORT);
        break;
    }
    if (i > 0)
        return scancode;
    return 0;
}
char alternate_chars(char ch) {
    switch(ch) {
        case '`': return '~';
        case '1': return '!';
        case '2': return '@';
        case '3': return '#';
        case '4': return '$';
        case '5': return '%';
        case '6': return '^';
        case '7': return '&';
        case '8': return '*';
        case '9': return '(';
        case '0': return ')';
        case '-': return '_';
        case '=': return '+';
        case '[': return '{';
        case ']': return '}';
        case '\\': return '|';
        case ';': return ':';
        case '\'': return '\"';
        case ',': return '<';
        case '.': return '>';
        case '/': return '?';
        default: return ch;
    }
}

void keyboard_handler(REGISTERS *r) {
    dbgprintf("Handler got keyboard called\n");
    int scancode;
    // printf("called keyboard handler\n");
    g_ch = 0;
    scancode = get_scancode();
    g_scan_code = scancode;
    if (scancode & 0x80) {
        // key release
    } else {
        // key down
        switch(scancode) {
            case SCAN_CODE_KEY_CAPS_LOCK:
                if (g_caps_lock == false)
                    g_caps_lock = true;
                else
                    g_caps_lock = false;
                break;

            case SCAN_CODE_KEY_ENTER:
                g_ch = '\n';
                break;

            case SCAN_CODE_KEY_TAB:
                g_ch = '\t';
                break;

            case SCAN_CODE_KEY_LEFT_SHIFT:
                g_shift_pressed = true;
                break;
            case 0x48: // Up Arrow
                // scroll_up();
                arrow_press(SCAN_CODE_KEY_UP);
                g_ch = -1;//SCAN_CODE_KEY_UP;
                break;
            case 0x50: // Down Arrow
                // scroll_down();
                // g_ch = SCAN_CODE_KEY_DOWN;
                g_ch = -1;//SCAN_CODE_KEY_UP;

                arrow_press(SCAN_CODE_KEY_DOWN);

                break;
            case 0x4D: // Right Arrow
                arrow_press(SCAN_CODE_KEY_RIGHT);
                g_ch =  -1;
                break;
            case 0x4B: // Left Arrow
                arrow_press(SCAN_CODE_KEY_LEFT);
                g_ch = -1;
                break;
            case 0x3B: // F1
                handle_F1_press(scancode);
                g_ch = -1;
                break;
            case 0x3C: // F2
                handle_F2_press(scancode);
                g_ch = -1;
                break;
            case 0x3D: // F3
                handle_F3_press(scancode);
                g_ch = -1;
                break;
            case 0x3E: // F4
                handle_F4_press(scancode);
                g_ch = -1;
                break;
            case 0x3F: // F5
                handle_F5_press(scancode);
                g_ch = -1;
                break;
            case 0x40: // F6
                handle_F6_press(scancode);
                g_ch = -1;
                break;
            case 0x41: // F7
                handle_F7_press(scancode);
                g_ch = -1;
                break;
            case 0x42: // F8
                handle_F8_press(scancode);
                g_ch = -1;
                break;
            case 0x43: // F9
                handle_F9_press(scancode);
                g_ch = -1;
                break;
            case 0x44: // F10
                handle_F10_press(scancode);
                g_ch = -1;
                break;
            case 0x57: // F11
                handle_F11_press(scancode);
                g_ch = -1;
                break;
            case 0x58: // F12
                handle_F12_press(scancode);
                g_ch = -1;
                break;
            case 0x37:
                printf("Print screen\n");
                g_ch = -1;
                break;
            default:
                g_ch = g_scan_code_chars[scancode];
                // if caps in on, covert to upper
                if (g_caps_lock) {
                    // if shift is pressed before
                    if (g_shift_pressed) {
                        // replace alternate chars
                        g_ch = alternate_chars(g_ch);
                    } else
                        g_ch = upper(g_ch);
                } else {
                    if (g_shift_pressed) {
                        if (isalpha(g_ch))
                            g_ch = upper(g_ch);
                        else 
                            // replace alternate chars
                        g_ch = alternate_chars(g_ch);
                    } else
                        g_ch = g_scan_code_chars[scancode];
                    g_shift_pressed = false;
                }
                break;
        }

    }
    if(g_ch > 0)
    {
        if(io_stream_active == true)
        {
            push_io(g_ch);
        }
        
        // reset_poss_from_scroll();
    }
    // kb_buffer.buffer_pos++; 
    // kb_buffer.buffer[kb_buffer.buffer_pos] = (int)g_ch;
    // //printf("%c",kb_buffer.buffer[kb_buffer.buffer_pos]);
    
    
}
char* kb_getchar_w() {
    int count = 0;
    // printf("HERE\n");
    char *c;
    
    while(g_ch <= 0)
    {
        // count++;
        // if(count >= 100)
        // {
        //    return '\0'; 
        // }
    };
    c = g_ch;
    g_ch = 0;
    g_scan_code = 0;
    //printf("CX");
    return c;
}
// Define a variable to store the ID of the process that currently holds the IO lock
int current_caller_process_id = 0;
int disable_io = 0; //0 = false || 1 = true
// Function to get a character from the keyboard input, respecting the lock
char get_char(int caller_process_id)
{
    while (disable_io == 1)
    {
        /* code */
    }
    
    // Check if the caller's process ID matches the one holding the lock
    if (caller_process_id == current_caller_process_id)
    {
        // If the IDs match, allow the caller to read from the keyboard
        return kb_getchar_w();
    }
    else
    {
        // If the IDs don't match, return null character to indicate no input
        return '\0';
    }
}

// Function to acquire the keyboard input lock
void lock_kb_input(int caller_process_id)
{
    // Set the current caller process ID to the one requesting the lock
    current_caller_process_id = caller_process_id;
}

// Function to release the keyboard input lock
void unlock_kb_input()
{
    // Reset the current caller process ID to indicate that the lock is released
    current_caller_process_id = 0;
}

void keyboard_init() {
    
    // printf("registering keyboard\n");
    isr_register_interrupt_handler(IRQ_BASE + 1, keyboard_handler,__func__);
}

/**
 * Function Name: handle_F1_press
 * Description: Handles the press of the F1 key.
 *
 * Parameters:
 *   scancode (int) - The scancode of the pressed key.
 *
 * Return:
 *   None
 */
void handle_F1_press(int scancode) {
    printf("F1 key pressed. Scancode: %d\n", scancode);
}

/**
 * Function Name: handle_F2_press
 * Description: Handles the press of the F2 key.
 *
 * Parameters:
 *   scancode (int) - The scancode of the pressed key.
 *
 * Return:
 *   None
 */
void handle_F2_press(int scancode) {
    

    int buffer = cycle_buffers_vbe();
    dbgprintf("Pressed F2, buffer: %d\n", buffer);
    if(buffer == 1)
    {
        disable_io = false;
        enable_io_stream();
    }
    else if (buffer == 2)
    {
        disable_io = true;
        disable_io_stream();
    }
    
    // printf("F2 key pressed. Scancode: %d\n", scancode);
}

/**
 * Function Name: handle_F3_press
 * Description: Handles the press of the F3 key.
 *
 * Parameters:
 *   scancode (int) - The scancode of the pressed key.
 *
 * Return:
 *   None
 */
void handle_F3_press(int scancode) {
    printf("F3 key pressed. Scancode: %d\n", scancode);
}

/**
 * Function Name: handle_F4_press
 * Description: Handles the press of the F4 key.
 *
 * Parameters:
 *   scancode (int) - The scancode of the pressed key.
 *
 * Return:
 *   None
 */
void handle_F4_press(int scancode) {
    printf("F4 key pressed. Scancode: %d\n", scancode);
}

/**
 * Function Name: handle_F5_press
 * Description: Handles the press of the F5 key.
 *
 * Parameters:
 *   scancode (int) - The scancode of the pressed key.
 *
 * Return:
 *   None
 */
void handle_F5_press(int scancode) {
    printf("F5 key pressed. Scancode: %d\n", scancode);
}

/**
 * Function Name: handle_F6_press
 * Description: Handles the press of the F6 key.
 *
 * Parameters:
 *   scancode (int) - The scancode of the pressed key.
 *
 * Return:
 *   None
 */
void handle_F6_press(int scancode) {
    printf("F6 key pressed. Scancode: %d\n", scancode);
}

/**
 * Function Name: handle_F7_press
 * Description: Handles the press of the F7 key.
 *
 * Parameters:
 *   scancode (int) - The scancode of the pressed key.
 *
 * Return:
 *   None
 */
void handle_F7_press(int scancode) {
    printf("F7 key pressed. Scancode: %d\n", scancode);
}

/**
 * Function Name: handle_F8_press
 * Description: Handles the press of the F8 key.
 *
 * Parameters:
 *   scancode (int) - The scancode of the pressed key.
 *
 * Return:
 *   None
 */
void handle_F8_press(int scancode) {
    printf("F8 key pressed. Scancode: %d\n", scancode);
}

/**
 * Function Name: handle_F9_press
 * Description: Handles the press of the F9 key.
 *
 * Parameters:
 *   scancode (int) - The scancode of the pressed key.
 *
 * Return:
 *   None
 */
void handle_F9_press(int scancode) {
    printf("F9 key pressed. Scancode: %d\n", scancode);
}

/**
 * Function Name: handle_F10_press
 * Description: Handles the press of the F10 key.
 *
 * Parameters:
 *   scancode (int) - The scancode of the pressed key.
 *
 * Return:
 *   None
 */
void handle_F10_press(int scancode) {
    printf("F10 key pressed. Scancode: %d\n", scancode);
}

/**
 * Function Name: handle_F11_press
 * Description: Handles the press of the F11 key.
 *
 * Parameters:
 *   scancode (int) - The scancode of the pressed key.
 *
 * Return:
 *   None
 */
void handle_F11_press(int scancode) {
    printf("F11 key pressed. Scancode: %d\n", scancode);
}

/**
 * Function Name: handle_F12_press
 * Description: Handles the press of the F12 key.
 *
 * Parameters:
 *   scancode (int) - The scancode of the pressed key.
 *
 * Return:
 *   None
 */
void handle_F12_press(int scancode) {
    printf("F12 key pressed. Scancode: %d\n", scancode);
}
int register_arrow_callback(int scancode, arrow_callback_t callback) {
    if (scancode >= 0 && scancode < 256) {
        arrow_callbacks[scancode] = callback;
        return 0; // Success
    }
    return -1; // Invalid scancode
}
void arrow_press(int scancode) {
    if (scancode >= 0 && scancode < 256 && arrow_callbacks[scancode] != NULL) {
        arrow_callbacks[scancode]();
    }
}