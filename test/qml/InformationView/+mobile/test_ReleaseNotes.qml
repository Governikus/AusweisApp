/*
 * \copyright Copyright (c) 2021 Governikus GmbH & Co. KG, Germany
 */

import QtTest 1.12

TestCase {
	id: testCase

	visible: true
	when: windowShown

	function createTestObject() {
		return createTemporaryQmlObject("import Governikus.InformationView 1.0; ReleaseNotes {}", testCase)
	}

	function test_load() {
		let testObject = createTestObject()
		verify(testObject, "Object loaded")
	}
}
