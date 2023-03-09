/**
 * Copyright (c) 2018-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import QtTest 1.15

TestCase {
	id: testCase
	function createTestObject() {
		return createTemporaryQmlObject("import Governikus.View 1.0; FocusFrame {}", testCase);
	}
	function test_load() {
		let testObject = createTestObject();
		verify(testObject, "Object loaded");
	}

	name: "test_FocusFrame"
	visible: true
	when: windowShown
}
