import os

from common.preconditions import Preconditions


class MacosPreconditions(Preconditions):
    def apply(self):
        s = f'"showOnboarding" = {self.show_onboarding}; '
        s += f'"selfauthTestUri" = {self.selfauth_test_uri}; '
        s += f'"simulator.enabled" = {self.use_simulator}; '
        s += f'"transportPinReminder" = {self.show_transportpinreminder}; '
        s += f'"remindToClose" = {self.remind_to_close};'
        s = f"'{{ {s} }}'"

        ausweisapp_domain = '~/Library/Containers/com.governikus.ausweisapp2/Data/Library/Preferences/com.governikus.AusweisApp2.plist'
        os.system(f'defaults write {ausweisapp_domain} {s}')
