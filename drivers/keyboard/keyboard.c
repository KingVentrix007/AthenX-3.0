#include "keyboard.h"
#include "stdbool.h"
#include "isr.h"
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
                g_ch = SCAN_CODE_KEY_UP;
                break;
            case 0x50: // Down Arrow
                g_ch = SCAN_CODE_KEY_DOWN;
                break;
            case 0x4D: // Right Arrow
                g_ch =  SCAN_CODE_KEY_RIGHT;
                break;
            case 0x4B: // Left Arrow
                g_ch = SCAN_CODE_KEY_LEFT;
                break;
            case 0x3B: // F1
                g_ch = "\u23A7";
                break;
            case 0x3C: // F2
                g_ch = "\u23A8";
                break;
            case 0x3D: // F3
                g_ch = "\u23A9";
                break;
            case 0x3E: // F4
                g_ch = "\u23AA";
                break;
            case 0x3F: // F5
                g_ch = "\u23AB";
                break;
            case 0x40: // F6
                g_ch = "\u23AC";
                break;
            case 0x41: // F7
                g_ch = "\u23AD";
                break;
            case 0x42: // F8
                g_ch = "\u23AE";
                break;
            case 0x43: // F9
                g_ch = "\u23AF";
                break;
            case 0x44: // F10
                g_ch = "\u23B0";
                break;
            case 0x57: // F11
                g_ch = "\u23B1";
                break;
            case 0x58: // F12
                g_ch = "\u23B2";
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

// Function to get a character from the keyboard input, respecting the lock
char get_char(int caller_process_id)
{
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
    printf("registering keyboard\n");
    isr_register_interrupt_handler(IRQ_BASE + 1, keyboard_handler);
}
