/*
 * \copyright Copyright (c) 2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12
import QtTest 1.12

import Governikus.Global 1.0

TestCase {
	id: testCase

	name: "test_TitledSeparator"
	visible: true
	when: windowShown

	function createTestObject() {
		return createTemporaryQmlObject("import Governikus.Global 1.0; TitledSeparator {}", testCase)
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

	function test_title() {
		let testObject = createTestObject()

		compare(testObject.title, "", "Initial title: empty")
		testObject.title = "test"
		compare(testObject.title, "test", "title: test")
	}

}
