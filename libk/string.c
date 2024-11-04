#include "string.h"
#include "stdint.h"
#include "stddef.h"
#include <stdbool.h>

#define INT_MAX 2147483647
#define INT_MIN -2147483648
#define EPS 1.073741824e-16

void *memset(void *dst, char c, uint32 n) {
    char *temp = dst;
    for (; n != 0; n--) *temp++ = c;
    return dst;
}
char* strchr(const char* str, int character) {
    while (*str != '\0') {
        if (*str == character) {
            return (char*)str;
        }
        str++;
    }

    // If the character is not found, return NULL
    return NULL;
}
char* strtok(char* str, const char* delimiters) {
    static char* next_token = NULL;
    char* current_token;
    const char* current_delimiter;

    // If a new string is provided, set it as the starting point for tokenization
    if (str != NULL) {
        next_token = str;
    }

    // Skip leading delimiters
    while (*next_token != '\0' && strchr(delimiters, *next_token) != NULL) {
        next_token++;
    }

    // If the string is empty or contains only delimiters, return NULL
    if (*next_token == '\0') {
        return NULL;
    }

    // Find the end of the current token
    current_token = next_token;
    while (*next_token != '\0' && strchr(delimiters, *next_token) == NULL) {
        next_token++;
    }

    // If we encountered a delimiter, null-terminate the current token and set the next_token to the character after the delimiter
    if (*next_token != '\0') {
        *next_token = '\0';
        next_token++;
    }

    return current_token;
}

void* memmove(void* dest, const void* src, size_t size) {
    // Check for overlapping regions
    if (src < dest && src + size > dest) {
        // Copy backward to avoid overwriting the source data
        const char* source = (const char*)src + size;
        char* destination = (char*)dest + size;

        while (size--) {
            *(--destination) = *(--source);
        }
    } else {
        // Copy forward since there's no overlap
        const char* source = (const char*)src;
        char* destination = (char*)dest;

        while (size--) {
            *(destination++) = *(source++);
        }
    }

    return dest;
}
void *memcpy(void *dst, const void *src, uint32 n) {
    //FUNC_ADDR_NAME(&memcpy);
    char *ret = dst;
    char *p = dst;
    const char *q = src;
    while (n--)
        *p++ = *q++;
    return ret;
}
char* strtok_r(char* str, const char* delim, char** saveptr) {
    if (str == NULL && (*saveptr) == NULL) {
        return NULL; // No more tokens to tokenize
    }

    if (str != NULL) {
        (*saveptr) = str; // Initialize or reset the saveptr
    }

    // Find the next occurrence of the delimiter or the end of the string
    while ((**saveptr) != '\0' && strchr(delim, (**saveptr)) == NULL) {
        (*saveptr)++;
    }

    // If the current character is a delimiter, replace it with null character
    if ((**saveptr) != '\0') {
        (**saveptr) = '\0';
        (*saveptr)++;
    } else {
        (*saveptr) = NULL; // No more tokens
    }

    return str;
}
int memcmp (const void *str1, const void *str2, size_t count)
{
  register const unsigned char *s1 = (const unsigned char*)str1;
  register const unsigned char *s2 = (const unsigned char*)str2;

  while (count-- > 0)
    {
      if (*s1++ != *s2++)
	  return s1[-1] < s2[-1] ? -1 : 1;
    }
  return 0;
}
int memcmp_string(const void *aptr, const void *bptr, size_t size)
{
    const unsigned char *a = (const unsigned char *)aptr;
    const unsigned char *b = (const unsigned char *)bptr;
    size_t i;
    for (i = 0; i < size; i++)
        if (a[i] < b[i])
            return -1;
        else if (b[i] < a[i])
            return 1;
    return 0;
}
size_t strlen(const char *s) {
    int len = 0;
    while (*s++)
        len++;
    return len;
}
int strcmp(const char *s1, const char *s2) {
    int i = 0;

    while ((s1[i] == s2[i])) {
        if (s2[i++] == 0)
            return 0;
    }
    return 1;
}

char *strstr(char *s1, const char *s2)
{
    size_t n = strlen(s2);
    while (*s1)
        if (!memcmp_string(s1++, s2, n))
            return s1 - 1;
    return NULL;
}
char *ctos(char s[2], const char c)
{
     s[0] = c;
     s[1] = '\0';
     return s;
}
int strcpy(char *dst, const char *src) {
    int i = 0;
    while ((*dst++ = *src++) != 0)
        i++;
    return i;
}
bool backspace(char *buffer) {
    int len = strlen(buffer);
    if (len > 0) {
        buffer[len - 1] = '\0';
        buffer--;
        return true;
    } else {
        return false;
    }
}
int atoi(const char *s)
{
    int n, sign;
    
    while (isspace(*s))
        s++;                        /* skip whitespace */
    sign = (*s == '-') ? -1 : 1;
    if (*s == '+' || *s == '-')     /* skip sign */
        s++;
    for (n = 0; isdigit(*s); s++)
        n = 10 * n + (*s -'0');
    return sign * n;
}
void strcat(char *dest, const char *src) {
    char *end = (char *)dest + strlen(dest);
    memcpy((void *)end, (void *)src, strlen(src));
    end = end + strlen(src);
    *end = '\0';
}



// char *strncpy(char *dst, const char *src, size_t n)
// {
// 	if (n != 0) {
// 		char *d = dst;
// 		const char *s = src;

// 		do {
// 			if ((*d++ = *s++) == 0) {
// 				/* NUL pad the remaining n-1 bytes */
// 				while (--n != 0)
// 					*d++ = 0;
// 				break;
// 			}
// 		} while (--n != 0);
// 	}
// 	return (dst);
// }

// int strncmp(const char *s1, const char *s2, register size_t n)
// {
//   register unsigned char u1, u2;

//   while (n-- > 0)
//     {
//       u1 = (unsigned char) *s1++;
//       u2 = (unsigned char) *s2++;
//       if (u1 != u2)
// 	return u1 - u2;
//       if (u1 == '\0')
// 	return 0;
//     }
//   return 0;
// }
char lower(char c) {
    if ((c >= 'A') && (c <= 'Z'))
        return (c + 32);
    return c;
}





char *strncpy(char *dst, const char *src, size_t n)
{
	if (n != 0) {
		char *d = dst;
		const char *s = src;

		do {
			if ((*d++ = *s++) == 0) {
				/* NUL pad the remaining n-1 bytes */
				while (--n != 0)
					*d++ = 0;
				break;
			}
		} while (--n != 0);
	}
	return (dst);
}


int
strncmp(const char *s1, const char *s2, register size_t n)
{
  register unsigned char u1, u2;

  while (n-- > 0)
    {
      u1 = (unsigned char) *s1++;
      u2 = (unsigned char) *s2++;
      if (u1 != u2)
	return u1 - u2;
      if (u1 == '\0')
	return 0;
    }
  return 0;
}
void itoa(char *buf, int base, int d) {
    char *p = buf;
    char *p1, *p2;
    unsigned long ud = d;
    int divisor = 10;

    /* If %d is specified and D is minus, put ‘-’ in the head. */
    if (base == 'd' && d < 0) {
        *p++ = '-';
        buf++;
        ud = -d;
    } else if (base == 'x')
        divisor = 16;

    /* Divide UD by DIVISOR until UD == 0. */
    do {
        int remainder = ud % divisor;
        *p++ = (remainder < 10) ? remainder + '0' : remainder + 'a' - 10;
    } while (ud /= divisor);

    /* Terminate BUF. */
    *p = 0;

    /* Reverse BUF. */
    p1 = buf;
    p2 = p - 1;
    while (p1 < p2) {
        char tmp = *p1;
        *p1 = *p2;
        *p2 = tmp;
        p1++;
        p2--;
    }
}


int string_length(char s[]) {
    int i = 0;
    while (s[i] != '\0') {
        ++i;
    }
    return i;
}

void append(char s[], char n) {
    int len = string_length(s);
    s[len] = n;
    s[len + 1] = '\0';
}




void parse_string(char *parser, char *string, char c)
{
    uint32_t i = 0;

    while (string[i] != c)
    {
        parser[i] = string[i];
        i++;
    }
    parser[i] = '\0';
}


bool hex_string_to_uint16(const char* hexString, uint16_t* result) {
    // Check if the string starts with "0x"
    if (hexString == NULL || hexString[0] != '0' || hexString[1] != 'x') {
        return false; // Invalid format
    }

    // Use strtol to convert the rest of the string to uint16_t
    char* endptr;
    unsigned long tempResult = strtoul(hexString + 2, &endptr, 16);

    // Check for errors during conversion
    if (*endptr != '\0' || tempResult > UINT16_MAX) {
        return false; // Conversion error or value out of range
    }

    *result = (uint16_t)tempResult;
    return true; // Successful conversion
}






// Function to convert a string to an unsigned long integer
unsigned long strtoul(const char* str, char** endptr, int base) {
    // Handle the base argument (for simplicity, only support base 10 and 16)
    if (base != 10 && base != 16) {
        if (endptr != NULL) {
            *endptr = (char*)str;
        }
        return 0;
    }

    // Initialize the result and the sign
    unsigned long result = 0;
    int sign = 1;

    // Skip leading whitespace characters
    while (*str == ' ' || *str == '\t') {
        str++;
    }

    // Handle optional sign
    if (*str == '-') {
        sign = -1;
        str++;
    } else if (*str == '+') {
        str++;
    }

    // Handle "0x" prefix for hexadecimal values
    if (base == 16 && str[0] == '0' && (str[1] == 'x' || str[1] == 'X')) {
        str += 2;
    }

    // Convert the string to an unsigned long integer
    while (*str != '\0') {
        char c = *str;

        if (base == 10 && (c < '0' || c > '9')) {
            break;
        } else if (base == 16 && !((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'))) {
            break;
        }

        result = result * base;
        if (c >= '0' && c <= '9') {
            result += c - '0';
        } else if (c >= 'a' && c <= 'f') {
            result += c - 'a' + 10;
        } else if (c >= 'A' && c <= 'F') {
            result += c - 'A' + 10;
        }

        str++;
    }

    // Set the endptr to the next character after the parsed number
    if (endptr != NULL) {
        *endptr = (char*)str;
    }

    // Apply the sign
    return result * sign;
}


bool isalnum(int c) {
    // Check if the character is a letter (a-z, A-Z) or a digit (0-9)
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9');
}


unsigned long long strtoull(const char *str, char **endptr, int base) {
    unsigned long long result = 0;
    int sign = 1;

    // Handle optional '+' or '-' sign
    if (*str == '+') {
        str++;
    } else if (*str == '-') {
        str++;
        sign = -1;
    }

    // Handle base prefix (0x for hexadecimal, 0 for octal)
    if (base == 0) {
        if (*str == '0') {
            str++;
            if (*str == 'x' || *str == 'X') {
                base = 16;
                str++;
            } else {
                base = 8;
            }
        } else {
            base = 10;
        }
    }

    // Convert the string to an unsigned long long
    while (isdigit(*str) || (base == 16 && isxdigit(*str))) {
        int digit;
        if (isdigit(*str)) {
            digit = *str - '0';
        } else {
            digit = tolower(*str) - 'a' + 10; // Convert hex character to digit
        }

        if (digit >= base) {
            break; // Invalid digit for the given base
        }

        result = result * base + digit;
        str++;
    }

    // Set endptr if it's provided
    if (endptr != NULL) {
        *endptr = (char *)str;
    }

    return result * sign;
}


char* strrchr(const char* str, int ch) {
    if (str == NULL)
        return NULL;

    const char* last_occurrence = NULL;

    // Traverse the string and update the pointer when the character is found
    while (*str != '\0') {
        if (*str == ch)
            last_occurrence = str;
        str++;
    }

    // If the character was found, return the pointer to the last occurrence
    if (last_occurrence != NULL)
        return (char*)last_occurrence;

    // If the character was not found, return NULL
    return NULL;
}
/**
 * Function Name: my_strcspn
 * Description: Finds the length of the initial segment of a string
 *              consisting of characters not in the reject string.
 *
 * Parameters:
 *   str (const char*) - The string to search.
 *   reject (const char*) - The set of characters to reject.
 *
 * Return:
 *   size_t - The length of the initial segment of 'str' without any characters from 'reject'.
 */
size_t strcspn(const char* str, const char* reject) {
    size_t length = 0;
    while (str[length] != '\0') {
        if (strchr(reject, str[length]) != NULL) {
            break;
        }
        length++;
    }
    return length;
}
// char* strdup(const char* str) {
//     if (str == NULL) {
//         return NULL;
//     }

//     // Calculate the length of the input string
//     size_t length = strlen(str) + 1;

//     // Allocate memory for the duplicated string
//     char* duplicate = (char*)kmalloc(length);

//     if (duplicate == NULL) {
//         // Memory allocation failed
//         return NULL;
//     }

//     // Copy the input string into the newly allocated memory
//     strcpy(duplicate, str);
//      return duplicate;
// }
   

void strncat(char* dest, const char* src, size_t destSize) {
    size_t destLen = strlen(dest);
    size_t srcLen = strlen(src);

    // Calculate available space in the destination buffer
    size_t availableSpace = destSize - destLen;

    // Check if there's enough space to concatenate the source string
    if (srcLen >= availableSpace) {
        // Truncate the source string if it's too long
        srcLen = availableSpace - 1;
    }

    // Copy the source string into the destination
    strncpy(dest + destLen, src, srcLen);

    // Null-terminate the resulting string
    dest[destLen + srcLen] = '\0';
}