/*
 * \copyright Copyright (c) 2020-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12
import QtTest 1.12

import Governikus.Global 1.0

TestCase {
	id: testCase

	name: "test_NavigationButton"
	visible: true
	when: windowShown

	function createTestObject() {
		return createTemporaryQmlObject("import Governikus.Global 1.0; NavigationButton {}", testCase)
	}

	TestCase {
		when: testCase.completed
		optional: true

		function benchmark_load() {
			let testObject = createTestObject()
			testObject.destroy()
		}
	}

	function test_load() {
		let testObject = createTestObject()
		verify(testObject, "Object loaded")
	}

	function test_type() {
		let testObject = createTestObject()

		compare(testObject.buttonType, NavigationButton.Type.Forward, "Initial type: Forward")

		testObject.buttonType = NavigationButton.Type.Back
		compare(testObject.buttonType, NavigationButton.Type.Back, "type: Back")

		testObject.buttonType = NavigationButton.Type.Check
		compare(testObject.buttonType, NavigationButton.Type.Check, "type: Check")

		testObject.buttonType = NavigationButton.Type.Cancel
		compare(testObject.buttonType, NavigationButton.Type.Cancel, "type: Cancel")
	}

	function test_subtext() {
		let testObject = createTestObject()

		compare(testObject.text, "Continue", "Initial subText: Continue")

		testObject.subText = "test"
		compare(testObject.text, "test", "subText: test")
	}

}
