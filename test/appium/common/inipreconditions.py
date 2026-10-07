import configparser

from common.preconditions import Preconditions


class IniPreconditions(Preconditions):
    def __init__(self, ini_file):
        super().__init__()
        self.ini_file = ini_file

    @staticmethod
    def bool_to_string(value):
        return 'true' if value else 'false'

    def apply(self):
        config = configparser.ConfigParser()
        config.optionxform = str  # preserve case of option names

        config['General'] = {
            'selfauthTestUri': self.bool_to_string(self.selfauth_test_uri),
            'showOnboarding': self.bool_to_string(self.show_onboarding),
            'transportPinReminder': self.bool_to_string(
                self.show_transportpinreminder
            ),
            'remindToClose': self.bool_to_string(self.remind_to_close),
        }
        config['simulator'] = {
            'enabled': self.bool_to_string(self.use_simulator)
        }

        with open(self.ini_file, 'w') as f:
            config.write(f)
