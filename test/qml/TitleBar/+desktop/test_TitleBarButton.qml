/**
 * Copyright (c) 2020-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtTest

TestCase {
	id: testCase

	function createTestObject() {
		return createTemporaryQmlObject("import Governikus.TitleBar; TitleBarButton {}", testCase);
	}
	function test_load() {
		let testObject = createTestObject();
		verify(testObject, "Object loaded");
	}

	name: "test_TitleBarButton"
	visible: true
	when: windowShown
}
