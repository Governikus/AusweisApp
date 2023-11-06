/**
 * Copyright (c) 2020-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtTest

TestCase {
	id: testCase

	readonly property var tabbedPane: Item {
		readonly property int availableHeight: 1000
	}

	function createTestObject() {
		return createTemporaryQmlObject("import Governikus.InformationView; ReleaseNotes { width: 500 }", testCase);
	}
	function test_load() {
		let testObject = createTestObject();
		verify(testObject, "Object loaded");
	}

	visible: true
	when: windowShown
}
