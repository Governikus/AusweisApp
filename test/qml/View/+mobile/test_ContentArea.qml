/**
 * Copyright (c) 2018-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtTest

TestCase {
	id: testCase

	function createTestObject() {
		return createTemporaryQmlObject("import Governikus.View; ContentArea { width: 500 }", testCase);
	}
	function test_load() {
		let testObject = createTestObject();
		verify(testObject, "Object loaded");
	}

	name: "test_ContentArea"
	visible: true
	when: windowShown
}
