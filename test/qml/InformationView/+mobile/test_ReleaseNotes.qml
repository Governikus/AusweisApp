/**
 * Copyright (c) 2020-2025 Governikus GmbH & Co. KG, Germany
 */
import QtTest

TestCase {
	id: testCase

	function createTestObject() {
		return createTemporaryQmlObject("import Governikus.InformationView; ReleaseNotes { width: 500 }", testCase);
	}
	function test_load() {
		let testObject = createTestObject();
		verify(testObject, "Object loaded");
	}

	visible: true
	when: windowShown
}
