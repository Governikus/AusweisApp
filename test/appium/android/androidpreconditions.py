import os
import sys
from tempfile import NamedTemporaryFile

from common.preconditions import Preconditions
from ppadb.client import Client as AdbClient


class AndroidPreconditions(Preconditions):
    DEVICE_TMP_PATH = r'/data/local/tmp/AusweisApp2.conf'
    SETTINGS_PATH = (
        r'files/settings/Governikus\ GmbH\ \&\ Co.\ KG/AusweisApp2.conf'
    )

    def apply(self):
        client = AdbClient()
        devices = client.devices()

        if len(devices) != 1:
            raise IndexError('Exactly one adb device is required.')

        device = devices[0]
        self.write_to_device(device)

    @staticmethod
    def bool_to_string(value):
        return 'true' if value else 'false'

    def write_to_device(self, device):
        def copy_to_device(filename, device):
            print('Writing', filename, 'to adb device', device.serial)
            device.push(filename, self.DEVICE_TMP_PATH)
            device.shell(
                f'run-as com.governikus.ausweisapp2 cp '
                f'{self.DEVICE_TMP_PATH} {self.SETTINGS_PATH}'
            )

        not_windows = sys.platform != 'win32'

        with NamedTemporaryFile(
            mode='w', delete=not_windows, delete_on_close=not_windows
        ) as fp:
            fp.write('[General]\n')
            fp.write(
                f'selfauthTestUri = {self.bool_to_string(self.selfauth_test_uri)}\n'
            )
            fp.write(
                f'showOnboarding = {self.bool_to_string(self.show_onboarding)}\n'
            )
            fp.write(
                f'transportPinReminder = {self.bool_to_string(self.show_transportpinreminder)}\n'
            )
            if self.use_simulator:
                fp.write('preferredTechnology = SIMULATOR\n')

            fp.write('\n[simulator]\n')
            fp.write(f'enabled = {self.bool_to_string(self.use_simulator)}\n')
            fp.flush()

            tmp_filename = fp.name
            if not_windows:
                copy_to_device(tmp_filename, device)
                return

        if not_windows is False:
            copy_to_device(tmp_filename, device)
            print('Deleting temporary file', tmp_filename)
            os.remove(tmp_filename)
