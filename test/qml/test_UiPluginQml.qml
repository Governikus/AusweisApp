/**
 * Copyright (c) 2018-2025 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtTest
import Governikus.Type

TestCase {
	id: testCase

	function createTestObject() {
		return createTemporaryQmlObject("
			import QtQuick
			import Governikus.Type
			Item {
				readonly property int v1: UiModule.CURRENT
				readonly property int v2: UiModule.DEFAULT
				readonly property int v3: UiModule.IDENTIFY
				readonly property int v4: UiModule.PINMANAGEMENT
				readonly property int v5: UiModule.SETTINGS
				readonly property int v6: UiModule.HELP
				readonly property int v7: UiModule.SELF_AUTHENTICATION
				readonly property int v8: UiModule.UPDATEINFORMATION
				readonly property int v9: UiModule.REMOTE_SERVICE
				readonly property int v10: UiModule.CHECK_ID_CARD
				readonly property int v11: UiModule.SMART_EID

				readonly property var testVar: UiModule.DEFAULT
				readonly property int testInt: UiModule.DEFAULT
			}
			", testCase);
	}
	function test_compare() {
		let testObject = createTestObject();
		verify(testObject.testVar == UiModule.DEFAULT);
		verify(testObject.testVar === UiModule.DEFAULT);
		verify(testObject.testInt == UiModule.DEFAULT);
		verify(testObject.testInt === UiModule.DEFAULT);
		verify(UiModule.DEFAULT === UiModule.DEFAULT);
		verify(UiModule.DEFAULT == UiModule.DEFAULT);
		verify(testObject.testVar == testObject.testInt);
		verify(testObject.testVar === testObject.testInt);
	}
	function test_enumeration() {
		verify(UiModule.CURRENT !== UiModule.DEFAULT);
		verify(UiModule.DEFAULT !== UiModule.IDENTIFY);
		verify(UiModule.IDENTIFY !== UiModule.PINMANAGEMENT);
		verify(UiModule.SETTINGS !== UiModule.ONBOARDING);
		verify(UiModule.ONBOARDING !== UiModule.HELP);
		verify(UiModule.HELP !== UiModule.SELF_AUTHENTICATION);
		verify(UiModule.UPDATEINFORMATION !== UiModule.REMOTE_SERVICE);
		verify(UiModule.REMOTE_SERVICE !== UiModule.CHECK_ID_CARD);
		verify(UiModule.CHECK_ID_CARD !== UiModule.SMART_EID);
		verify(UiModule.SMART_EID !== UiModule.CURRENT);
	}
	function test_load() {
		let testObject = createTestObject();
		verify(testObject, "Object loaded");
	}
	function test_startupModule() {
		verify(SettingsModel.showOnboarding);
		let onboardingValue = UiModule.ONBOARDING;
		verify(SettingsModel.startupModule == onboardingValue);
		verify(SettingsModel.startupModule === onboardingValue);
		SettingsModel.onboardingShown = true;
		let initialValue = SettingsModel.startupModule;
		let newValue = UiModule.IDENTIFY;
		SettingsModel.startupModule = newValue;
		verify(SettingsModel.startupModule == newValue);
		verify(SettingsModel.startupModule === newValue);
		SettingsModel.onboardingShown = false;
		verify(SettingsModel.startupModule == onboardingValue);
		verify(SettingsModel.startupModule === onboardingValue);
		SettingsModel.showOnboarding = false;
		verify(SettingsModel.startupModule == newValue);
		verify(SettingsModel.startupModule === newValue);
		SettingsModel.startupModule = initialValue;
		verify(SettingsModel.startupModule == initialValue);
		verify(SettingsModel.startupModule === initialValue);
	}

	name: "test_UiPluginQml"
	visible: true
	when: windowShown
}
