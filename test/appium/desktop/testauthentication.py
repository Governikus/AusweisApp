import threading
import unittest
from queue import Queue

import httpx
from common.basetest import BaseTest
from common.panstarhtmlparser import PanstarHtmlParser
from ddt import data, ddt


def bind_authentication(url, queue):
    timeout_config = httpx.Timeout(60.0, connect=10.0)
    with httpx.Client(follow_redirects=True, timeout=timeout_config) as client:
        response = client.get(url)
        queue.put(response)


def start_authentication(tcTokenUrl, bind=False):
    url = f'http://127.0.0.1:24727/eID-Client?tcTokenURL={tcTokenUrl}'
    result_queue = Queue()
    activation_thread = threading.Thread(target=bind_authentication, args=(url, result_queue))
    activation_thread.start()

    if not bind:
        activation_thread.join(timeout=1)
        return None, None
    else:
        return activation_thread, result_queue


@ddt
class TestAuthentication(BaseTest):
    def test_self_auth(self):
        self.click_element('MainView_SelfAuthentication')
        self.click_element('DecisionView_ok')
        self.click_element('EditRights_confirmButton', 30)
        self.ensure_element_exists('SelfAuthenticationData_successText', timeout=30)
        self.scroll_vertically(-10)
        self.click_element('SelfAuthenticationData_okButton')
        self.ensure_element_exists('MainView_SelfAuthentication')

    @data(
        ('https://www.governikus.de', 'Get_TcToken_Invalid_Data'),
        ('https://ausweisapp.bund.de', 'Get_TcToken_Invalid_Server_Reply'),
    )
    def test_faulty_external_authentication(self, value):
        tcTokenUrl, failure_code = value
        start_authentication(tcTokenUrl)
        self.click_element('ResultView_details', timeout=10)
        self.ensure_element_contains_text('BaseConfirmationPopup_mainText', failure_code)
        self.ensure_element_contains_text('BaseConfirmationPopup_mainText', tcTokenUrl)
        self.click_element('ConfirmationPopup_ok')
        self.click_element('ResultView_button')

    def test_external_authentication(self):
        thread, queue = start_authentication(
            'https://test.governikus-eid.de/Autent-DemoApplication/api/eid/request?dateOfExpiry&#61;ALLOWED&#38;givenNames&#61;ALLOWED&#38;familyName&#61;ALLOWED&#38;dateOfBirth&#61;ALLOWED&#38;placeOfBirth&#61;ALLOWED&#38;nationality&#61;ALLOWED&#38;restrictedId&#61;ALLOWED',
            bind=True,
        )
        confirm_rights = self.find_element('EditRights_confirmButton', 30)
        if not confirm_rights and self.find_element('ResultView_button'):
            self.fail(
                'The external authentication failed to start and no Access Rights are present'
            )

        self.click_element('EditRights_providerInfoButton')
        self.ensure_element_with_text_is_visible('Governikus Service GmbH')
        self.click_element('TitleBar_navigationAction')
        self.click_element('EditRights_confirmButton')
        self.click_element('ResultView_button', 30)
        thread.join(timeout=30)
        if not queue.empty():
            result = queue.get()
            self.assertEqual(result.status_code, 200)
            parser = PanstarHtmlParser()
            parser.feed(result.text)
            self.assertTrue(parser.data_present('Family names', 'ALLOWED', 'MUSTERMANN'))
            self.assertTrue(parser.data_present('Given names', 'ALLOWED', 'ERIKA'))
            self.assertTrue(parser.data_present('Date of expiry', 'ALLOWED', '2034-06-30+02:00'))
            self.assertTrue(parser.data_present('Date of birth', 'ALLOWED', '19840812'))
            self.assertTrue(parser.data_present('Place of birth', 'ALLOWED', 'BERLIN'))
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
