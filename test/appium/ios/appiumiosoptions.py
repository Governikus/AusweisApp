import subprocess

from appium.options.ios import XCUITestOptions


class AppiumIosOptions(XCUITestOptions):
    def __init__(self):
        super().__init__()

        self.udid = self.detect_udid()
        self.bundle_id = 'com.governikus.ausweisapp2'
        self.updated_wda_bundle_id = 'com.governikus.appiumdriver'
        self.xcode_org_id = 'G7EQCJU4BR'
        self.xcode_signing_id = 'iPhone Developer'

    def detect_udid(self):
        try:
            output = (
                subprocess.check_output(['idevice_id', '-l']).decode().strip()
            )
        except FileNotFoundError:
            print(
                '"idevice_id" not found. Make sure libimobiledevice is '
                'installed.'
            )
        else:
            udids = output.splitlines()

        if len(udids) != 1:
            raise IndexError('Exactly ONE connected iPhone is required.')
        return udids[0]
