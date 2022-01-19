/*
 * \copyright Copyright (c) 2020-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12
import QtTest 1.12

import Governikus.Global 1.0

TestCase {
	id: testCase

	name: "test_ToggleableOption"
	visible: true
	when: windowShown

	function createTestObject() {
		return createTemporaryQmlObject("import Governikus.Global 1.0; ToggleableOption {}", testCase)
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

	function test_text() {
		let testObject = createTestObject()

		compare(testObject.text, "", "Initial text: empty")
		testObject.text = "test"
		compare(testObject.text, "test", "text: test")
	}

	function test_checked() {
		let testObject = createTestObject()

		verify(!testObject.checked, "Initial checked: false")
		testObject.checked = true
		verify(testObject.checked, "checked: true")
		testObject.checked = false
		verify(!testObject.checked, "checked: false")
	}

	ToggleableOption {
		id: testObject

		TestCase {
			when: windowShown

			function test_click() {
				testObject.checked = false

				verify(!testObject.checked, "Initial checked: false")
				mouseClick(testObject)
				tryVerify(function(){ return testObject.checked })
			}

			function test_spacePress() {
				testObject.checked = false

				verify(!testObject.checked, "Initial checked: false")
				testObject.forceActiveFocus()
				verify(testObject.focus)
				keyClick(Qt.Key_Space)
				tryVerify(function(){ return testObject.checked })
			}
		}

	}
}
