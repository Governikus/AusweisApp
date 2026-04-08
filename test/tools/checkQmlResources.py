#! /usr/bin/env python

import argparse
import os
import re
import sys


class CommandLineParser(argparse.ArgumentParser):
    def __init__(self):
        super().__init__()
        self.add_argument(
            '--module-dir', help='Directory of QML modules', required=True
        )
        self.add_argument(
            '--resources-dir', help='Directory of QML resources', required=True
        )
        self.add_argument(
            '--exclude-filenames',
            help='Filenames to exclude from QML module check',
        )
        self.add_argument(
            '--exclude-resources',
            help='Path to textfile that defines excludes for QML'
            'resource usage check',
        )
        self.args = self.parse_args()

    def qml_module_dir(self):
        return self.args.module_dir

    def resources_dir(self):
        return self.args.resources_dir

    def exclude_filenames(self):
        if self.args.exclude_filenames is None:
            return []
        return self.args.exclude_filenames.split(',')

    def exclude_resources(self):
        return self.args.exclude_resources


class QmlFileCache:
    def __init__(self, qml_dir):
        self.cache = dict()
        self.qml_dir = qml_dir
        if not self.qml_dir.endswith('/'):
            self.qml_dir += '/'

        for path in self.collect_qml_files(qml_dir):
            with open(path, encoding='utf8') as fp:
                self.cache[path] = [
                    line.strip() for line in fp.read().splitlines()
                ]

    def collect_qml_files(self, qml_dir):
        for root, _, files in os.walk(qml_dir):
            for file in files:
                if os.path.splitext(file)[-1].lower() != '.qml':
                    continue

                path = os.path.join(root, file)
                yield path

    def is_empty(self):
        return len(self.cache.keys()) == 0

    def in_cache(self, search_string):
        pattern = re.compile(rf'\b{search_string}')
        for lines in self.cache.values():
            for line in lines:
                if re.search(pattern, line):
                    return True

        return False

    def is_singleton(self, filename):
        if filename not in self.cache:
            return False

        lines = self.cache[filename]
        return 'pragma Singleton' in lines

    def unused_qml_files(self, exclude_filenames):
        def strip_basepath(path):
            return file.split(self.qml_dir)[-1]

        unused_files = []
        for file in self.cache.keys():
            filename, ext = os.path.splitext(os.path.split(file)[-1])

            if filename in exclude_filenames:
                print(f'Skipping {file}')
                continue

            if self.is_singleton(file):
                if not self.in_cache(filename + '.'):
                    print(f'QML singleton unused {strip_basepath(file)}')
                    unused_files.append(strip_basepath(file))
            elif not self.in_cache(filename + ' {'):
                print(f'QML file unused {strip_basepath(file)}')
                unused_files.append(strip_basepath(file))

        return unused_files

    def platform_selector_conflict(self):
        conflicts = list()

        for file in self.cache.keys():
            if '+' not in file:
                continue

            filename = os.path.basename(file)
            dirname = os.path.dirname(file)
            while '+' in dirname:
                dirname = dirname[: dirname.rfind('+')]
                higher_level_name = os.path.join(dirname, filename)
                if higher_level_name in self.cache.keys():
                    print(
                        f'Platform specific component {file} has higher level counterpart {higher_level_name}'
                    )
                    conflicts.append(file)

        return conflicts

    @staticmethod
    def root_id_on_top_level_item(lines):
        top_level_item_found = False
        top_level_item_pattern = re.compile('\\w+ {')

        for line in lines:
            if not line or line.startswith(('/', '*')):
                continue

            if not top_level_item_found:
                if top_level_item_pattern.match(line):
                    top_level_item_found = True
                continue

            return line == 'id: root'

        return False

    def id_error_in_component(self):
        files_with_id_errors = []
        id_pattern = re.compile(r'id:\s*(\w+)')

        for file, lines in self.cache.items():
            for line in lines:
                match = id_pattern.match(line)
                if not match:
                    continue

                id_string = match.group(1)
                if id_string == 'root':
                    if not self.root_id_on_top_level_item(lines):
                        print(f'"root" id not on top level item in {file}')
                        files_with_id_errors.append(file)

                id_found = any(
                    id_string in other_line
                    for other_line in lines
                    if other_line is not line
                )

                if not id_found:
                    print(f'QML id "{id_string}" not used in {file}')
                    if file not in files_with_id_errors:
                        files_with_id_errors.append(file)

        return files_with_id_errors


class ResourceCache:
    def __init__(self, resources_dir):
        self.language_names = ['de', 'en', 'uk', 'ru']
        self.qrc_content = set()
        self.duplicate_files = []
        for qrc_file in self.collect_qrc_files(resources_dir):
            with open(qrc_file, encoding='utf8') as fp:
                for line in [line.strip() for line in fp.read().splitlines()]:
                    m = re.match('<file>(.+)</file>', line)
                    if m is None:
                        continue

                    if m.group(1) in self.qrc_content:
                        self.duplicate_files.append(m.group(1))
                        print(f'Duplicate: {qrc_file} {m.group(1)}')

                    self.qrc_content.add(m.group(1))

    def collect_qrc_files(self, resources_dir):
        for root, _, files in os.walk(resources_dir):
            for file in files:
                if os.path.splitext(file)[-1].lower() != '.qrc':
                    continue

                path = os.path.join(root, file)
                yield path

    def is_empty(self):
        return len(self.qrc_content) == 0

    def resource_file_missing(self, resources_dir):
        missing_files = []
        for resource in self.qrc_content:
            if not os.path.exists(os.path.join(resources_dir, resource)):
                print(f'Missing: {resource}')
                missing_files.append(resource)

        return missing_files

    def resource_unused(self, qml_cache, excluded_resources):
        unused_resources = []
        for line in sorted(self.qrc_content):
            if excluded_resources.is_excluded(line):
                print(f'Skipping: {line}')
                continue

            if qml_cache.in_cache(line):
                continue

            if self.is_language_aware_resource(line):
                language_filename = self.language_aware_filename(line)
                if not qml_cache.in_cache(language_filename):
                    print(f'UNUSED language aware files: {line}')
                    unused_resources.append(line)
            else:
                print(f'UNUSED: {line}')
                unused_resources.append(line)

        return unused_resources

    def is_language_aware_resource(self, resource_file):
        return self.language_aware_filename(resource_file) is not None

    def language_aware_filename(self, resource_file):
        filename, ext = os.path.splitext(resource_file)
        for language_name in self.language_names:
            pattern = re.compile('_' + language_name + '$')
            if re.search(pattern, filename):
                return re.sub(pattern, '_%1', filename) + ext

        return None


class ExcludedResources:
    def __init__(self, excludes_filename):
        if excludes_filename:
            with open(excludes_filename, encoding='utf8') as fp:
                self.excluded_resources = [
                    re.compile(line.strip()) for line in fp.readlines()
                ]
        else:
            self.excluded_resources = []

    def is_excluded(self, filename):
        return any(
            [
                expression.fullmatch(filename)
                for expression in self.excluded_resources
            ]
        )


if __name__ == '__main__':

    def print_if_not_empty(list_obj, message):
        if len(list_obj):
            print(f'\n{message}')
            for line in list_obj:
                print(f'  {line}')

    parser = CommandLineParser()

    qml_cache = QmlFileCache(parser.qml_module_dir())
    if qml_cache.is_empty():
        print(f'Found no QML files in {parser.qml_module_dir()}')
        sys.exit(1)

    resource_cache = ResourceCache(parser.resources_dir())
    if resource_cache.is_empty():
        print(f'Found no resources in {parser.resources_dir()}')
        sys.exit(1)

    excluded_resources = ExcludedResources(parser.exclude_resources())

    unused_qml_files = qml_cache.unused_qml_files(parser.exclude_filenames())
    missing_resource_files = resource_cache.resource_file_missing(
        parser.resources_dir()
    )
    duplicate_files = resource_cache.duplicate_files
    unused_resources = resource_cache.resource_unused(
        qml_cache, excluded_resources
    )

    platform_conflicts = qml_cache.platform_selector_conflict()
    files_with_id_errors = qml_cache.id_error_in_component()

    if any(
        [
            len(unused_qml_files),
            len(missing_resource_files),
            len(duplicate_files),
            len(unused_resources),
            len(platform_conflicts),
            len(files_with_id_errors),
        ]
    ):
        print('\n')
        print_if_not_empty(unused_qml_files, 'Unused QML file(s):')
        print_if_not_empty(missing_resource_files, 'Resource file(s) missing:')
        print_if_not_empty(duplicate_files, 'Duplicated resources:')
        print_if_not_empty(unused_resources, 'Unused resources:')
        print_if_not_empty(platform_conflicts, 'Platform conflicts:')
        print_if_not_empty(files_with_id_errors, 'QML id errors:')
        sys.exit(1)
    else:
        sys.exit(0)
