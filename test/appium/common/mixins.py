import time

from appium.webdriver.common.appiumby import AppiumBy
from selenium.common import (
    NoSuchElementException,
    StaleElementReferenceException,
    TimeoutException,
)
from selenium.webdriver.support import expected_conditions as EC
from selenium.webdriver.support.wait import WebDriverWait

from common.utils import get_rect_center

WAIT_FOR_STATE_CHANGE_TIMEOUT = 2
WAIT_FOR_ELEMENT_LOOKUP_TIMEOUT = 1


class ElementQueryMixin:
    """Search and test elements and their attributes.

    expects self.driver, self.platform and unittest.TestCase.assert*
    """

    def find_element(self, accessibility_id, timeout=WAIT_FOR_ELEMENT_LOOKUP_TIMEOUT):
        if self.platform in ('win32', 'ios', 'darwin'):
            locator = (AppiumBy.ACCESSIBILITY_ID, accessibility_id)
        elif self.platform == 'android':
            locator = (AppiumBy.XPATH, f"//*[@resource-id='{accessibility_id}']")
        else:
            raise NotImplementedError(f'No element search strategy for platform "{self.platform}"')

        try:
            wait = WebDriverWait(self.driver, timeout)
            element = wait.until(EC.visibility_of_element_located(locator))
        except (NoSuchElementException, TimeoutException):
            return None
        else:
            return element

    def ensure_element_exists(
        self, accessibility_id, exists=True, timeout=WAIT_FOR_ELEMENT_LOOKUP_TIMEOUT
    ):
        if exists:
            element = self.find_element(accessibility_id, timeout)
            self.assertIsNotNone(element, f'Element with id "{accessibility_id}" is not present')
            return element

        try:
            WebDriverWait(self.driver, timeout).until_not(
                lambda _: self.find_element(accessibility_id, timeout=0)
            )
        except TimeoutException:
            self.fail(f'Element with id "{accessibility_id}" still exists after {timeout}s')
        return None

    def ensure_element_contains_text(
        self, accessibility_id, text, timeout=WAIT_FOR_ELEMENT_LOOKUP_TIMEOUT
    ):
        if self.platform != 'darwin':
            raise NotImplementedError(
                f'ensure_element_contains_text is not implemented for platform "{self.platform}"'
            )

        element = self.ensure_element_exists(accessibility_id, timeout=timeout)

        texts = {
            attribute: str(element.get_attribute(attribute) or '')
            for attribute in ('value', 'title', 'label')
        }
        self.assertTrue(
            any(text in value for value in texts.values()),
            f'Element with id "{accessibility_id}" does not contain "{text}", found {texts}',
        )
        return element

    def ensure_element_with_text_is_visible(self, text, timeout=WAIT_FOR_ELEMENT_LOOKUP_TIMEOUT):
        if self.platform != 'darwin':
            raise NotImplementedError(
                f'ensure_element_with_text_is_visible is not implemented for platform "{self.platform}"'
            )

        predicate = f"value CONTAINS '{text}' OR label CONTAINS '{text}' OR title CONTAINS '{text}'"

        element = None
        try:
            wait = WebDriverWait(self.driver, timeout)
            element = wait.until(
                EC.visibility_of_element_located((AppiumBy.IOS_PREDICATE, predicate))
            )
        except TimeoutException:
            pass

        self.assertIsNotNone(element, f'Element with text "{text}" is not present')
        return element

    def ensure_element_is_enabled(
        self,
        accessibility_id,
        enabled=True,
        timeout=WAIT_FOR_STATE_CHANGE_TIMEOUT,
    ):
        element = self._wait_for_element_state(
            accessibility_id,
            lambda e: e.is_enabled() == enabled,
            timeout,
        )
        self.assertEqual(
            enabled,
            element.is_enabled(),
            f'Element with id "{accessibility_id}" is not '
            f'{"enabled" if enabled else "disabled"} after {timeout}s',
        )
        return element

    def ensure_element_is_checked(
        self,
        accessibility_id,
        checked=True,
        timeout=WAIT_FOR_STATE_CHANGE_TIMEOUT,
    ):
        element = self._wait_for_element_state(
            accessibility_id, lambda e: self._is_checked(e) == checked, timeout
        )
        self.assertEqual(
            checked,
            self._is_checked(element),
            f'Element with id "{accessibility_id}" is not '
            f'{"checked" if checked else "unchecked"} after {timeout}s',
        )
        return element

    def _wait_for_element_state(self, accessibility_id, predicate, timeout):
        def element_in_state(_):
            element = self.find_element(accessibility_id, timeout=0)
            if element is not None and predicate(element):
                return element
            return None

        try:
            return WebDriverWait(
                self.driver, timeout, ignored_exceptions=(StaleElementReferenceException)
            ).until(element_in_state)
        except TimeoutException:
            pass

        return self.ensure_element_exists(accessibility_id, timeout=0)

    def _is_checked(self, element):
        if self.platform in ('darwin', 'ios'):
            return str(element.get_attribute('value')) == '1'
        if self.platform == 'android':
            return element.get_attribute('checked') == 'true'
        if self.platform == 'win32':
            return element.get_attribute('Toggle.ToggleState') in ('1', 'On')
        raise NotImplementedError(f'_is_checked not implemented for "{self.platform}"')


class NavigationMixin:
    """Navigate with click, tap, swipe and scroll.

    expects self.driver, self.platform and ElementQueryMixin
    """

    def click_element(self, accessibility_id, timeout=WAIT_FOR_ELEMENT_LOOKUP_TIMEOUT):
        element = self.ensure_element_exists(accessibility_id, timeout=timeout)
        if self.platform == 'win32':
            x, y = get_rect_center(element.rect)
            window_rect = self.driver.get_window_rect()
            x, y = x + window_rect['x'], y + window_rect['y']
            self.driver.execute_script('windows: click', {'x': x, 'y': y})
        else:
            element.click()

    def tap(self, position=(100, 100)):
        if self.platform == 'ios':
            x, y = position
            self.driver.execute_script('mobile: tap', {'x': x, 'y': y})

    def scroll_vertically(self, delta_y):
        x, y = get_rect_center(self.driver.get_window_rect())

        if self.platform == 'win32':
            self.driver.execute_script('windows: scroll', {'x': x, 'y': y, 'deltaY': delta_y})
        elif self.platform == 'darwin':
            self.driver.execute_script(
                'macos: scroll',
                {'x': x, 'y': y, 'deltaX': 0, 'deltaY': delta_y * 10},
            )

        # Wait for Scroll Action to finish so the caller may click any Buttons
        # that became visible right away
        time.sleep(1)

    def swipe_right(self):
        rect = self.driver.get_window_rect()
        x, y = get_rect_center(rect)
        self.driver.swipe(x, y, x - rect['width'] / 2, y)

    def swipe_down(self):
        rect = self.driver.get_window_rect()
        x, y = get_rect_center(rect)
        self.driver.swipe(x, y, x, y - rect['height'] / 2)
