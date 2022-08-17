/*
 * \copyright Copyright (c) 2020-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.15
import QtTest 1.15

import Governikus.Global 1.0
import Governikus.Style 1.0

TestCase {
	id: testCase

	name: "test_LogView"
	visible: true
	when: windowShown

	function createTestObject() {
		return createTemporaryQmlObject("import Governikus.FeedbackView 1.0; LogView {}", testCase)
	}

	function test_load() {
		let testObject = createTestObject()
		verify(testObject, "Object loaded")
	}

}
