/*
 * \copyright Copyright (c) 2020-2022 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import QtTest 1.15

TestCase {
	id: testCase

	readonly property var tabbedPane: Item {
		readonly property int availableHeight: 1000
	}

	function createTestObject() {
		return createTemporaryQmlObject("import Governikus.InformationView 1.0; ReleaseNotes {}", testCase);
	}
	function test_load() {
		let testObject = createTestObject();
		verify(testObject, "Object loaded");
	}

	visible: true
	when: windowShown
}
