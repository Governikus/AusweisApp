import os
from abc import ABC, abstractmethod


def create_platform_preconditions(platform):
    if os.getenv('AUSWEISAPP_INI_FILE'):
        from common.inipreconditions import IniPreconditions

        return IniPreconditions(os.getenv('AUSWEISAPP_INI_FILE'))

    if platform == 'win32':
        from windows.windowspreconditions import WindowsPreconditions

        return WindowsPreconditions()
    elif platform == 'android':
        from android.androidpreconditions import AndroidPreconditions

        return AndroidPreconditions()
    elif platform == 'darwin':
        from macos.macospreconditions import MacosPreconditions

        return MacosPreconditions()
    elif platform == 'ios':
        from ios.iospreconditions import IosPreconditions

        return IosPreconditions()
    else:
        return None


class Preconditions(ABC):
    def __init__(self):
        self.show_onboarding = False
        self.selfauth_test_uri = True
        self.show_transportpinreminder = False
        self.use_simulator = True
        self.remind_to_close = False

    @abstractmethod
    def apply(self):
        pass
