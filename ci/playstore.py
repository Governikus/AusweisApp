#!/usr/bin/env python

import argparse
import logging
import sys

from google.oauth2 import service_account
from googleapiclient.discovery import build
from googleapiclient.http import MediaFileUpload

logging.basicConfig(level=logging.ERROR)


class CommandLineParser(argparse.ArgumentParser):
    def __init__(self):
        super().__init__()
        self.add_argument('--json', help='Service account file', required=True)
        self.add_argument(
            '--package',
            help='Package name',
            default='com.governikus.ausweisapp2',
        )
        self.add_argument('--track', help='Release track', default='internal')
        self.add_argument(
            '--upload', nargs='+', help='List of files (APK, AAB) to upload'
        )
        self.add_argument(
            '--symbols',
            nargs='*',
            help=(
                'List of debug symbols file to upload. '
                'The filename has to match the corresponding apk file '
                'like "<apk-name>.apk.native-debug-symbols.zip"'
            ),
        )
        self.add_argument(
            '-v', '--verbose', help='Verbose output', action='store_true'
        )

        self.args = self.parse_args()
        for key, value in vars(self.args).items():
            setattr(self, key, value)


def FetchNextVersionCode(parser, service, edit_id):
    track_result = (
        service.edits()
        .tracks()
        .get(packageName=parser.package, editId=edit_id, track=parser.track)
        .execute()
    )

    releases = track_result.get('releases', [])
    all_version_codes = []
    for release in releases:
        codes = release.get('versionCodes', [])
        all_version_codes.extend(int(code) for code in codes)

    if all_version_codes:
        print(max(all_version_codes) + 1)
    else:
        print('0')


def UploadFiles(parser, service, edit_id):
    versionCodes = []

    for uploadFile in parser.upload:
        if uploadFile.endswith('.apk'):
            target = service.edits().apks()
            media = MediaFileUpload(
                uploadFile, mimetype='application/vnd.android.package-archive'
            )
        elif uploadFile.endswith('.aab'):
            target = service.edits().bundles()
            media = MediaFileUpload(
                uploadFile, mimetype='application/octet-stream'
            )
        else:
            print(f'Unknown file: {uploadFile}')
            sys.exit(-1)

        response = target.upload(
            editId=edit_id, packageName=parser.package, media_body=media
        ).execute()
        versionCodes.append(response['versionCode'])
        print(f'Version code {response["versionCode"]} has been uploaded')

        if not parser.symbols:
            continue

        for debugFile in parser.symbols:
            if debugFile == (f'{uploadFile}.native-debug-symbols.zip'):
                debugTarget = service.edits().deobfuscationfiles()
                debugMedia = MediaFileUpload(
                    debugFile, mimetype='application/octet-stream'
                )
                debugTarget.upload(
                    editId=edit_id,
                    packageName=parser.package,
                    media_body=debugMedia,
                    deobfuscationFileType='nativeCode',
                    apkVersionCode=response['versionCode'],
                ).execute()
                print(
                    f'Debug symbols for {response["versionCode"]}'
                    ' have been uploaded'
                )

    response = (
        service.edits()
        .tracks()
        .update(
            editId=edit_id,
            track=parser.track,
            packageName=parser.package,
            body={
                'releases': [{'versionCodes': versionCodes, 'status': 'draft'}]
            },
        )
        .execute()
    )
    print(f'Track {response["track"]} is ready')

    commit_request = (
        service.edits()
        .commit(editId=edit_id, packageName=parser.package)
        .execute()
    )
    print(f'Edit {commit_request["id"]} has been committed')


def main():
    parser = CommandLineParser()
    if parser.verbose:
        logging.root.setLevel(logging.DEBUG)

    SCOPES = ['https://www.googleapis.com/auth/androidpublisher']
    credentials = service_account.Credentials.from_service_account_file(
        parser.json, scopes=SCOPES
    )

    service = build('androidpublisher', 'v3', credentials=credentials)
    result = (
        service.edits().insert(body={}, packageName=parser.package).execute()
    )
    edit_id = result['id']

    if parser.upload:
        UploadFiles(parser, service, edit_id)
    else:
        FetchNextVersionCode(parser, service, edit_id)


if __name__ == '__main__':
    main()
