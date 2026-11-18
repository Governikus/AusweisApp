import winreg

from common.preconditions import Preconditions


class WindowsPreconditions(Preconditions):
    def apply(self):
        self.set_show_onboarding(self.show_onboarding)
        self.set_use_selfauth_test_uri(self.selfauth_test_uri)
        self.set_use_simulator(self.use_simulator)
        self.set_show_transportpinreminder(self.show_transportpinreminder)
        self.set_remind_to_close(self.remind_to_close)
        self.set_developer_options(self.developer_options)
        self.set_developer_mode(self.developer_mode)
        self.set_in_app_notifications(self.show_in_app_notifications)

    def set_show_onboarding(self, value):
        self.write_registry_bool('showOnboarding', value)

    def set_use_selfauth_test_uri(self, value):
        self.write_registry_bool('selfauthTestUri', value)

    def set_use_simulator(self, value):
        self.write_registry_bool('enabled', value, r'\simulator')

    def set_show_transportpinreminder(self, value):
        self.write_registry_bool('transportPinReminder', value)

    def set_remind_to_close(self, value):
        self.write_registry_bool('remindToClose', value)

    def set_developer_options(self, value):
        self.write_registry_bool('developer_options', value)

    def set_developer_mode(self, value):
        self.write_registry_bool('developer_mode', value)

    def set_in_app_notifications(self, value):
        self.write_registry_bool('set_in_app_notifications', value)

    @staticmethod
    def write_registry_bool(key, value, append_key=None):
        WindowsPreconditions.write_registry_value(key, 'true' if value else 'false', append_key)

    @staticmethod
    def write_registry_value(key, value, append_key=None):
        reg_path = r'Software\Governikus GmbH & Co. KG\AusweisApp2'
        if append_key:
            reg_path += append_key

        try:
            base_path = winreg.OpenKey(winreg.HKEY_CURRENT_USER, reg_path, 0, winreg.KEY_SET_VALUE)
        except FileNotFoundError:
            base_path = winreg.CreateKey(winreg.HKEY_CURRENT_USER, reg_path)

        winreg.SetValueEx(base_path, key, 0, winreg.REG_SZ, value)
