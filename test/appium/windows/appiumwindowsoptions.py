import win32gui
from appium.options.windows import WindowsOptions


class AppiumWindowsOptions(WindowsOptions):
    def __init__(self):
        super().__init__()

        hwnd = AppiumWindowsOptions.find_ausweisapp_window()
        if hwnd:
            print('Using existing AusweisApp window with handle', hwnd)
            self.app_top_level_window = hwnd
        else:
            print('Start new AusweisApp instance')
            self.app = 'C:\\Program Files\\AusweisApp\\AusweisApp.exe'

    @staticmethod
    def find_ausweisapp_window():
        def enum_handler(hwnd, output):
            if 'AusweisApp' == win32gui.GetWindowText(hwnd):
                output.append(hex(hwnd))
                return False

        handle = []
        win32gui.EnumWindows(enum_handler, handle)
        return handle[0] if len(handle) == 1 else None
