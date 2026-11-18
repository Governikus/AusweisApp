def get_rect_center(rect):
    x, y = rect['x'] + rect['width'] / 2, rect['y'] + rect['height'] / 2
    return int(x), int(y)
