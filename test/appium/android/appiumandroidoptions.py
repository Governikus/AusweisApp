from appium.options.android import UiAutomator2Options


class AppiumAndroidOptions(UiAutomator2Options):
    def __init__(self):
        super().__init__()

        self.app_package = 'com.governikus.ausweisapp2'
        self.app_activity = 'com.governikus.ausweisapp2.MainActivity'
        self.no_reset = True
        self.capabilities['appium:forceAppLaunch'] = True
