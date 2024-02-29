/**
 * Copyright (c) 2023-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import QtTest 1.15
import Governikus.Type.SmartModel 1.0

TestCase {
	id: testCase

	function createTestObject(result) {
		return createTemporaryQmlObject("import \"qrc:/qml/Governikus/SmartView/+mobile\"; CheckSmartResultView { result: %1 }".arg(result), testCase);
	}
	function test_load(data) {
		let testObject = createTestObject(data.result);
		verify(testObject, "Object loaded");
	}
	function test_load_data() {
		return [{
				"result": SmartModel.SMART_UPDATING_STATUS
			}, {
				"result": SmartModel.SMART_UNAVAILABLE
			}, {
				"result": SmartModel.SMART_UNUSABLE
			}, {
				"result": SmartModel.SMART_NO_PROVISIONING
			}, {
				"result": SmartModel.SMART_NO_PERSONALIZATION
			}, {
				"result": -1
			}];
	}

	name: "test_CheckSmartResultView"
	visible: true
	when: windowShown
}
