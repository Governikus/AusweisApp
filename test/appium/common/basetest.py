import sys
import unittest

from appium import webdriver
from appium.webdriver.appium_service import AppiumService

from common.config import create_platform_options, hostname_and_port_from_env
from common.logfiles import copy_logfile
from common.mixins import ElementQueryMixin, NavigationMixin
from common.preconditions import create_platform_preconditions


class BaseTest(ElementQueryMixin, NavigationMixin, unittest.TestCase):
    platform = sys.platform

    def cleanup_appium(self):
        if hasattr(self, 'driver') and self.driver:
            self.driver.quit()
        if hasattr(self, 'service') and self.service:
            self.service.stop()

    def setUp(self):
        try:
            hostname, port = hostname_and_port_from_env()
            self.service = AppiumService()
            self.service.start(args=['--address', hostname, '--port', str(port)])
            if False in (self.service.is_running, self.service.is_listening):
                self.fail('Failed to start Appium service')

            test_method = getattr(self, self._testMethodName)
            required = getattr(test_method, 'required_preconditions', {})
            create_platform_preconditions(self.platform, required).apply()

            self.driver = webdriver.Remote(
                f'http://{hostname}:{port}',
                options=create_platform_options(self.platform),
            )
            if not self.driver:
                self.fail('Failed to initialize Appium driver')
        except Exception as e:
            self.cleanup_appium()
            raise

    def tearDown(self):
        log_name = f'{type(self).__name__}.{self._testMethodName}.log'
        error_string = copy_logfile(log_name)
        self.cleanup_appium()
        if error_string:
            self.fail(error_string)
