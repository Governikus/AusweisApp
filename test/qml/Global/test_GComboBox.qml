/*
 * \copyright Copyright (c) 2020-2021 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12
import QtTest 1.12

import Governikus.Global 1.0
import Governikus.Style 1.0

TestCase {
	id: testCase

	name: "test_GComboBox"
	visible: true
	when: windowShown

	function createTestObject() {
		return createTemporaryQmlObject("import Governikus.Global 1.0; GComboBox {}", testCase)
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

	function test_initial() {
		let testObject = createTestObject()

		compare(testObject.count, 0, "count: -1")
		compare(testObject.currentIndex, -1, "currentIndex: -1")
		verify(!testObject.indicator.visible, "Indicator visible: false")
	}

	GComboBox {
		id: testObject

		textStyle: Style.text.hint

		model: ["a", "b", "c", "d"]

		TestCase {
			when: windowShown

			function test_model() {
				compare(testObject.count, 4, "count: 4")
				verify(testObject.indicator.visible, "Indicator visible: true")

				compare(testObject.currentIndex, 0, "currentIndex: 0")
				compare(testObject.currentText, "a", "currentText: a")

				testObject.currentIndex = 1
				compare(testObject.currentIndex, 1, "currentIndex: 1")
				compare(testObject.currentText, "b", "currentText: b")
			}

			function test_click() {
				verify(!testObject.popup.visible, "Initial popup visible: false")
				mouseClick(testObject)
				tryVerify(function(){ return testObject.popup })
				compare(testObject.popup.visible, true)
				mouseClick(testObject)
				tryVerify(function(){ return testObject.popup })
				compare(testObject.popup.visible, false)
			}
		}

	}

}
