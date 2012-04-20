#include <avr/pgmspace.h>
#include "ninja.h"
#include "parse.h"
#include "commands.h"
#include "global_commands.h"
#include "console.h"
#include "bus.h"
#include "portpin.h"


#ifdef COMMAND_PIN

BOOL parse_pin_spec(const uint8_t *s, const uint8_t **end, uint8_t *port, uint8_t *pin)
{
    if (*s++ != 'p')
        return FALSE;

    if (*s >= 'a' && *s <= 'd')
    {
        // pa3
        *port = *s++ - 'a';
        *pin = *s++ & 7;
    }
    else if (s[1] == '.' && *s >= '1' && *s <= '4')
    {
        // p1.3
        *port = *s - '1';
        s += 2;
        *pin = *s++ & 7;
    }
    else if (*s >= '0' && *s <= '9')
    {
        *pin = *s++ & 0x0f;
        if (*s >= '0' && *s <= '9') {
            *pin = *pin * 10 + (*s++ & 0x0f);
        }
        if (*pin > 8)
            *port = 1;
        else
            *port = 3;
        *pin &= 7;
    }
    else
    {
        return FALSE;
    }

    *end = s;
    return TRUE;
}


BOOL handle_pin_command(const uint8_t *cmdname, size_t len)
{
    uint8_t port, pin;
    const uint8_t *e;
    volatile uint8_t *reg;

    if (!parse_pin_spec(cmdname, &e, &port, &pin))
        return syntax_error();

    pin = _BV(pin);
    switch (port) {
#ifdef PORTA
        case 0:
            reg = &PORTA; break;
#endif
        case 1:
            reg = &PORTB; break;
        case 2:
            reg = &PORTC; break;
        case 3:
            reg = &PORTD; break;
        default:
            return syntax_error();
    }

    if (*e == '=')
    {
        if (e[1] & 1)
            *reg |= pin;
        else
            *reg &= ~pin;
    }
    else if (*e == '?')
    {
        bus_log_puts_P(PSTR("READ: "));
        bus_log_putc(*reg & pin ? '1' : '0');
        bus_log_newline();
    }

    return FALSE;
}

BOOL handle_pinmode_command(const uint8_t *tok_start, const uint8_t *line_end)
{
    const uint8_t *tok_end;
    uint32_t dummy;

    uint8_t port, pin;
    const uint8_t *e;
    volatile uint8_t *reg;

    TOK_NUM(&tok_end, &dummy);
    if (!parse_pin_spec(tok_start, &e, &port, &pin))
    {
        syntax_error();
        return TRUE;
    }

    tok_start = tok_end;
    TOK_NUM(&tok_end, &dummy);

    pin = _BV(pin);
    switch (port) {
#ifdef PORTA
        case 0:
            reg = &DDRA; break;
#endif
        case 1:
            reg = &DDRB; break;
        case 2:
            reg = &DDRC; break;
        case 3:
            reg = &DDRD; break;
        default:
            return syntax_error();
    }

    if (*tok_start == 'i')
        *reg &= ~pin;
    else if (*tok_start == 'o')
        *reg |= pin;
    else
        syntax_error();

    return TRUE;
}

#endif /* COMMAND_PIN */
