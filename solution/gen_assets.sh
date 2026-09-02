#!/bin/bash
# Generate placeholder billboard sprites for r01.
#
# tree.png: a green triangle canopy on a brown trunk, transparent background.
# rock.png: a grey rounded polygon, transparent background.
#
# Both are drawn at a fixed 128x128 so scaler.c's SDL_RenderCopy(..., NULL,
# &dst) always samples the whole texture -- the scaling happens entirely
# in the destination rect, not the source.

set -e

mkdir -p assets

python3 - <<'EOF'
from PIL import Image, ImageDraw

SIZE = 128

tree = Image.new("RGBA", (SIZE, SIZE), (0, 0, 0, 0))
d = ImageDraw.Draw(tree)
d.rectangle([SIZE // 2 - 8, SIZE - 40, SIZE // 2 + 8, SIZE], fill=(0x5a, 0x3a, 0x1e, 255))
d.polygon(
    [(SIZE // 2, 4), (SIZE - 12, SIZE - 44), (12, SIZE - 44)],
    fill=(0x2e, 0x8b, 0x57, 255),
)
tree.save("assets/tree.png")
print("  wrote assets/tree.png")

rock = Image.new("RGBA", (SIZE, SIZE), (0, 0, 0, 0))
d = ImageDraw.Draw(rock)
d.polygon(
    [
        (14, SIZE - 8), (6, SIZE - 50), (30, SIZE - 96),
        (SIZE - 30, SIZE - 100), (SIZE - 8, SIZE - 54), (SIZE - 16, SIZE - 8),
    ],
    fill=(0x7a, 0x7a, 0x7a, 255),
)
rock.save("assets/rock.png")
print("  wrote assets/rock.png")
EOF
