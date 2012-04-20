#include "commands.h"
#include "global_commands.h"

BOOL parse_pin_spec(const uint8_t *s, const uint8_t **end, uint8_t *port, uint8_t *pin);
BOOL handle_pin_command(const uint8_t *cmdname, size_t len);
BOOL handle_pinmode_command(const uint8_t *tok_start, const uint8_t *line_end);
