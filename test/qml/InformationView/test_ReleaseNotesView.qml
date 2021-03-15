/*
 * \copyright Copyright (c) 2021 Governikus GmbH & Co. KG, Germany
 */

import QtTest 1.12

import Governikus.Global 1.0

TestCase {
	id: testCase

	name: "test_ReleaseNotesView"
	visible: true
	when: windowShown

	function createTestObject() {
		return createTemporaryQmlObject("import Governikus.InformationView 1.0; ReleaseNotesView {}", testCase)
	}

	function test_load() {
		let testObject = createTestObject()
		verify(testObject, "Object loaded")
	}
}
