import os

from appium.options.mac import Mac2Options


class AppiumMacosOptions(Mac2Options):
    def __init__(self):
        super().__init__()

        if os.getenv('APPIUM_APP_PATH'):
            self.app_path = os.getenv('APPIUM_APP_PATH')
            self.skip_app_kill = False
            self.no_reset = False
        else:
            self.bundle_id = 'com.governikus.ausweisapp2'
            self.skip_app_kill = True
            self.no_reset = True
