/**
 * Copyright (c) 2017-2025 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtTest

TestCase {
	id: testCase

	function createTestObject() {
		return createTemporaryQmlObject("import Governikus.RemoteServiceView; RemoteServiceView { width: 500 }", testCase);
	}
	function test_load() {
		let testObject = createTestObject();
		verify(testObject, "Object loaded");
	}

	name: "test_RemoteServiceView"
	visible: true
	when: windowShown
}
