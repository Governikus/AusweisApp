import os
from abc import ABC, abstractmethod


def with_preconditions(**required):
    """Decorator that attaches required preconditions to a test method.
    Usage:
        @with_preconditions(show_onboarding=True, use_simulator=False)
        def test_onboarding(self):
        ...
    """

    def decorator(test_method):
        test_method.required_preconditions = required
        return test_method

    return decorator


def create_platform_preconditions(platform, required_preconditions):
    if os.getenv('AUSWEISAPP_INI_FILE'):
        from common.inipreconditions import IniPreconditions

        preconditions = IniPreconditions(os.getenv('AUSWEISAPP_INI_FILE'))
    elif platform == 'win32':
        from windows.windowspreconditions import WindowsPreconditions

        preconditions = WindowsPreconditions()
    elif platform == 'android':
        from android.androidpreconditions import AndroidPreconditions

        preconditions = AndroidPreconditions()
    elif platform == 'darwin':
        from macos.macospreconditions import MacosPreconditions

        preconditions = MacosPreconditions()
    elif platform == 'ios':
        from ios.iospreconditions import IosPreconditions

        preconditions = IosPreconditions()
    else:
        raise NotImplementedError(f'No Preconditions available for platform "{platform}"')

    for name, value in required_preconditions.items():
        if not hasattr(preconditions, name):
            raise AttributeError(f'Unknown precondition: {name}')
        setattr(preconditions, name, value)

    return preconditions


class Preconditions(ABC):
    def __init__(self):
        self.show_onboarding = False
        self.selfauth_test_uri = True
        self.show_transportpinreminder = False
        self.use_simulator = True
        self.remind_to_close = False
        self.developer_options = False
        self.developer_mode = False
        self.show_in_app_notifications = False

    @abstractmethod
    def apply(self):
        pass
