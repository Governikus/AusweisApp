/**
 * Copyright (c) 2020-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtTest
import Governikus.Global

TestCase {
	id: testCase

	function createTestObject() {
		return createTemporaryQmlObject("import Governikus.Global; RetryCounter { width: 64; height: 64 }", testCase);
	}
	function test_load() {
		let testObject = createTestObject();
		verify(testObject, "Object loaded");
	}

	name: "test_RetryCounter"
	visible: true
	when: windowShown
}
