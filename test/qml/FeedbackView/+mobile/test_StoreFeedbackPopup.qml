/*
 * \copyright Copyright (c) 2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12
import QtTest 1.12

import Governikus.Global 1.0
import Governikus.Style 1.0

TestCase {
	id: testCase

	name: "test_StoreFeedbackPopup"
	visible: true
	when: windowShown

	function createTestObject() {
		return createTemporaryQmlObject("import Governikus.FeedbackView 1.0; StoreFeedbackPopup {}", testCase)
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

}
