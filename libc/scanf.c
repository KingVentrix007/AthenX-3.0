
#include "stdarg.h"
#include <string.h>
#include "stdbool.h"
#include "scanf.h"
#include "stdint.h"
#include "ctype.h"
#include "io.h"
static int do_scanf(const char *buffer, const char *fmt, __builtin_va_list args);
static void store_int(void *dest, unsigned int size, unsigned long long i);
int fill_buffer(char *buffer) {
    int c;
    int index = 0;
    while ((c = getchar()) != '\n' && c != EOF && index < BUFFER_SIZE - 1) {
        printf("%c",c);
        buffer[index++] = (char)c;
    }
    buffer[index] = '\0'; // Null-terminate the string
    return index; // Return the number of characters read
}
int scanf(const char *__restrict format, ...) {
    char buffer[BUFFER_SIZE]; // Create a buffer to store input
    int chars_read = fill_buffer(buffer); // Fill the buffer with input from stdin
    va_list args;
    va_start(args, format);
    int result = 0;
    if (chars_read > 0) {
        // If characters were read into the buffer, use sscanf to parse them
        result = do_scanf(buffer, format, args);
    }
    va_end(args);
    return result; // Return the result of vsscanf
}
// Assuming the definition of SCANF_TYPE_INT, SCANF_TYPE_CHAR, etc.
// and the definition of handler and its methods are provided elsewhere.

/**
 * Function Name: do_scanf
 * Description: Parses input according to a format string, similar to scanf in C.
 *
 * Parameters:
 *   handler (H&) - The handler object for input.
 *   fmt (const char*) - The format string.
 *   args (__builtin_va_list) - Variable arguments list.
 *
 * Return:
 *   int - The number of successful matches.
 */
static int do_scanf(const char *buffer, const char *fmt, __builtin_va_list args)
{
    int match_count = 0;
    int num_consumed = 0;
    for (; *fmt; fmt++)
    {
        if (isspace(*fmt))
        {
            while (isspace(fmt[1]))
                fmt++;
            while (isspace(*buffer))
                buffer++;
            num_consumed++;
            continue;
        }

        if (*fmt != '%' || fmt[1] == '%')
        {
            if (*fmt == '%')
                fmt++;
            char c = buffer++;
            num_consumed++;
            if (c != *fmt)
                break;
            continue;
        }

        void *dest = NULL;
        if (isdigit(*fmt) && fmt[1] == '$')
        {
            fmt += 3;
        }
        else
        {
            if (fmt[1] != '*')
            {
                dest = va_arg(args, void *);
            }
            fmt++;
        }

        int width = 0;
        if (*fmt == '*')
        {
            fmt++;
        }
        else if (*fmt == '\'')
        {
            // TODO: numeric separators locale stuff
            fmt++;
            continue;
        }
        else if (*fmt == 'm')
        {
            // TODO: allocate buffer for them
            fmt++;
            continue;
        }
        else if (*fmt >= '0' && *fmt <= '9')
        {
            width = 0;
            while (*fmt >= '0' && *fmt <= '9')
            {
                width = width * 10 + (*fmt - '0');
                fmt++;
            }
        }

        unsigned int type = SCANF_TYPE_INT;
        unsigned int base = 10;
        switch (*fmt)
        {
        case 'h':
        {
            if (fmt[1] == 'h')
            {
                type = SCANF_TYPE_CHAR;
                fmt += 2;
                break;
            }
            type = SCANF_TYPE_SHORT;
            fmt++;
            break;
        }
        case 'j':
        {
            type = SCANF_TYPE_INTMAX;
            fmt++;
            break;
        }
        case 'l':
        {
            if (fmt[1] == 'l')
            {
                type = SCANF_TYPE_LL;
                fmt += 2;
                break;
            }
            type = SCANF_TYPE_L;
            fmt++;
            break;
        }
        case 'L':
        {
            type = SCANF_TYPE_LL;
            fmt++;
            break;
        }
        case 'q':
        {
            type = SCANF_TYPE_LL;
            fmt++;
            break;
        }
        case 't':
        {
            type = SCANF_TYPE_PTRDIFF;
            fmt++;
            break;
        }
        case 'z':
        {
            type = SCANF_TYPE_SIZE_T;
            fmt++;
            break;
        }
        }

        if (*fmt != 'c' && *fmt != '[' && *fmt != 'n')
        {
            while (isspace(*buffer))
                buffer++;
            num_consumed++;
        }

        switch (*fmt)
        {
        case 'd':
        case 'u':
            base = 10;
            // Fallthrough
        case 'i':
        {
            bool is_negative = false;
            unsigned long long res = 0;
            if ((*fmt == 'i' || *fmt == 'd') && *buffer == '-')
            {
                buffer++;
                num_consumed++;
                is_negative = true;
            }
            if (*fmt == 'i' && *buffer == '0')
            {
                buffer++;
                num_consumed++;
                if (*buffer == 'x')
                {
                    buffer++;
                    num_consumed++;
                    base = 16;
                }
                else
                {
                    base = 8;
                }
            }
            char c = *buffer;
            switch (base)
            {
            case 10:
                if (!isdigit(c))
                    return match_count;
                while (c >= '0' && c <= '9')
                {
                    buffer++;
                    num_consumed++;
                    res = res * 10 + (c - '0');
                    c = *buffer;
                }
                break;
            case 16:
                if (c == '0')
                {
                    buffer++;
                    num_consumed++;
                    c = *buffer;
                    if (c == 'x')
                    {
                        buffer++;
                        num_consumed++;
                        c = *buffer;
                    }
                }
                while (true)
                {
                    if (c >= '0' && c <= '9')
                    {
                        buffer++;
                        num_consumed++;
                        res = res * 16 + (c - '0');
                    }
                    else if (c >= 'a' && c <= 'f')
                    {
                        buffer++;
                        num_consumed++;
                        res = res * 16 + (c - 'a' + 10);
                    }
                    else if (c >= 'A' && c <= 'F')
                    {
                        buffer++;
                        num_consumed++;
                        res = res * 16 + (c - 'A' + 10);
                    }
                    else
                    {
                        break;
                    }
                    c = *buffer;
                }
                break;
            case 8:
                while (c >= '0' && c <= '7')
                {
                    buffer++;
                    num_consumed++;
                    res = res * 8 + (c - '0');
                    c = *buffer;
                }
                break;
            }
            if (dest)
            {
                if (is_negative)
                    store_int(dest, type, -res);
                else
                    store_int(dest, type, res);
            }
            break;
        }
        case 'o':
        {
            unsigned long long res = 0;
            char c = *buffer;
            while (c >= '0' && c <= '7')
            {
                buffer++;
                num_consumed++;
                res = res * 8 + (c - '0');
                c = *buffer;
            }
            if (dest)
                store_int(dest, type, res);
            break;
        }
        case 'x':
        case 'X':
        {
            unsigned long long res = 0;
            char c = *buffer;
            if (c == '0')
            {
                buffer++;
                num_consumed++;
                c = *buffer;
                if (c == 'x')
                {
                    buffer++;
                    num_consumed++;
                    c = *buffer;
                }
            }
            while (true)
            {
                if (c >= '0' && c <= '9')
                {
                    buffer++;
                    num_consumed++;
                    res = res * 16 + (c - '0');
                }
                else if (c >= 'a' && c <= 'f')
                {
                    buffer++;
                    num_consumed++;
                    res = res * 16 + (c - 'a' + 10);
                }
                else if (c >= 'A' && c <= 'F')
                {
                    buffer++;
                    num_consumed++;
                    res = res * 16 + (c - 'A' + 10);
                }
                else
                {
                    break;
                }
                c = *buffer;
            }
            if (dest)
                store_int(dest, type, res);
            break;
        }
        case 's':
        {
            char *typed_dest = (char *)dest;
            char c = *buffer;
            int count = 0;
            while (c && !isspace(c))
            {
                buffer++;
                num_consumed++;
                if (typed_dest)
                    typed_dest[count] = c;
                c = *buffer;
                count++;
                if (width && count >= width)
                    break;
            }
            if (typed_dest)
                typed_dest[count] = '\0';
            break;
        }
        case 'c':
        {
            char *typed_dest = (char *)dest;
            char c = *buffer;
            int count = 0;
            if (!width)
                width = 1;
            while (c && count < width)
            {
                buffer++;
                num_consumed++;
                if (typed_dest)
                    typed_dest[count] = c;
                c = *buffer;
                count++;
            }
            break;
        }
        case '[':
        {
            fmt++;
            int invert = 0;
            if (*fmt == '^')
            {
                invert = 1;
                fmt++;
            }
            char scanset[257];
            memset(&scanset[0], invert, sizeof(char) * 257);
            scanset[0] = '\0';
            if (*fmt == '-')
            {
                fmt++;
                scanset[1 + '-'] = 1 - invert;
            }
            else if (*fmt == ']')
            {
                fmt++;
                scanset[1 + ']'] = 1 - invert;
            }
            for (; *fmt != ']'; fmt++)
            {
                if (!*fmt)
                    return EOF;
                if (*fmt == '-' && *fmt != ']')
                {
                    fmt++;
                    for (char c = *(fmt - 2); c < *fmt; c++)
                        scanset[1 + c] = 1 - invert;
                }
                scanset[1 + *fmt] = 1 - invert;
            }
            char *typed_dest = (char *)dest;
            int count = 0;
            char c = *buffer;
            while (c && (!width || count < width))
            {
                buffer++;
                num_consumed++;
                if (!scanset[1 + c])
                    break;
                if (typed_dest)
                    typed_dest[count] = c;
                c = *buffer;
                count++;
            }
            if (typed_dest)
                typed_dest[count] = '\0';
            break;
        }
        case 'p':
        {
            unsigned long long res = 0;
            char c = *buffer;
            if (c == '0')
            {
                buffer++;
                num_consumed++;
                c = *buffer;
                if (c == 'x')
                {
                    buffer++;
                    num_consumed++;
                    c = *buffer;
                }
            }
            while (true)
            {
                if (c >= '0' && c <= '9')
                {
                    buffer++;
                    num_consumed++;
                    res = res * 16 + (c - '0');
                }
                else if (c >= 'a' && c <= 'f')
                {
                    buffer++;
                    num_consumed++;
                    res = res * 16 + (c - 'a');
                }
                else if (c >= 'A' && c <= 'F')
                {
                    buffer++;
                    num_consumed++;
                    res = res * 16 + (c - 'A');
                }
                else
                {
                    break;
                }
                c = *buffer;
            }
            void **typed_dest = (void **)dest;
            *typed_dest = (void *)(uintptr_t)res;
            break;
        }
        case 'n':
        {
            int *typed_dest = (int *)dest;
            if (typed_dest)
                *typed_dest = num_consumed;
            continue;
        }
        }
        if (dest)
            match_count++;
    }
    return match_count;
}
static void store_int(void *dest, unsigned int size, unsigned long long i) {
	switch (size) {
		case SCANF_TYPE_CHAR:
			*(char *)dest = i;
			break;
		case SCANF_TYPE_SHORT:
			*(short *)dest = i;
			break;
		case SCANF_TYPE_INTMAX:
			*(intmax_t *)dest = i;
			break;
		case SCANF_TYPE_L:
			*(long *)dest = i;
			break;
		case SCANF_TYPE_LL:
			*(long long *)dest = i;
			break;
		case SCANF_TYPE_PTRDIFF:
			*(ptrdiff_t *)dest = i;
			break;
		case SCANF_TYPE_SIZE_T:
			*(size_t *)dest = i;
			break;
		/* fallthrough */
		case SCANF_TYPE_INT:
		default:
			*(int *)dest = i;
			break;
	}
}