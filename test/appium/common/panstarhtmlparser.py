from html.parser import HTMLParser


class PanstarHtmlParser(HTMLParser):
    def __init__(self):
        super().__init__()
        self.non_empty_lines = []

    def handle_data(self, data):
        data = data.strip()
        if data:
            self.non_empty_lines.append(data)

    def data_present(self, field_name, permission, field_value):
        try:
            i = self.non_empty_lines.index(field_name)
        except ValueError:
            return False

        return (
            self.non_empty_lines[i + 1] == permission
            and self.non_empty_lines[i + 2] == field_value
        )
