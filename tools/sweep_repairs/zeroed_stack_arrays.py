"""Recover indexed stack arrays whose first write is a bulk clear."""

import re

from . import repair
from ._common import SIZES


CLEAR = re.compile(
    r"\bmemset\(\((?P<type>[su](?:8|16|32)|f32) \(\*\)\[\]\) &(?P<slot>sp[0-9A-F]+), "
    r"0, (?P<size>0x[0-9A-Fa-f]+|[1-9][0-9]*)\)"
)
FUNCTION_BODY = re.compile(r"^[^ \t\n][^\n]*\)\s*\{\n", re.M)


@repair("scored", 45)
def zeroed_stack_arrays(body: str) -> str:
    """Declare an m2c stack slot indexed as an array after a typed memset."""
    head = FUNCTION_BODY.search(body)
    if head is None:
        return body
    declarations = []
    for slot in dict.fromkeys(match["slot"] for match in CLEAR.finditer(body)):
        clears = [match for match in CLEAR.finditer(body) if match["slot"] == slot]
        shapes = {(match["type"], int(match["size"], 0)) for match in clears}
        if len(shapes) != 1:
            continue
        (element, size), = shapes
        indexed = re.compile(rf"\(&{slot}\)\[")
        if size == 0 or size % SIZES[element] or not indexed.search(body):
            continue
        without_clears = CLEAR.sub(lambda match: "" if match["slot"] == slot else match[0], body)
        if re.search(rf"\b{slot}\b", indexed.sub("[", without_clears)):
            continue
        declarations.append(f"    {element} {slot}[{size // SIZES[element]}];\n")
        body = CLEAR.sub(
            lambda match: f"memset({slot}, 0, {match['size']})" if match["slot"] == slot else match[0],
            body,
        )
        body = indexed.sub(f"{slot}[", body)
    return body[:head.end()] + "".join(declarations) + body[head.end():]
