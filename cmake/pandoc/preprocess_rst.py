#!/usr/bin/env python
import argparse
import os
import re
import sys
from abc import ABC, abstractmethod

first_toctree_found = False
main_hierarchy = ['#', '*', '=', '-', '"', '^', '~']


def wrap_latex(latex_strings: list[str]) -> list[str]:
    """
    Wrap latex strings in a rst raw latex block and indent the code.
    """
    output = []
    output.append('')
    output.append('.. raw:: latex')
    output.append('')
    for latex_string in latex_strings:
        output.append(f'    {latex_string}')
    output.append('')
    return output


def is_header(line):
    """
    Check if the line is a heading by looking for 3 or more repeating
    characters from the main list and checking if it is the only character
    (to not confuse them with tables).
    """
    return (
        any(line.startswith(c * 3) for c in main_hierarchy)
        and len(set(line.rstrip())) == 1
    )


def get_file_heading_hierarchy(input_lines):
    """
    Identifies the heading characters used in a file and returns them in the
    order of their appearance.
    """
    hierarchy = []
    for line in input_lines:
        if is_header(line):
            if line[0] not in hierarchy:
                hierarchy.append(line[0])
    return hierarchy


def convert_file_heading_hierarchy_to_main(
    input_lines, local_hierarchy, starting_level
):
    """
    Rewrites the headings in a file to conform to a main heading hierarchy.
    """
    output_lines = []
    for line in input_lines:
        if is_header(line):
            character = line.strip()[0]
            level = local_hierarchy.index(character)
            new_character = main_hierarchy[starting_level + level]
            output_lines.append(
                line.rstrip().replace(character, new_character)
            )
        else:
            output_lines.append(line.rstrip())
    return output_lines


def parse_po_file(translation_data):
    """
    Parses translation data from a .po file
    """
    translations = {}
    # Split the file content into entry blocks based on one or more empty
    # lines. Skip the first block, which is the file header.
    blocks = re.split(r'\n\s*\n', translation_data.strip())[1:]

    for block in blocks:
        # Skip any empty blocks that might result from splitting
        if not block.strip():
            continue

        lines = block.splitlines()
        msgid_lines = []
        msgstr_lines = []
        current_key = None

        for line in lines:
            line = line.strip()
            # Skip comments and empty lines
            if line.startswith('#') or not line:
                continue

            if line.startswith('msgid '):
                current_key = 'msgid'
                match = re.search(r'"(.*)"', line)
                if match:
                    msgid_lines.append(match.group(1))
            elif line.startswith('msgstr '):
                current_key = 'msgstr'
                match = re.search(r'"(.*)"', line)
                if match:
                    msgstr_lines.append(match.group(1))
            elif line.startswith('"') and current_key:
                match = re.search(r'"(.*)"', line)
                if match:
                    content = match.group(1)
                    if current_key == 'msgid':
                        msgid_lines.append(content)
                    elif current_key == 'msgstr':
                        msgstr_lines.append(content)
        if msgid_lines:
            final_msgid = (
                ''.join(msgid_lines).replace('\\"', '"').replace('\\\\', '\\')
            )
            final_msgstr = (
                ''.join(msgstr_lines).replace('\\"', '"').replace('\\\\', '\\')
            )

            if final_msgid and final_msgstr:
                translations[final_msgid] = final_msgstr

    return translations


def translate_lines(input_lines, translation_file_path):
    """
    Translate the input_lines using the .po file at translation_file_path.
    """
    try:
        with open(translation_file_path, 'r', encoding='utf-8') as infile:
            translation_data = infile.read()
    except FileNotFoundError:
        print(f'Error: Input file "{translation_file_path}" not found.')
        sys.exit(1)
    except Exception as e:
        print(
            f'Error: Input file "{translation_file_path}" could not accessed: '
            + f'{e}'
        )
        sys.exit(1)

    translation_map = parse_po_file(translation_data)

    merged_lines = []
    line_idx = 0
    previous_line = ''
    while line_idx < len(input_lines):
        current_line = input_lines[line_idx]
        # Merge the current line with the previous line if
        # a) The previous line ended on an alphanumerical character or
        #    punctuation
        # b) The current line starts on an alphanumerical character or
        #    punctuation, disregarding spaces (except when we are in blocks,
        #    like toctree)
        # c) The current line is not a header underline
        # d) The current line does not start with '| ' (distinguish between
        #    item list and replace-marker)
        marker_for_merge = '[a-zA-Z0-9\\.,\\"-\\(\\)\\|:\\\\]'
        if (
            re.search(f'{marker_for_merge}$', previous_line.rstrip('\n'))
            and (
                re.match(f'^{marker_for_merge}', current_line)
                or re.match(f'^   \\s+{marker_for_merge}', current_line)
            )
            and not is_header(current_line)
            and not re.match(r'^\s*\| ', current_line)
        ):
            new_line = current_line.rstrip('\n').lstrip(' ')
            merged_lines[-1] = f'{previous_line} {new_line}'
        else:
            merged_lines.append(current_line.rstrip('\n'))
        previous_line = merged_lines[-1]
        line_idx += 1

    output_lines = []
    for original_line in merged_lines:
        translated_lines = []
        for key in translation_map.keys():
            if key in original_line:
                translated_lines.append(
                    (
                        original_line.rstrip().replace(
                            key, translation_map[key]
                        ),
                        len(key),
                    )
                )

        if translated_lines:
            # Use the longest translated line to not use partial translations.
            # Multiple translations per line are currently not supported.
            translated_lines.sort(key=lambda x: x[1], reverse=True)
            output_lines.append(translated_lines[0][0])
        else:
            output_lines.append(original_line)

    line_idx = 0
    while line_idx + 1 < len(output_lines):
        title = output_lines[line_idx]
        underline = output_lines[line_idx + 1]
        if is_header(underline):
            character = underline[0]
            output_lines[line_idx + 1] = character * len(title)
        line_idx += 1

    return output_lines


def include_file(filepath, parent_filepath, current_level, translate):
    """
    Resolves the path for an included file and processes it for flattening.
    """
    dir_path = os.path.dirname(parent_filepath)
    file_path = os.path.join(dir_path, filepath)
    if not os.path.exists(file_path):
        print(f'Error: Referenced file {file_path} does not exit.')
        sys.exit(1)

    return flatten_files(file_path, current_level, translate)


def flatten_include(
    input_lines, start_idx, current_level, parent_filepath, translate
):
    """
    Helper function parsing the include directive. Includes recursively the
    referenced files.
    """
    line_idx = start_idx
    output_lines = []

    match = re.match(r'.. include:: (.*)', input_lines[line_idx].strip())
    if not match:
        print(
            'Error: could not parse include directive,'
            + f'{parent_filepath} line {line_idx}'
        )
        sys.exit(1)
    output_lines.extend(
        include_file(match.group(1), parent_filepath, current_level, translate)
    )
    output_lines.append('')
    line_idx += 1
    return output_lines, line_idx


def flatten_toctree(
    input_lines, start_idx, current_level, parent_filepath, translate
):
    """
    Helper function parsing the toctree directive. Adds the table of contents
    at the position of the first toctree, parses the options and includes
    recursively the referenced files.
    """
    global first_toctree_found
    line_idx = start_idx + 1
    output_lines = []

    # For the first toctree found, insert a LaTeX table of contents.
    if not first_toctree_found:
        output_lines.extend(wrap_latex(['\\tableofcontents', '\\newpage']))
        first_toctree_found = True

    options = {}
    # Parse toctree options.
    while line_idx < len(input_lines) and input_lines[line_idx].strip() != '':
        option_match = re.match(
            ':([a-zA-Z0-9]+): (.*)',
            input_lines[line_idx].strip(),
        )
        if option_match:
            option_key = option_match.group(1)
            option_value = option_match.group(2)
            options[option_key] = option_value
        line_idx += 1
    line_idx += 1

    if 'caption' in options.keys():
        caption = options['caption']
        output_lines.append('')
        output_lines.append(caption)
        output_lines.append(main_hierarchy[current_level] * len(caption))
        current_level += 1
        output_lines.append('')

    # Recursively include and process files listed in the toctree.
    while line_idx < len(input_lines) and input_lines[line_idx].strip() != '':
        file_name = input_lines[line_idx].strip()
        output_lines.extend(
            include_file(
                f'{file_name}.rst', parent_filepath, current_level, translate
            )
        )
        output_lines.append('')
        line_idx += 1

    return output_lines, line_idx


def flatten_files(input_filepath, starting_level, translate):
    """
    Recursively flattens a ReStructuredText file by replacing `.. toctree::`
    and `.. include::` directives with their content, adjusting heading levels
    accordingly.
    """

    # read the input file
    try:
        with open(input_filepath, 'r', encoding='utf-8') as infile:
            input_lines = infile.readlines()
    except FileNotFoundError:
        print(f'Error: Input file "{input_filepath}" not found.')
        sys.exit(1)
    except Exception as e:
        print(
            f'Error: Input file "{input_filepath}" could not be accessed: {e}'
        )
        sys.exit(1)

    if translate:
        dir_path = os.path.dirname(input_filepath)
        filename = os.path.basename(input_filepath)
        filename_without_ext = os.path.splitext(filename)[0]
        translation_file_path = os.path.join(
            dir_path,
            f'locales/{translate}/LC_MESSAGES/{filename_without_ext}.po',
        )
        if not os.path.exists(translation_file_path):
            print(
                f'Error: Translation {translate} for {input_filepath} does not'
                + f'exit at {translation_file_path}.'
            )
            sys.exit(1)
        input_lines = translate_lines(input_lines, translation_file_path)

    # Determine the existing heading structure and adjust it to fit the main
    # document's hierarchy.
    heading_hierarchy = get_file_heading_hierarchy(input_lines)
    input_lines = convert_file_heading_hierarchy_to_main(
        input_lines, heading_hierarchy, starting_level
    )

    output_lines = []
    line_idx = 0
    current_level = starting_level

    # Process each line of the input file.
    while line_idx < len(input_lines):
        # If the line is a heading, update the current heading level.
        if is_header(input_lines[line_idx]):
            current_level = main_hierarchy.index(
                input_lines[line_idx].strip()[0]
            )
            output_lines.append(input_lines[line_idx].rstrip())
            line_idx += 1
        # If the line is a toctree directive, process the included files.
        elif input_lines[line_idx].strip().startswith('.. toctree::'):
            toctree_lines, line_idx = flatten_toctree(
                input_lines, line_idx, current_level, input_filepath, translate
            )
            output_lines.extend(toctree_lines)
        # If the line is an include directive, recursively include and process
        # the specified file.
        elif input_lines[line_idx].strip().startswith('.. include::'):
            include_lines, line_idx = flatten_include(
                input_lines, line_idx, current_level, input_filepath, translate
            )
            output_lines.extend(include_lines)
        # Otherwise, add the line to the output.
        else:
            output_lines.append(input_lines[line_idx].rstrip())
            line_idx += 1
    return output_lines


def replace_placeholder(input_lines, old, new):
    """
    Replace every occurrence of |<old>|
    """
    output_lines = []
    idx = 0
    while idx < len(input_lines):
        line = input_lines[idx]
        old_guarded = f'|{old}|'
        new_line = line.replace(old_guarded, new)
        output_lines.append(new_line)
        if (
            old_guarded in line
            and idx + 1 < len(input_lines)
            and is_header(input_lines[idx + 1])
        ):
            # Correct header underline length
            header_line = input_lines[idx + 1]
            header_character = header_line[0]
            output_lines.append(header_character * len(new_line))
            idx += 1
        idx += 1

    return output_lines


def inline_toc_header(input_lines):
    """
    The rst-files contain a manual ToC header which should
        1. Not be listed in the ToC
        2. Not influence the whole section hierarchy by grouping the whole
           document under it
    This function replaces it with a raw LaTeX header
    """
    output_lines = []
    line_idx = 0
    while line_idx < len(input_lines) - 1:
        # Check if the line is a heading by looking for 3 or more repeating
        # characters from the main list.
        possible_header = [
            'Table of contents',
            'Inhaltsverzeichnis',
            'Release Notes',
        ]

        if any(
            input_lines[line_idx].strip() == title for title in possible_header
        ) and is_header(input_lines[line_idx + 1]):
            output_lines.extend(
                wrap_latex([f'\\section*{{{input_lines[line_idx].strip()}}}'])
            )
            line_idx += 2
        else:
            output_lines.append(input_lines[line_idx].rstrip())
            line_idx += 1

    return output_lines


class BaseElement(ABC):
    @abstractmethod
    def to_rst(self) -> list[str]:
        pass


class Label(BaseElement):
    def __init__(self, input_string: str):
        super().__init__()
        self.label_id: str | None = self._parse_string(input_string)

    def _parse_string(self, label_line: str) -> str | None:
        label_match = re.match(r'.. _(.*):', label_line)
        if label_match:
            return label_match.group(1)
        return None

    def to_rst(self) -> list[str]:
        return wrap_latex(self.to_latex())

    def to_latex(self) -> list[str]:
        return [f'\\label{{{self.label_id}}}']


class Paragraph(BaseElement):
    def __init__(self, strings: list[str]):
        super().__init__()
        self.strings = strings

    def to_rst(self):
        return self.strings


class CSVTable(BaseElement):
    def __init__(self, label: Label, table_lines: list[str]):
        super().__init__()
        self.label = label
        self._parse_table_lines(table_lines)

    def _parse_table_lines(self, table_lines):
        line_idx = 0
        self.table_title_match = re.match(
            r'.. csv-table:: (.*)', table_lines[line_idx]
        )

        line_idx += 1
        self.csv_options = {}
        # Parse options
        while (
            line_idx < len(table_lines) and table_lines[line_idx].strip() != ''
        ):
            option_match = re.match(
                ':([a-zA-Z0-9]+): (.*)',
                table_lines[line_idx].strip(),
            )
            if option_match:
                option_key = option_match.group(1)
                option_value = option_match.group(2)
                self.csv_options[option_key] = option_value
            line_idx += 1

        line_idx += 1
        self.csv_rows = []
        # Split csv strings
        while (
            line_idx < len(table_lines)
            and not table_lines[line_idx].strip() == ''
        ):
            self.csv_rows.append(
                self.split_csv_string(table_lines[line_idx].strip())
            )
            line_idx += 1

        line_idx += 1
        self.footnotes = {}
        # Parse footnote definitions
        if line_idx < len(table_lines) and table_lines[
            line_idx
        ].strip().startswith('.. ['):
            while line_idx < len(table_lines) and table_lines[
                line_idx
            ].strip().startswith('.. ['):
                match = re.match(
                    r'.. \[(#.+?)\] (.*)',
                    table_lines[line_idx].strip(),
                )
                if match:
                    footnote_label = match.group(1)
                    footnote_text = match.group(2)
                    self.footnotes[footnote_label] = footnote_text
                    line_idx += 1
                    while table_lines[line_idx].strip() and not table_lines[
                        line_idx
                    ].strip().startswith('.. ['):
                        self.footnotes[footnote_label] = (
                            self.footnotes[footnote_label]
                            + ' '
                            + table_lines[line_idx].strip()
                        )
                        line_idx += 1

    def generate_labeled_tex_table(self):
        """
        Generate raw LaTeX table
        """
        latex_table = [
            '\\setcounter{originalfootnotevalue}{\\value{footnote}}',
            '\\begin{table}[h!]',
            '\\centering',
            '\\rowcolors{1}{table_even_row_color}{table_odd_row_color}',
        ]
        if self.table_title_match:
            latex_table.append(
                f'\\caption{{{self.table_title_match.group(1)}}}'
            )
        if self.label:
            latex_table.extend(self.label.to_latex())

        num_cols = 0
        if self.csv_rows:
            num_cols = max(len(row) for row in self.csv_rows)
        if num_cols > 0:
            # Define column widths
            if 'widths' in self.csv_options:
                widths = self.csv_options['widths'].split(', ')
                columns = [
                    f'p{{{int(width) / 120.0}\\linewidth}}' for width in widths
                ]
                latex_table.append(
                    '\\begin{tabular}{' + ' '.join(columns) + '}'
                )
            else:
                latex_table.append('\\begin{tabular}{' + 'l' * num_cols + '}')
            latex_table.append('\\toprule')

            # Define column titles
            if 'header' in self.csv_options:
                latex_table.append('\\rowcolor{table_header_color}')
                header_list = self.csv_options['header'].split(', ')
                latex_table.append(
                    ' & '.join([header.strip('"') for header in header_list])
                    + ' \\\\'
                )
                latex_table.append('\\midrule')

            # Add rows
            for row in self.csv_rows:
                latex_row = []
                for cell in row:
                    # Handle footnotes in cells
                    def replace_footnote(match):
                        footnote_ref = match.group(1)
                        if footnote_ref in self.footnotes:
                            key_list = list(self.footnotes.keys())
                            idx = key_list.index(footnote_ref) + 1
                            number = (
                                '\\numexpr\\value{originalfootnotevalue}'
                                + f'{idx}'
                            )
                            return f'\\footnotemark[{number}]'
                        return match.group(0)

                    latex_cell = re.sub(r'\[(#.+?)\]_', replace_footnote, cell)

                    # Remove string delimiters
                    latex_cell = latex_cell.strip("'")
                    # Escape asterisk
                    latex_cell = latex_cell.replace('\\*', '$*$')
                    latex_row.append(latex_cell)
                latex_table.append(' & '.join(latex_row) + ' \\\\')

            latex_table.append('\\bottomrule')
            latex_table.append('\\end{tabular}')
            latex_table.append('\\end{table}')

            # Append footnotes
            if self.footnotes:
                number_string = '\\numexpr\\value{originalfootnotevalue}+'
                footnote_counter = 1
                for _, text in self.footnotes.items():
                    number = number_string + f'{footnote_counter}'
                    latex_table.append(f'\\footnotetext[{number}]{{{text}}}')
                    footnote_counter += 1
                new_counter = number_string + f'{footnote_counter}'
                latex_table.append(
                    f'\\setcounter{{footnote}}{{{new_counter}}}'
                )
            latex_table.append('')

        return latex_table

    def split_csv_string(self, s):
        """
        Manually parses a single line of a CSV string, handling quoted fields.
        The csv module cannot correctly handle delimiters in strings.
        """
        delimiter = ','
        quote = '"'
        row = []
        in_string = False
        current_string = ''
        for c in s:
            if c == quote:
                if in_string:
                    in_string = False
                else:
                    in_string = True
                current_string = current_string + c
            elif c == delimiter and not in_string:
                row.append(current_string.strip().strip(quote))
                current_string = ''
            else:
                current_string = current_string + c
        if current_string and not in_string:
            row.append(current_string.strip().strip(quote))
        return row

    def to_rst(self):
        return wrap_latex(self.generate_labeled_tex_table())


class Header(BaseElement):
    def __init__(self, header, underline):
        super().__init__()
        self.header = header
        self.underline = underline

    def to_rst(self):
        return [self.header, self.underline]


class Figure(BaseElement):
    def __init__(self, label: Label, figure_lines: list[str]):
        super().__init__()
        self.label = label
        self._parse_figure_lines(figure_lines)

    def _parse_figure_lines(self, figure_lines):
        line_idx = 0
        self.figure_path_match = re.match(
            r'.. figure:: (.*)', figure_lines[line_idx]
        )

        line_idx += 2
        caption = []
        while line_idx < len(figure_lines) and figure_lines[line_idx].strip():
            caption.append(figure_lines[line_idx].strip())
            line_idx += 1
        self.caption = ' '.join(caption)

    def generate_labeled_tex_figure(self):
        """
        Generate raw LaTeX figure
        """
        output = []
        output.append('\\begin{figure}[htbp]')
        output.append('\\centering')
        if self.figure_path_match:
            match = self.figure_path_match.group(1)
            output.append(
                '\\includegraphics['
                + 'width=\\textwidth, keepaspectratio'
                + ']{'
                + f'{match}'
                + '}'
            )
        output.append(f'\\caption{{{self.caption}}}')
        if self.label:
            output.extend(self.label.to_latex())
        output.append('\\end{figure}')
        return output

    def to_rst(self):
        return wrap_latex(self.generate_labeled_tex_figure())


def extract_element_with_linebreaks(input_lines, start_idx, prefixes):
    """
    Helper function to extract the lines of an element containing line breaks
    by defining prefixes which define the element/block.
    """
    element_lines = [input_lines[start_idx]]
    line_idx = start_idx + 1
    while line_idx < len(input_lines) and (
        input_lines[line_idx] == ''
        or any(
            re.match(f'^[\t ]*{prefix}', input_lines[line_idx])
            for prefix in prefixes
        )
    ):
        element_lines.append(input_lines[line_idx])
        line_idx += 1
    return element_lines, line_idx


def parse_document(input_lines):
    """
    Parse the rst document into a list of internal elements deriving from
    BaseElement.
    """
    output_doc = []
    current_label = None
    elem_before_label = None
    current_paragraph = []
    line_idx = 0
    while line_idx < len(input_lines):
        line = input_lines[line_idx].rstrip()
        if line.startswith('.. _'):
            current_label = Label(line)
            elem_before_label = output_doc[-1]
            line_idx += 1
        elif line_idx < len(input_lines) - 1 and is_header(
            input_lines[line_idx + 1]
        ):
            header_line = input_lines[line_idx + 1].rstrip()
            output_doc.append(Header(line, header_line))
            if current_label:
                output_doc.append(current_label)
                current_label = None
            line_idx += 2
        elif line.startswith('.. csv-table::'):
            table_lines, line_idx = extract_element_with_linebreaks(
                input_lines, line_idx, ['   ', '\\.\\. \\[']
            )
            output_doc.append(CSVTable(current_label, table_lines))
            current_label = None
        elif line.startswith('.. figure::'):
            figure_lines, line_idx = extract_element_with_linebreaks(
                input_lines, line_idx, ['   ']
            )
            output_doc.append(Figure(current_label, figure_lines))
            current_label = None
        elif current_label and line.startswith('- '):
            list_lines, line_idx = extract_element_with_linebreaks(
                input_lines, line_idx, ['- ', '   ']
            )
            output_doc.append(Paragraph(list_lines))
            if current_label:
                output_doc.append(current_label)
                current_label = None
        elif current_label and line.startswith('.. versionadded::'):
            block_lines, line_idx = extract_element_with_linebreaks(
                input_lines, line_idx, ['   ']
            )
            output_doc.append(Paragraph(block_lines))
            if current_label:
                output_doc.append(current_label)
                current_label = None
        elif line:  # Non-blank line
            current_paragraph.append(line)
            line_idx += 1
        elif not line:  # Blank line
            if current_paragraph:
                output_doc.append(Paragraph(current_paragraph))
                current_paragraph = []
            if current_label and elem_before_label != output_doc[-1]:
                output_doc.append(current_label)
                current_label = None
            line_idx += 1

    return output_doc


def serialize_to_rst(elements):
    """
    Serialize the internal representation to rst.
    """
    output_lines = []
    for element in elements:
        output_lines.extend(element.to_rst())
        output_lines.append('')
    return output_lines


if __name__ == '__main__':
    parser = argparse.ArgumentParser(
        description='Preprocess rst files to assist pandoc in creating a11y '
        + 'PDFs'
    )

    parser.add_argument(
        'input_filepath', help='Path to the input file for processing.'
    )

    parser.add_argument(
        'output_filepath', help='Path to the output file for saving results.'
    )

    parser.add_argument(
        'app_name', help='Replace |AppName| with the provided value.'
    )

    parser.add_argument(
        'version', help='Replace |version| with the provided value.'
    )

    parser.add_argument(
        '-t',
        '--translate',
        type=str,
        default='',
        help='Translate file to given language if .po files can be found.',
    )

    if len(sys.argv) == 1:
        parser.print_help(sys.stderr)
        sys.exit(1)

    args = parser.parse_args()

    lines = flatten_files(args.input_filepath, 0, args.translate)

    lines = replace_placeholder(lines, 'AppName', args.app_name)
    lines = replace_placeholder(lines, 'version', args.version)

    lines = inline_toc_header(lines)

    elements = parse_document(lines)

    rst_lines = serialize_to_rst(elements)

    try:
        with open(args.output_filepath, 'w', encoding='utf-8') as outfile:
            outfile.write('\n'.join(rst_lines))
    except Exception as e:
        print(f'Error writing to output file: {e}')
        sys.exit(1)

    sys.exit(0)
