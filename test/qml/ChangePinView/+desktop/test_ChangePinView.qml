/**
 * Copyright (c) 2017-2025 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtTest
import Governikus.ChangePinView

TestCase {
	id: testCase

	function createTestObject() {
		return createTemporaryQmlObject("import Governikus.ChangePinView; ChangePinView { title: \"ChangePinView\" }", testCase);
	}
	function test_load() {
		let testObject = createTestObject();
		verify(testObject, "Object loaded");
	}

	name: "test_ChangePinView"
	visible: true
	when: windowShown
}
