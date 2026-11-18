import unittest

from common.basetest import BaseTest
from common.preconditions import with_preconditions


class TestDeveloperMode(BaseTest):
    def test_t694_enable_dev_mode(self):
        self.click_element('MainView_HelpTile')
        self.click_element('TabbedPaneDelegate_2')

        for _ in range(10):
            self.click_element('VersionInformation_LabeledText_0')

        self.click_element('TitleBar_navigationAction')
        self.click_element('MainView_SettingsTile')
        self.click_element('TabbedPaneDelegate_5')
        self.click_element('DeveloperSettings_DeveloperModeSwitch')

        self.ensure_element_is_checked('DeveloperSettings_DeveloperModeSwitch')
        self.ensure_element_is_enabled('DeveloperSettings_ShowInAppNotificationsSwitch', False)
        self.ensure_element_is_checked('DeveloperSettings_ShowInAppNotificationsSwitch')

        self.click_element('TitleBar_navigationAction')

        self.ensure_element_exists('DeveloperModeWarning_Button')

    @with_preconditions(developer_options=True, developer_mode=True)
    def test_t695_disable_dev_mode(self):
        self.click_element('DeveloperModeWarning_Button')
        self.ensure_element_exists('DeveloperModeWarning_Button', False)

    @with_preconditions(
        developer_options=True, developer_mode=False, show_in_app_notifications=False
    )
    def test_t696_disable_dev_mode_with_settings(self):
        self.ensure_element_exists('TitleBar_notifyButton', False)

        self.click_element('MainView_SettingsTile')
        self.click_element('TabbedPaneDelegate_5')
        self.click_element('DeveloperSettings_DeveloperModeSwitch')

        self.ensure_element_is_enabled('DeveloperSettings_ShowInAppNotificationsSwitch', False)
        self.ensure_element_is_checked('DeveloperSettings_ShowInAppNotificationsSwitch')
        self.ensure_element_exists('TitleBar_notifyButton')

        self.click_element('DeveloperSettings_DeveloperModeSwitch')

        self.ensure_element_is_enabled('DeveloperSettings_ShowInAppNotificationsSwitch', True)
        self.ensure_element_is_checked('DeveloperSettings_ShowInAppNotificationsSwitch', False)

        self.click_element('TitleBar_navigationAction')

        self.ensure_element_exists('DeveloperModeWarning_Button', False)

    @with_preconditions(developer_options=True, developer_mode=True, show_in_app_notifications=True)
    def test_t697_disable_dev_mode_with_disabling_dev_options(self):
        self.ensure_element_exists('TitleBar_notifyButton')

        self.click_element('MainView_HelpTile')
        self.click_element('TabbedPaneDelegate_2')

        for _ in range(10):
            self.click_element('VersionInformation_LabeledText_0')

        self.ensure_element_exists('TitleBar_notifyButton', False)

        self.click_element('TitleBar_navigationAction')

        self.ensure_element_exists('DeveloperModeWarning_Button', False)


if __name__ == '__main__':
    unittest.main()
