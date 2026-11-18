import os


def create_platform_options(platform):
    if platform == 'win32':
        from windows.appiumwindowsoptions import AppiumWindowsOptions

        return AppiumWindowsOptions()
    elif platform == 'android':
        from android.appiumandroidoptions import AppiumAndroidOptions

        return AppiumAndroidOptions()
    elif platform == 'darwin':
        from macos.appiummacosoptions import AppiumMacosOptions

        return AppiumMacosOptions()
    elif platform == 'ios':
        from ios.appiumiosoptions import AppiumIosOptions

        return AppiumIosOptions()
    else:
        return None


def hostname_and_port_from_env():
    try:
        hostname = os.environ['APPIUM_HOSTNAME']
    except KeyError:
        hostname = '127.0.0.1'
    try:
        port = os.environ['APPIUM_PORT']
    except KeyError:
        port = 4723
    return hostname, port
