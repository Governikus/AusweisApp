/**
 * Copyright (c) 2017-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtTest

TestCase {
	id: testCase

	readonly property var navBar: Item {
		readonly property bool lockedAndHiden: false
	}

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
