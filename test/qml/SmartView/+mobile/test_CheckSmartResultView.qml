/**
 * Copyright (c) 2023-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtTest
import Governikus.Type

TestCase {
	id: testCase

	function test_load(data) {
		const component = Qt.createComponent(importPrefix + "/Governikus/SmartView/CheckSmartResultView.qml");
		verify(component.status === Component.Ready, "Component loaded");
		const testObject = component.createObject(parent, {
			result: data.result
		});
		verify(testObject, "Object loaded");
	}
	function test_load_data() {
		return [
			{
				"result": SmartModel.State.UPDATING_STATUS
			},
			{
				"result": SmartModel.State.UNAVAILABLE
			},
			{
				"result": SmartModel.State.UNUSABLE
			},
			{
				"result": SmartModel.State.NO_PROVISIONING
			},
			{
				"result": SmartModel.State.NO_PERSONALIZATION
			},
			{
				"result": -1
			}
		];
	}

	name: "test_CheckSmartResultView"
	visible: true
	when: windowShown
}
