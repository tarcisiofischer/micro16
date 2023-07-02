import sys

from PIL import Image

COLORMAP = {
    (0x19, 0x19, 0x19): '0',
    (0xcb, 0xcb, 0xcb): '1',
    (0xac, 0x32, 0x32): '2',
    (0xac, 0x71, 0x6b): '3',
    (0x4f, 0xac, 0x43): '4',
    (0x92, 0xb6, 0x87): '5',
    (0x5b, 0x69, 0xac): '6',
    (0xac, 0xad, 0xc8): '7',
    (0xcc, 0xdb, 0x25): '8',
    (0xcc, 0xdb, 0x88): '9',
    (0xd2, 0x84, 0x2a): 'A',
    (0xd2, 0xac, 0x7a): 'B',
    (0x82, 0x4a, 0xad): 'C',
    (0xb7, 0x95, 0xc2): 'D',
    (0x1f, 0x7d, 0x6e): 'E',
    (0x87, 0xcc, 0xc8): 'F',
}

def as_hex_color(c):
    return COLORMAP[c]


def closest_color_to(c):
    import math
    mindist = +math.inf
    selected_color = (0, 0, 0)
    for cm in COLORMAP.keys():
        dist = math.sqrt((cm[0] - c[0])**2 + (cm[1] - c[1])**2 + (cm[2] - c[2])**2)
        if dist < mindist:
            mindist = dist
            selected_color = cm
    return selected_color


if __name__ == "__main__":
    im = Image.open(sys.argv[1])
    print(im.width, im.height, im.width * im.height)
    if im.width != 320 or im.height != 200:
        print("Error: Image size must be exactly 320x200 pixels")
        sys.exit(-1)

    s = ''
    with open('./pixel_art.m16asm.template', 'r') as f:
        s += f.read()

    for i, color in enumerate(im.getdata()):
        if i % 4 == 0:
            s += '\n.data 0x'
        s += as_hex_color(closest_color_to(color))

    with open('out.m16asm', 'w') as f:
        f.write(s)
