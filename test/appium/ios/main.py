from common.basetest import BaseTest
from mobile.testausweisapp import TestAusweisApp


class TestAusweisAppIos(TestAusweisApp):
    BaseTest.platform = 'ios'
