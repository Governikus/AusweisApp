#!/usr/bin/env python

import hglib
from hglib.util import b

import os
import sys


def main():
    if len(sys.argv) > 2:
        print('Unknown parameter: {}'.format(sys.argv))
        return -1

    patch = b(sys.argv[1]) if len(sys.argv) == 2 else b('../patch.diff')
    if not os.path.isfile(patch):
        if not os.path.isfile(patch):
            print('Patch file "{}" does not exists'.format(patch))
            return -1

    if 'BUILD_URL' not in os.environ:
        print('Warning: This script should be called on jenkins only')
        return -1

    cfg = ['extensions.hgext.purge=',
           'extensions.hgext.strip=',
           'patch.eol=auto',
           'phases.new-commit=draft']
    client = hglib.open(configs=cfg)

    print('Revert workspace...')
    client.revert(b('.'), all=True, nobackup=True)

    print('Purge workspace...')
    client.rawcommand([b('purge'), b('--all')])

    revs = len(client.log(revrange='secret() or draft()'))
    print('Found secret/draft changesets: {}'.format(revs))

    if revs > 0:
        print('Strip secret and draft changesets...')
        client.rawcommand([b('strip'),
                           b('-r'),
                           b('secret() or draft()'),
                           b('--no-backup'),
                           b('--force')])

    print('Import patch: {}'.format(patch))
    client.import_([patch], nocommit=True)

    return 0


if __name__ == '__main__':
    sys.exit(main())
