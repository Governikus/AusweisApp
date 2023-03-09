/**
 * Copyright (c) 2018-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import QtTest 1.15
import Governikus.Type.UiModule 1.0
import Governikus.Type.SettingsModel 1.0

TestCase {
	id: testCase
	function createTestObject() {
		return createTemporaryQmlObject("
			import QtQuick 2.15
			import Governikus.Type.UiModule 1.0
			Item {
				readonly property int v1: UiModule.CURRENT
				readonly property int v2: UiModule.DEFAULT
				readonly property int v3: UiModule.IDENTIFY
				readonly property int v4: UiModule.PINMANAGEMENT
				readonly property int v5: UiModule.SETTINGS
				readonly property int v6: UiModule.HELP
				readonly property int v7: UiModule.PROVIDER
				readonly property int v8: UiModule.SELF_AUTHENTICATION
				readonly property int v9: UiModule.HISTORY
				readonly property int v10: UiModule.UPDATEINFORMATION
				readonly property int v11: UiModule.REMOTE_SERVICE
				readonly property int v12: UiModule.CHECK_ID_CARD
				readonly property int v13: UiModule.SMART

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
		verify(UiModule.SETTINGS !== UiModule.TUTORIAL);
		verify(UiModule.TUTORIAL !== UiModule.HELP);
		verify(UiModule.HELP !== UiModule.PROVIDER);
		verify(UiModule.PROVIDER !== UiModule.SELF_AUTHENTICATION);
		verify(UiModule.SELF_AUTHENTICATION !== UiModule.HISTORY);
		verify(UiModule.HISTORY !== UiModule.UPDATEINFORMATION);
		verify(UiModule.UPDATEINFORMATION !== UiModule.REMOTE_SERVICE);
		verify(UiModule.REMOTE_SERVICE !== UiModule.CHECK_ID_CARD);
		verify(UiModule.CHECK_ID_CARD !== UiModule.SMART);
		verify(UiModule.SMART !== UiModule.CURRENT);
	}
	function test_load() {
		let testObject = createTestObject();
		verify(testObject, "Object loaded");
	}
	function test_startupModule() {
		let initialValue = SettingsModel.startupModule;
		let newValue = UiModule.IDENTIFY;
		SettingsModel.startupModule = newValue;
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
