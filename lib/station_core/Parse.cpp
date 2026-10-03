#include "Parse.h"
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdlib.h>
#include "Protocol.h"

// Digits-only unsigned parse; overflow saturates to ULONG_MAX.
static bool parseUnsigned(const char* s, int base, unsigned long* out) {
    if (*s == '\0') return false;
    for (const char* p = s; *p; p++) {
        const unsigned char c = static_cast<unsigned char>(*p);
        if (base == 16 ? !isxdigit(c) : !isdigit(c)) return false;
    }
    errno = 0;
    unsigned long v = strtoul(s, nullptr, base);
    *out = errno == ERANGE ? ULONG_MAX : v;
    return true;
}

bool parseMask(const char* s, uint8_t* out) {
    if (s == nullptr) return false;
    int base = 10;
    if (s[0] == '0' && (s[1] == 'x' || s[1] == 'X')) {
        base = 16;
        s += 2;
    }
    unsigned long v;
    if (!parseUnsigned(s, base, &v) || v > ALL_BUTTONS_MASK) return false;
    *out = static_cast<uint8_t>(v);
    return true;
}

bool parseDurationMs(const char* s, uint32_t defaultMs, uint32_t maxMs, uint32_t* out) {
    if (s == nullptr || *s == '\0') {
        *out = defaultMs;
        return true;
    }
    unsigned long v;
    if (!parseUnsigned(s, 10, &v) || v == 0) return false;
    *out = v > maxMs ? maxMs : static_cast<uint32_t>(v);
    return true;
}
