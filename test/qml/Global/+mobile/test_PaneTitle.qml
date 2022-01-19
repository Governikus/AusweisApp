/*
 * \copyright Copyright (c) 2020-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12
import QtTest 1.12

import Governikus.Global 1.0

TestCase {
	id: testCase

	name: "test_PaneTitle"
	visible: true
	when: windowShown

	function createTestObject() {
		return createTemporaryQmlObject("import Governikus.Global 1.0; PaneTitle {}", testCase)
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

	function test_visible() {
		let testObject = createTestObject()

		compare(testObject.text, "", "Initial text empty")
		verify(!testObject.visible, "Initial visible: false")

		testObject.text = "test"
		verify(testObject.visible, "visible: true")
		testObject.text = ""
		verify(!testObject.visible, "visible: false")
	}

}
