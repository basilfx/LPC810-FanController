/**
 * Connect to UART0.
 */
int puts(const char * str)
{
    put_string(str);

    return 0;
}

/**
 * Connect to UART0.
 */
void putc(char c)
{
    put_char(c);
}