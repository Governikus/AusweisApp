from common.basetest import BaseTest


class TestAusweisApp(BaseTest):
    def test_pin_change(self):
        self.ensure_element_is_present('MainView_CheckDevice', 30)
        self.swipe_right()
        self.click_element('MainView_ChangePin', 30)
        self.click_element('PinSelectionButtons_sixDigitButton')
        self.click_element('TechnologyInfo_enableButton', 3)
        self.click_element('ResultView_button', 30)
        self.ensure_element_is_present('MainView_ChangePin')

    def test_self_auth(self):
        self.ensure_element_is_present('MainView_CheckDevice', 30)
        self.swipe_right()
        self.swipe_right()
        self.click_element('MainView_SelfAuthentication', 30)
        self.swipe_down()
        self.click_element('DecisionView_ok')
        self.click_element('EditRights_confirmButton', 30)
        self.click_element('TechnologyInfo_enableButton', 3)
        self.ensure_element_is_present('SelfAuthenticationData_backAction', 30)
        # Tap the "Remove card feedback" popup to allow proper swiping
        self.tap()
        self.swipe_down()
        self.click_element('SelfAuthenticationData_okButton')
        self.ensure_element_is_present('MainView_SelfAuthentication')
