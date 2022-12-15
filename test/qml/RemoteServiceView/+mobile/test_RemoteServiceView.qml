/*
 * \copyright Copyright (c) 2017-2022 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import QtTest 1.15

TestCase {
	id: testCase

	readonly property var navBar: Item {
		readonly property bool lockedAndHiden: false
	}

	function createTestObject() {
		return createTemporaryQmlObject("import Governikus.RemoteServiceView 1.0; RemoteServiceView {}", testCase);
	}
	function test_load() {
		let testObject = createTestObject();
		verify(testObject, "Object loaded");
	}

	name: "test_RemoteServiceView"
	visible: true
	when: windowShown
}
