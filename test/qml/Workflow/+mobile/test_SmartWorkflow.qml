/**
 * Copyright (c) 2021-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtTest
import Governikus.Global

TestCase {
	id: testCase

	function createTestObject() {
		return createTemporaryQmlObject("
			import QtQuick
			import Governikus.Workflow
			SmartWorkflow {
				workflowModel: Item {
					property bool isCurrentSmartCardAllowed: false
				}
				width: 420
				height: 420
			}
		", testCase);
	}
	function test_load() {
		let testObject = createTestObject();
		verify(testObject, "Object loaded");
	}

	name: "test_SmartWorkflow"
	visible: true
	when: windowShown
}
