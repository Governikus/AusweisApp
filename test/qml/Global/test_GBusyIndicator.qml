/**
 * Copyright (c) 2022-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtTest
import Governikus.Global

TestCase {
	id: testCase

	function createTestObject() {
		return createTemporaryQmlObject("import Governikus.Global; GBusyIndicator {}", testCase);
	}
	function test_factor() {
		let testObject = createTestObject();
		compare(testObject.factor, 1.1, "Initial factor: 1.1");
		testObject.factor = 1.2;
		compare(testObject.factor, 1.2, "factor: 1.2");
	}
	function test_load() {
		let testObject = createTestObject();
		verify(testObject, "Object loaded");
	}

	name: "test_GBusyIndicator"
	visible: true
	when: windowShown
}
