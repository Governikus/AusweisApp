/*
 * \copyright Copyright (c) 2021-2022 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import QtTest 1.15
import Governikus.Global 1.0

TestCase {
	id: testCase
	function createTestObject() {
		return createTemporaryQmlObject("
			import QtQuick 2.15
			import Governikus.Workflow 1.0
			SmartWorkflow {
				workflowModel: Item {
					property bool isSmartCardAllowed: false
				}
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
