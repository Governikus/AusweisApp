/*
 * \copyright Copyright (c) 2020-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12
import QtTest 1.12

import Governikus.Global 1.0

TestCase {
	id: testCase

	name: "test_GCheckBox"
	visible: true
	when: windowShown

	function createTestObject() {
		return createTemporaryQmlObject("import Governikus.Global 1.0; GCheckBox {}", testCase)
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

	function test_checked() {
		let testObject = createTestObject()

		verify(!testObject.checked, "Initial checked: false")
		testObject.checked = true
		verify(testObject.checked, "checked: true")
		testObject.checked = false
		verify(!testObject.checked, "checked: false")
	}

	function test_text() {
		let testObject = createTestObject()
		testObject.text = "testText"
		verify(testObject.text == "testText", "Object text can be set")
	}

	GCheckBox {
		id: testObject

		TestCase {
			when: windowShown

			function test_click() {
				testObject.checked = false

				verify(!testObject.checked, "Initial checked: false")
				mouseClick(testObject)
				tryVerify(function(){ return testObject.checked })
			}
		}

	}
}
