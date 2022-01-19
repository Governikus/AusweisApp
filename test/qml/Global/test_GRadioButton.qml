/*
 * \copyright Copyright (c) 2020-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12
import QtTest 1.12

import Governikus.Global 1.0
import Governikus.Style 1.0

TestCase {
	id: testCase

	name: "test_GRadioButton"
	visible: true
	when: windowShown

	function createTestObject() {
		return createTemporaryQmlObject("import Governikus.Global 1.0; GRadioButton {}", testCase)
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

	function test_textStyle() {
		let testObject = createTestObject()

		compare(testObject.textStyle, Style.text.normal, "Initial textStyle: normal")

		testObject.textStyle = Style.text.hint_warning
		compare(testObject.textStyle, Style.text.hint_warning, "textStyle: hint_warning")
	}

	function test_text() {
		let testObject = createTestObject()

		compare(testObject.text, "", "Initial empty text")

		testObject.text = "test"
		compare(testObject.text, "test", "text: test")
	}

	function test_tintIcon() {
		let testObject = createTestObject()

		verify(!testObject.tintIcon,"Initial tintIcon: false")

		testObject.tintIcon = true
		verify(testObject.tintIcon, "tintIcon: true")
	}

	function test_icon() {
		let testObject = createTestObject()

		compare(testObject.icon.source, "", "Initial no image")
		testObject.icon.source = "qrc:///images/material_check.svg"
		compare(testObject.icon.source, "qrc:///images/material_check.svg", "Image: qrc:///images/material_check.svg")
	}

	Item {
		GRadioButton {
			id: button_a
			checked: true
		}

		GRadioButton {
			id: button_b
		}

		TestCase {
			when: windowShown

			function test_selection() {
				verify(button_a.checked, "button_a checked: true")
				verify(!button_b.checked, "button_b checked: false")

				mouseClick(button_b)

				verify(!button_a.checked, "button_a checked: false")
				verify(button_b.checked, "button_b checked: true")
			}

		}
	}

}
