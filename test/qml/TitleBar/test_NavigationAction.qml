/**
 * Copyright (c) 2018-2025 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtTest

TestCase {
	id: testCase

	function createTestObject() {
		return createTemporaryQmlObject("import Governikus.TitleBar; NavigationAction {}", testCase);
	}
	function test_load() {
		let testObject = createTestObject();
		verify(testObject, "Object loaded");
	}

	name: "test_CancelAction"
	visible: true
	when: windowShown
}
