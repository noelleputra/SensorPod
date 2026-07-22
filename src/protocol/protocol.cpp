#include "protocol.h"

#include <string.h>

bool protocol::parseNodeCommand(const char* line, char prefix, uint8_t expectedNodeId,
                                 char* restOut, size_t restOutSize)
{
    if (line[0] != prefix)
    {
        return false;
    }

    const char* p = &line[1];
    uint8_t requestedId = 0;
    bool anyDigit = false;
    while (*p >= '0' && *p <= '9')
    {
        anyDigit = true;
        const uint8_t digit = static_cast<uint8_t>(*p - '0');
        if (requestedId > (255 - digit) / 10) return false; // overflow guard
        requestedId = requestedId * 10 + digit;
        ++p;
    }

    if (!anyDigit || requestedId != expectedNodeId)
    {
        return false;
    }

    if (*p == ':')
    {
        ++p;
    }
    else if (*p != '\0')
    {
        // Trailing characters that aren't a ':' separator -- not a
        // valid command for this node, e.g. "R22" would otherwise wrongly
        // match node 2.
        return false;
    }

    if (restOutSize > 0)
    {
        strncpy(restOut, p, restOutSize - 1);
        restOut[restOutSize - 1] = '\0';
    }
    return true;
}
