import unittest

from common.basetest import BaseTest
from common.panstarhtmlparser import PanstarHtmlParser
from ddt import data, ddt


@ddt
class TestAusweisApp(BaseTest):
    def test_pin_change(self):
        self.click_element('MainView_PinManagement')
        self.click_element('PinSelectionButtons_sixDigitButton')
        self.click_element('ResultView_button', 30)
        self.ensure_element_is_present('MainView_PinManagement')

    def test_self_auth(self):
        self.click_element('MainView_SelfAuthentication')
        self.click_element('DecisionView_ok')
        self.click_element('EditRights_confirmButton', 30)
        self.ensure_element_is_present(
            'SelfAuthenticationData_successText', 30
        )
        self.scroll_vertically(-10)
        self.click_element('SelfAuthenticationData_okButton')
        self.ensure_element_is_present('MainView_SelfAuthentication')

    @data(
        ('https://www.governikus.de', 'Get_TcToken_Invalid_Data'),
        ('https://ausweisapp.bund.de', 'Get_TcToken_Invalid_Server_Reply'),
    )
    def test_faulty_external_authentication(self, value):
        tcTokenUrl, failure_code = value
        self.start_authentication(tcTokenUrl)
        self.click_element('ResultView_details', timeout=10)
        self.ensure_element_contains_text(
            'BaseConfirmationPopup_mainText', failure_code
        )
        self.ensure_element_contains_text(
            'BaseConfirmationPopup_mainText', tcTokenUrl
        )
        self.click_element('ConfirmationPopup_ok')
        self.click_element('ResultView_button')

    def test_external_authentication(self):
        thread, queue = self.start_authentication(
            'https://test.governikus-eid.de/Autent-DemoApplication/api/eid/request?dateOfExpiry&#61;ALLOWED&#38;givenNames&#61;ALLOWED&#38;familyName&#61;ALLOWED&#38;dateOfBirth&#61;ALLOWED&#38;placeOfBirth&#61;ALLOWED&#38;nationality&#61;ALLOWED&#38;restrictedId&#61;ALLOWED',
            bind=True,
        )
        confirm_rights = self.find_element('EditRights_confirmButton', 30)
        if not confirm_rights and self.find_element('ResultView_button'):
            self.fail(
                'The external authentication failed to start and no Access Rights are present'
            )

        self.click_element('EditRights_providerInfoButton')
        self.ensure_text_in_view('Governikus Service GmbH')
        self.click_element('TitleBar_navigationAction')
        self.click_element('EditRights_confirmButton')
        self.click_element('ResultView_button', 30)
        thread.join(timeout=30)
        if not queue.empty():
            result = queue.get()
            self.assertEqual(result.status_code, 200)
            parser = PanstarHtmlParser()
            parser.feed(result.text)
            self.assertTrue(
                parser.data_present('Family names', 'ALLOWED', 'MUSTERMANN')
            )
            self.assertTrue(
                parser.data_present('Given names', 'ALLOWED', 'ERIKA')
            )
            self.assertTrue(
                parser.data_present(
                    'Date of expiry', 'ALLOWED', '2034-06-30+02:00'
                )
            )
            self.assertTrue(
                parser.data_present('Date of birth', 'ALLOWED', '19840812')
            )
            self.assertTrue(
                parser.data_present('Place of birth', 'ALLOWED', 'BERLIN')
            )
            self.assertTrue(parser.data_present('Nationality', 'ALLOWED', 'D'))
            self.assertTrue(
                parser.data_present(
                    'Pseudonym',
                    'ALLOWED',
                    '35a993fb392f1e950d12a9945c8ed5910387fbd16c324efcf52d51ad6aed60dc',
                )
            )
        else:
            self.fail('No data received from authentication workflow')


if __name__ == '__main__':
    unittest.main()
