import re
import sys
import math
import xml.etree.ElementTree as ET


def strip_namespace(tag):
    return tag.split("}", 1)[-1]


def parse_length(value):
    match = re.match(r"^\s*([-+]?\d*\.?\d+)([a-zA-Z]*)\s*$", value or "")
    if not match:
        raise ValueError(f"Invalid length: {value}")
    return float(match.group(1)), match.group(2)


def to_mm(value, unit):
    if unit in ("mm", ""):
        return value
    if unit == "cm":
        return value * 10.0
    if unit == "in":
        return value * 25.4
    if unit == "px":
        return value * 25.4 / 96.0
    raise ValueError(f"Unsupported unit: {unit}")


def mat_mul(a, b):
    return [
        [
            a[0][0] * b[0][0] + a[0][1] * b[1][0],
            a[0][0] * b[0][1] + a[0][1] * b[1][1],
            a[0][0] * b[0][2] + a[0][1] * b[1][2] + a[0][2],
        ],
        [
            a[1][0] * b[0][0] + a[1][1] * b[1][0],
            a[1][0] * b[0][1] + a[1][1] * b[1][1],
            a[1][0] * b[0][2] + a[1][1] * b[1][2] + a[1][2],
        ],
        [0, 0, 1],
    ]


def apply_mat(m, x, y):
    return (
        m[0][0] * x + m[0][1] * y + m[0][2],
        m[1][0] * x + m[1][1] * y + m[1][2],
    )


def parse_transform(transform):
    m = [[1, 0, 0], [0, 1, 0], [0, 0, 1]]

    if not transform:
        return m

    commands = re.findall(r"(\w+)\(([^)]*)\)", transform)

    for name, args in commands:
        values = [float(v) for v in re.split(r"[,\s]+", args.strip()) if v]

        if name == "translate":
            tx = values[0]
            ty = values[1] if len(values) > 1 else 0
            t = [[1, 0, tx], [0, 1, ty], [0, 0, 1]]

        elif name == "scale":
            sx = values[0]
            sy = values[1] if len(values) > 1 else sx
            t = [[sx, 0, 0], [0, sy, 0], [0, 0, 1]]

        elif name == "matrix":
            a, b, c, d, e, f = values
            t = [[a, c, e], [b, d, f], [0, 0, 1]]

        elif name == "rotate":
            angle = math.radians(values[0])
            cos_a = math.cos(angle)
            sin_a = math.sin(angle)

            if len(values) == 3:
                cx, cy = values[1], values[2]
                t1 = [[1, 0, cx], [0, 1, cy], [0, 0, 1]]
                r = [[cos_a, -sin_a, 0], [sin_a, cos_a, 0], [0, 0, 1]]
                t2 = [[1, 0, -cx], [0, 1, -cy], [0, 0, 1]]
                t = mat_mul(mat_mul(t1, r), t2)
            else:
                t = [[cos_a, -sin_a, 0], [sin_a, cos_a, 0], [0, 0, 1]]

        else:
            continue

        m = mat_mul(m, t)

    return m


def walk(element, parent_matrix, leds):
    local_matrix = parse_transform(element.attrib.get("transform"))
    current_matrix = mat_mul(parent_matrix, local_matrix)

    tag = strip_namespace(element.tag)

    if tag == "circle":
        element_id = element.attrib.get("id", "unnamed")
        cx = float(element.attrib.get("cx", 0))
        cy = float(element.attrib.get("cy", 0))
        r = float(element.attrib.get("r", 0))

        x, y = apply_mat(current_matrix, cx, cy)
        leds.append((element_id, x, y, r * 2.0))

    elif tag == "ellipse":
        element_id = element.attrib.get("id", "unnamed")
        cx = float(element.attrib.get("cx", 0))
        cy = float(element.attrib.get("cy", 0))
        rx = float(element.attrib.get("rx", 0))
        ry = float(element.attrib.get("ry", 0))

        x, y = apply_mat(current_matrix, cx, cy)
        leds.append((element_id, x, y, max(rx * 2.0, ry * 2.0)))

    for child in element:
        walk(child, current_matrix, leds)


def main(svg_path):
    tree = ET.parse(svg_path)
    root = tree.getroot()

    width_value, width_unit = parse_length(root.attrib.get("width"))
    height_value, height_unit = parse_length(root.attrib.get("height"))

    width_mm = to_mm(width_value, width_unit)
    height_mm = to_mm(height_value, height_unit)

    viewbox = root.attrib.get("viewBox")
    if viewbox:
        vb_x, vb_y, vb_w, vb_h = [float(v) for v in viewbox.split()]
    else:
        vb_x, vb_y, vb_w, vb_h = 0.0, 0.0, width_mm, height_mm

    scale_x = width_mm / vb_w
    scale_y = height_mm / vb_h

    leds = []
    identity = [[1, 0, 0], [0, 1, 0], [0, 0, 1]]

    walk(root, identity, leds)

    for index, (element_id, svg_x, svg_y, diameter) in enumerate(leds, start=1):
        x_mm = (svg_x - vb_x) * scale_x
        y_mm = (svg_y - vb_y) * scale_y
        diameter_mm = diameter * scale_x

        print(f"// {element_id} | diameter: {diameter_mm:.3f} mm")
        print(
            f"addChild(createLightCentered<MediumLight<MagentaLight>>("
            f"mm2px(Vec({x_mm:.3f}, {y_mm:.3f})), module, TL_Treex::VU_{index}_LIGHT));"
        )
        print()


if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: python svg_leds_to_vcv.py leds.svg")
        sys.exit(1)

    main(sys.argv[1])