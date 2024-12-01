#include "memory.h"

void* memcpy(void* dest, const void* src, size_t len)
{
    char* d = (char*) dest;
    char* s = (char*) src;

    while (len--)
    {
        *d++ = *s++;
    }
        
    return dest;
}

void* memset(void* dst, int value, size_t len)
{
    uint8_t* pu = (uint8_t*) dst;
    while (len--)
    {
        *pu++ = (uint8_t) value;
    }

    return dst;
}

char* strncat(char* dst, const char* src, size_t n)
{
    char* s = dst;

    while (*dst)
    {
        dst++;
    }
    
    while (n-- != 0 && (*dst++ = *src++))
    {
        if (n == 0)
        {
            *dst = '\0';
        }
    }

    return s;
}

int strncmp(const char* s1, const char* s2, size_t n)
{
    if (n == 0)
    {
        return 0;
    }

    while (n-- != 0 && *s1 == *s2)
    {
        if (n == 0 || *s1 == '\0')
        {
            break;
        }
        
        s1++;
        s2++;
    }

    return (*(uint8_t*) s1) - (*(uint8_t*) s2);
}

char* strncpy(char* dst, const char* src, size_t n)
{
    char *dscan = dst;
    const char *sscan = src;

    while (n > 0)
    {
        --n;
        if ((*dscan++ = *sscan++) == '\0')
        {
            break;
        }
    }

    while (n-- > 0)
    {
        *dscan++ = '\0';
    }

    return dst;
}

size_t strlen(const char* str)
{
    const char *start = str;

    while (*str)
    {
        str++;
    }
        
    return str - start;
}

char* utoa(uint32_t value, char *str, int base)
{
    const char digits[] = "0123456789abcdefghijklmnopqrstuvwxyz";
    int i, j;
    uint32_t remainder;
    char c;
    
    /* Check base is supported. */
    if ((base < 2) || (base > 36))
    { 
        str[0] = '\0';
        return NULL;
    }  
        
    /* Convert to string. Digits are in reverse order.  */
    i = 0;
    do 
    {
        remainder = value % base;
        str[i++] = digits[remainder];
        value = value / base;
    } while (value != 0);  
    str[i] = '\0'; 
    
    /* Reverse string.  */
    for (j = 0, i--; j < i; j++, i--)
    {
        c = str[j];
        str[j] = str[i];
        str[i] = c; 
    }       
    
    return str;
}

void itoa(int value, char* str, int base)
{
    uint32_t uvalue;
    int i = 0;
    
    /* Check base is supported. */
    if ((base < 2) || (base > 36))
    { 
        str[0] = '\0';
        return NULL;
    }  
        
    /* Negative numbers are only supported for decimal.
    * Cast to unsigned to avoid overflow for maximum negative value.  */ 
    if ((base == 10) && (value < 0))
    {              
        str[i++] = '-';
        uvalue = (uint32_t)-value;
    }
    else
    {
        uvalue = (uint32_t)value;
    }
    
    utoa(uvalue, &str[i], base);
    return str;
}
