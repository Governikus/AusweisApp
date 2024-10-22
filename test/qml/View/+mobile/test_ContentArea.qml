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
		if (hasPolishLoop) {
			skip("Skip test because of old Qt version with unfixed Polish Loops");
		}
		let testObject = createTestObject();
		verify(testObject, "Object loaded");
	}

	name: "test_ContentArea"
	visible: true
	when: windowShown
}
