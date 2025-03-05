/**
 * Copyright (c) 2024-2025 Governikus GmbH & Co. KG, Germany
 */
import QtTest

TestCase {
	id: testCase

	function createTestObject() {
		return createTemporaryQmlObject("import Governikus.Global; GStagedProgressBar {}", testCase);
	}
	function test_load() {
		let testObject = createTestObject();
		verify(testObject, "Object loaded");
	}

	name: "test_GStagedProgressBar"
	visible: true
	when: windowShown
}
