/**
 * Copyright (c) 2017-2023 Governikus GmbH & Co. KG, Germany
 */
import QtTest

TestCase {
	id: testCase

	function createTestObject() {
		return createTemporaryQmlObject("
			import QtQuick
			import QtQuick.Controls
			import Governikus.SetupAssistantView

			ApplicationWindow {
				menuBar: Item {
					function updateActions() {}
				}

				SetupAutostartView {}
			}
		", testCase);
	}
	function test_load() {
		let testObject = createTestObject();
		verify(testObject, "Object loaded");
	}

	name: "test_SetupAutostartView"
	visible: true
	when: windowShown
}
