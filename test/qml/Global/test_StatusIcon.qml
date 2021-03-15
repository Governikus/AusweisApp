/*
 * \copyright Copyright (c) 2020-2021 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12
import QtTest 1.12

import Governikus.Global 1.0

TestCase {
	id: testCase

	name: "test_StatusIcon"
	visible: true
	when: windowShown

	function createTestObject() {
		return createTemporaryQmlObject("import Governikus.Global 1.0; StatusIcon { width: 64; height: 64 }", testCase)
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

		compare(testObject.text, "", "Initial text empty")

		testObject.text = "test"
		compare(testObject.text, "test", "Set text: test")
	}

	function test_busy() {
		let testObject = createTestObject()

		verify(!testObject.busy, "Initial busy: false")

		testObject.busy = true
		verify(testObject.busy, "Set busy: true")
	}

	function test_icon() {
		let testObject = createTestObject()

		compare(testObject.source, "", "Initial no image")
		testObject.source = "qrc:///images/material_check.svg"
		compare(testObject.source, "qrc:///images/material_check.svg", "Image: qrc:///images/material_check.svg")
	}

}
