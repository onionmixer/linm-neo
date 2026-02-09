#!/bin/sh
# Generate default_config.h from .cfg files
# Usage: generate_default_config.sh default.cfg colorset.cfg keyset.cfg syntexset.cfg output.h

DEFAULT_CFG="$1"
COLORSET_CFG="$2"
KEYSET_CFG="$3"
SYNTEXSET_CFG="$4"
OUTPUT="$5"

cat > "$OUTPUT" << 'HEADER_START'
#ifndef DEFAULT_CONFIG_H
#define DEFAULT_CONFIG_H

HEADER_START

for pair in "DEFAULT_CFG:$DEFAULT_CFG" "COLORSET_CFG:$COLORSET_CFG" "KEYSET_CFG:$KEYSET_CFG" "SYNTEXSET_CFG:$SYNTEXSET_CFG"; do
    varname="${pair%%:*}_CONTENT"
    filepath="${pair#*:}"
    printf 'static const char* %s = R"rawcfg(' "$varname" >> "$OUTPUT"
    cat "$filepath" >> "$OUTPUT"
    printf ')rawcfg";\n\n' >> "$OUTPUT"
done

echo '#endif // DEFAULT_CONFIG_H' >> "$OUTPUT"
