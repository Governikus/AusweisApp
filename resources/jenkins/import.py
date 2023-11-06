#!/usr/bin/env python

import hglib
from hglib.util import b

import os
import sys

patchy = None

try:
    from rbtools.commands.patch import Patch

    class PatchHelper(Patch):
        def __init__(self):
            super(PatchHelper, self).__init__()
            parser = self.create_arg_parser([])
            self.options = parser.parse_args([])
            self._api_root = None
            if self.options.server is None:
                self.options.server = os.environ.get('REVIEWBOARD_SERVER')

            # RBTools >= 2.0
            self.v1 = not hasattr(self, 'get_patches')

        def download(self, request_id, diff_rev=None):
            if request_id is None:
                print('ID of review request cannot be undefined')
                return None

            if self._api_root is None:
                if self.options.server is None:
                    print('Cannot find Review Board! Provide .reviewboardrc?')
                    return None
                _, self._api_root = self.get_api(self.options.server)

                if not self.v1:
                    _, self._tool = self.initialize_scm_tool(
                        require_repository_info=False
                    )
                    self.setup_tool(self._tool, api_root=self._api_root)

            print('Download patch revision {} '
                  'of review request {}'.format(diff_rev, request_id))

            if self.v1:
                diff, _, _ = self.get_patch(request_id, self._api_root,
                                            diff_rev)
            else:
                self._review_request_id = request_id
                diff = self.get_patches(diff_rev, squashed=True)[0]['diff']

            return diff

        def save(self, file, request_id=None, diff_rev=None):
            if request_id is None:
                request_id = os.environ.get('REVIEWBOARD_REVIEW_ID')

            if diff_rev is None:
                diff_rev = os.environ.get('REVIEWBOARD_DIFF_REVISION')

            diff = self.download(request_id, diff_rev)

            if diff is not None:
                with open(file, 'wb') as f:
                    f.write(diff)
                    f.flush()

    patchy = PatchHelper()
except Exception:
    print('Cannot import rbtools')


def main():
    if len(sys.argv) > 2:
        print('Unknown parameter: {}'.format(sys.argv))
        return -1

    patch = b(sys.argv[1]) if len(sys.argv) == 2 else b('../patch.diff')
    if not os.path.isfile(patch):
        if patchy is not None:
            print('Try to ask Review Board for patch file')
            patchy.save(patch)

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
