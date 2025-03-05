#! /usr/bin/env python

import argparse
import os
import re


class CommandLineParser(argparse.ArgumentParser):
    def __init__(self):
        super().__init__()
        self.add_argument("--module-dir", help="Directory of QML modules",
                          required=True)
        self.add_argument("--resources-dir", help="Directory of QML resources",
                          required=True)
        self.add_argument("--exclude-filenames",
                          help="Filenames to exclude from QML module check")
        self.add_argument("--exclude-resources",
                          help="Path to textfile that defines excludes for QML"
                               "resource usage check")
        self.args = self.parse_args()

    def qml_module_dir(self):
        return self.args.module_dir

    def resources_dir(self):
        return self.args.resources_dir

    def exclude_filenames(self):
        if self.args.exclude_filenames is None:
            return []
        return self.args.exclude_filenames.split(",")

    def exclude_resources(self):
        return self.args.exclude_resources


class QmlFileCache:
    def __init__(self, qml_dir):
        self.cache = dict()
        for path in self.collect_qml_files(qml_dir):
            with open(path, "r", encoding="utf8") as fp:
                self.cache[path] = [line.strip() for line
                                    in fp.read().splitlines()]

    def collect_qml_files(self, qml_dir):
        for root, _, files in os.walk(qml_dir):
            for file in files:
                if os.path.splitext(file)[-1].lower() != ".qml":
                    continue

                path = os.path.join(root, file)
                yield path

    def is_empty(self):
        return len(self.cache.keys()) == 0

    def in_cache(self, search_string):
        pattern = re.compile(r"\b%s" % search_string)
        for lines in self.cache.values():
            for line in lines:
                if re.search(pattern, line):
                    return True

        return False

    def is_singleton(self, filename):
        if filename not in self.cache:
            return False

        lines = self.cache[filename]
        return "pragma Singleton" in lines

    def unused_qml_files(self, exclude_filenames):
        found_unused_file = False
        for file in self.cache.keys():
            filename, ext = os.path.splitext(os.path.split(file)[-1])

            if filename in exclude_filenames:
                print("Skipping", file)
                continue

            if self.is_singleton(file):
                if not self.in_cache(filename + "."):
                    print("QML singleton unused", file)
                    found_unused_file = True
            elif not self.in_cache(filename + " {"):
                print("QML file unused", file)
                found_unused_file = True

        return found_unused_file


class ResourceCache:
    def __init__(self, resources_dir):
        self.language_names = ["de", "en", "uk", "ru"]
        self.qrc_content = set()
        self.found_duplicate = False
        for qrc_file in self.collect_qrc_files(resources_dir):
            with open(qrc_file, "r", encoding="utf8") as fp:
                for line in [line.strip() for line in fp.read().splitlines()]:
                    m = re.match("<file>(.+)</file>", line)
                    if m is None:
                        continue

                    if m.group(1) in self.qrc_content:
                        self.found_duplicate = True
                        print("Duplicate QML resource in", qrc_file,
                              m.group(1))

                    self.qrc_content.add(m.group(1))

    def collect_qrc_files(self, resources_dir):
        for root, _, files in os.walk(resources_dir):
            for file in files:
                if os.path.splitext(file)[-1].lower() != ".qrc":
                    continue

                path = os.path.join(root, file)
                yield path

    def is_empty(self):
        return len(self.qrc_content) == 0

    def resource_file_missing(self, resources_dir):
        file_missing = False
        for resource in self.qrc_content:
            if not os.path.exists(os.path.join(resources_dir, resource)):
                print("QML resource is missing", resource)
                file_missing = True

        return file_missing

    def has_duplicate(self):
        return self.found_duplicate

    def resource_unused(self, qml_cache, excluded_resources):
        resource_unused = False
        for line in sorted(self.qrc_content):
            if excluded_resources.is_excluded(line):
                print("QML resource is excluded", line)
                continue

            if self.is_language_aware_resource(line):
                language_filename = self.language_aware_filename(line)
                if not qml_cache.in_cache(language_filename):
                    print("Language aware QML resource is unused", line)
                    resource_unused = True
            elif not qml_cache.in_cache(line):
                print("QML resource is unused", line)
                resource_unused = True

        return resource_unused

    def is_language_aware_resource(self, resource_file):
        return self.language_aware_filename(resource_file) is not None

    def language_aware_filename(self, resource_file):
        filename, ext = os.path.splitext(resource_file)
        for language_name in self.language_names:
            pattern = re.compile("_" + language_name + "$")
            if re.search(pattern, filename):
                return re.sub(pattern, "_%1", filename) + ext

        return None


class ExcludedResources:
    def __init__(self, excludes_filename):
        if excludes_filename:
            with open(excludes_filename, "r", encoding="utf8") as fp:
                self.excluded_resources = [re.compile(line.strip()) for line
                                           in fp.readlines()]
        else:
            self.excluded_resources = []

    def is_excluded(self, filename):
        return any([expression.fullmatch(filename) for expression
                    in self.excluded_resources])


if __name__ == "__main__":
    parser = CommandLineParser()

    qml_cache = QmlFileCache(parser.qml_module_dir())
    if qml_cache.is_empty():
        print("Found no QML files in", parser.qml_module_dir())
        exit(1)

    resource_cache = ResourceCache(parser.resources_dir())
    if resource_cache.is_empty():
        print("Found no QML resources in", parser.resources_dir())
        exit(1)

    excluded_resources = ExcludedResources(parser.exclude_resources())

    has_error = qml_cache.unused_qml_files(parser.exclude_filenames())
    has_error |= resource_cache.resource_file_missing(parser.resources_dir())
    has_error |= resource_cache.has_duplicate()
    has_error |= resource_cache.resource_unused(qml_cache, excluded_resources)

    exit(has_error)
