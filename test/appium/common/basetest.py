import os
import pathlib
import shutil
import sys
import threading
import time
import unittest
from queue import Queue

import httpx
from appium import webdriver
from appium.webdriver.appium_service import AppiumService
from appium.webdriver.common.appiumby import AppiumBy
from selenium.common import NoSuchElementException, TimeoutException

if sys.platform == 'darwin':
    from selenium.webdriver.support import expected_conditions as EC
    from selenium.webdriver.support.wait import WebDriverWait

from common.preconditions import create_platform_preconditions


class BaseTest(unittest.TestCase):
    platform = sys.platform

    def cleanup_appium(self):
        if hasattr(self, 'driver') and self.driver:
            self.driver.quit()
        if hasattr(self, 'service') and self.service:
            self.service.stop()

    def setUp(self):
        try:
            hostname, port = self.hostname_and_port_from_env()
            self.service = AppiumService()
            self.service.start(
                args=['--address', hostname, '--port', str(port)]
            )
            if False in (self.service.is_running, self.service.is_listening):
                self.fail('Failed to start Appium service')

            create_platform_preconditions(self.platform).apply()
            self.driver = webdriver.Remote(
                f'http://{hostname}:{port}',
                options=self.create_platform_options(),
            )
            if not self.driver:
                self.fail('Failed to initialize Appium driver')
        except Exception as e:
            print(e)
            self.cleanup_appium()

    @staticmethod
    def find_logfiles():
        log_dir = os.getenv('AUSWEISAPP_LOGFILE_FOLDER')
        if not log_dir:
            return (
                [],
                'AUSWEISAPP_LOGFILE_FOLDER is not defined, cannot retrieve logfiles',
            )

        logfiles = list(pathlib.Path(log_dir).glob('AusweisApp.*.log'))
        if len(logfiles) == 1:
            return logfiles, None

        return (
            [],
            f'Expected exactly one logfile in {log_dir} but found {logfiles}',
        )

    def copy_logfile(self):
        save_folder = os.getenv('APPIUM_LOGFILE_SAVE_FOLDER')
        if not save_folder:
            return 'APPIUM_LOGFILE_SAVE_FOLDER is not defined'

        logfiles, error_string = self.find_logfiles()
        if error_string:
            return error_string

        pathlib.Path(save_folder).mkdir(parents=True, exist_ok=True)

        shutil.copy(
            logfiles[0],
            os.path.join(save_folder, self._testMethodName + '.log'),
        )
        return None

    def tearDown(self):
        error_string = self.copy_logfile()
        self.cleanup_appium()
        if error_string:
            self.fail(error_string)

    def create_platform_options(self):
        if self.platform == 'win32':
            from windows.appiumwindowsoptions import AppiumWindowsOptions

            return AppiumWindowsOptions()
        elif self.platform == 'android':
            from android.appiumandroidoptions import AppiumAndroidOptions

            return AppiumAndroidOptions()
        elif self.platform == 'darwin':
            from macos.appiummacosoptions import AppiumMacosOptions

            return AppiumMacosOptions()
        elif self.platform == 'ios':
            from ios.appiumiosoptions import AppiumIosOptions

            return AppiumIosOptions()
        else:
            return None

    @staticmethod
    def hostname_and_port_from_env():
        try:
            hostname = os.environ['APPIUM_HOSTNAME']
        except KeyError:
            hostname = '127.0.0.1'
        try:
            port = os.environ['APPIUM_PORT']
        except KeyError:
            port = 4723
        return hostname, port

    def find_element(self, accessibility_id, timeout=1):
        self.driver.implicitly_wait(timeout)
        try:
            if self.platform in ('win32', 'ios'):
                element = self.driver.find_element(
                    by=AppiumBy.ACCESSIBILITY_ID, value=accessibility_id
                )
            elif self.platform == 'darwin':
                wait = WebDriverWait(self.driver, timeout)
                element = wait.until(
                    EC.visibility_of_element_located(
                        (AppiumBy.ACCESSIBILITY_ID, accessibility_id)
                    )
                )
            elif self.platform == 'android':
                element = self.driver.find_element(
                    by=AppiumBy.XPATH,
                    value=f"//*[@resource-id='{accessibility_id}']",
                )
            else:
                self.fail(
                    f'No element search strategy for platform '
                    f'"{self.platform}"'
                )
        except (NoSuchElementException, TimeoutException):
            return None
        else:
            return element

    @staticmethod
    def get_rect_center(rect):
        x, y = rect['x'] + rect['width'] / 2, rect['y'] + rect['height'] / 2
        return int(x), int(y)

    def click_element(self, accessibility_id, timeout=1):
        element = self.ensure_element_is_present(accessibility_id, timeout)
        if self.platform == 'win32':
            x, y = self.get_rect_center(element.rect)
            window_rect = self.driver.get_window_rect()
            x, y = x + window_rect['x'], y + window_rect['y']
            self.driver.execute_script('windows: click', {'x': x, 'y': y})
        else:
            element.click()

    def tap(self, position=(100, 100)):
        if self.platform == 'ios':
            x, y = position
            self.driver.execute_script('mobile: tap', {'x': x, 'y': y})

    def ensure_element_is_present(self, accessibility_id, timeout=1):
        element = self.find_element(accessibility_id, timeout)
        self.assertTrue(
            element, f'Element with id "{accessibility_id}" is not present'
        )
        return element

    def ensure_element_contains_text(self, accessibility_id, text, timeout=1):
        element = self.ensure_element_is_present(accessibility_id, timeout)
        element_text = element.get_attribute('value')
        self.assertIn(text, element_text)

    def ensure_text_in_view(self, text, timeout=1):
        if sys.platform == 'darwin':
            wait = WebDriverWait(self.driver, timeout)
            element = wait.until(
                EC.visibility_of_element_located(
                    (AppiumBy.IOS_PREDICATE, f"value CONTAINS '{text}'")
                )
            )
            return element
        else:
            return False

    def scroll_vertically(self, delta_y):
        x, y = self.get_rect_center(self.driver.get_window_rect())

        if self.platform == 'win32':
            self.driver.execute_script(
                'windows: scroll', {'x': x, 'y': y, 'deltaY': delta_y}
            )
        elif sys.platform == 'darwin':
            self.driver.execute_script(
                'macos: scroll',
                {'x': x, 'y': y, 'deltaX': 0, 'deltaY': delta_y * 10},
            )

        # Wait for Scroll Action to finish so the caller may click any Buttons
        # that became visible right away
        time.sleep(1)

    def swipe_right(self):
        rect = self.driver.get_window_rect()
        x, y = self.get_rect_center(rect)
        self.driver.swipe(x, y, x - rect['width'] / 2, y)

    def swipe_down(self):
        rect = self.driver.get_window_rect()
        x, y = self.get_rect_center(rect)
        self.driver.swipe(x, y, x, y - rect['height'] / 2)

    @staticmethod
    def bind_authentication(url, queue):
        timeout_config = httpx.Timeout(60.0, connect=10.0)
        with httpx.Client(
            follow_redirects=True, timeout=timeout_config
        ) as client:
            response = client.get(url)
            queue.put(response)

    def start_authentication(self, tcTokenUrl, bind=False):
        url = f'http://127.0.0.1:24727/eID-Client?tcTokenURL={tcTokenUrl}'
        result_queue = Queue()
        activation_thread = threading.Thread(
            target=BaseTest.bind_authentication, args=(url, result_queue)
        )
        activation_thread.start()

        if not bind:
            activation_thread.join(timeout=1)
            return None, None
        else:
            return activation_thread, result_queue
