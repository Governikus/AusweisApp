from common.basetest import BaseTest
from mobile.testausweisapp import TestAusweisApp


class TestAusweisAppAndroid(TestAusweisApp):
    BaseTest.platform = 'android'
