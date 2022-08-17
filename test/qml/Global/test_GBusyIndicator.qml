/*
 * \copyright Copyright (c) 2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.15
import QtTest 1.15

import Governikus.Global 1.0

TestCase {
	id: testCase

	name: "test_GBusyIndicator"
	visible: true
	when: windowShown

	function createTestObject() {
		return createTemporaryQmlObject("import Governikus.Global 1.0; GBusyIndicator {}", testCase)
	}

	function test_load() {
		let testObject = createTestObject()
		verify(testObject, "Object loaded")
	}

	function test_factor() {
		let testObject = createTestObject()

		compare(testObject.factor, 1.1, "Initial factor: 1.1")

		testObject.factor = 1.2
		compare(testObject.factor, 1.2, "factor: 1.2")
	}

}
