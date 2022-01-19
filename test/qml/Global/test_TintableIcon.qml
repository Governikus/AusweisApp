/*
 * \copyright Copyright (c) 2020-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12
import QtTest 1.12

import Governikus.Global 1.0
import Governikus.Style 1.0

TestCase {
	id: testCase

	name: "test_TintableIcon"
	visible: true
	when: windowShown

	function createTestObject() {
		return createTemporaryQmlObject("import Governikus.Global 1.0; TintableIcon {}", testCase)
	}

	TestCase {
		when: testCase.completed
		optional: true

		TestCase {
			when: testCase.completed
			optional: true

			function benchmark_load() {
				let testObject = createTestObject()
				testObject.destroy()
			}
		}
	}

	function test_load() {
		let testObject = createTestObject()
		verify(testObject, "Object loaded")
	}

	function test_tintEnabled() {
		let testObject = createTestObject()

		verify(testObject.tintEnabled, "Initial tintEnabled: true")

		testObject.tintEnabled = false
		verify(!testObject.tintEnabled, "Set tintEnabled: false")
	}

	function test_tintColor() {
		let testObject = createTestObject()

		compare(testObject.tintColor, Style.color.primary_text, "Initial tintColor: primary_text")

		testObject.tintColor = "#000000"
		compare(testObject.tintColor, "#000000", "Set tintColor")
	}

	function test_icon() {
		let testObject = createTestObject()

		compare(testObject.source, "", "Initial no image")
		testObject.source = "qrc:///images/material_check.svg"
		compare(testObject.source, "qrc:///images/material_check.svg", "Image: qrc:///images/material_check.svg")
	}

	function test_fillMode() {
		let testObject = createTestObject()

		compare(testObject.fillMode, Image.PreserveAspectFit, "Initial fillMode: PreserveAspectFit")
	}

}
