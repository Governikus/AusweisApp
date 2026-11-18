import unittest

from common.basetest import BaseTest


class TestPinManagement(BaseTest):
    def test_pin_change(self):
        self.click_element('MainView_PinManagement')
        self.click_element('PinSelectionButtons_sixDigitButton')
        self.click_element('ResultView_button', 30)
        self.ensure_element_exists('MainView_PinManagement')


if __name__ == '__main__':
    unittest.main()
