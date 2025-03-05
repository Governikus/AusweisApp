/**
 * Copyright (c) 2024-2025 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtTest

TestCase {
	id: testCase

	function createTestObject() {
		return createTemporaryQmlObject("import Governikus.RemoteServiceView; RemoteServiceSettings { }", testCase);
	}
	function test_load() {
		let testObject = createTestObject();
		verify(testObject, "Object loaded");
	}

	name: "test_RemoteServiceSettings"
	visible: true
	when: windowShown
}
