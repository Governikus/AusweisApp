/**
 * Copyright (c) 2020-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import QtTest 1.15
import Governikus.Global 1.0

TestCase {
	id: testCase
	function createTestObject() {
		return createTemporaryQmlObject("import Governikus.Global 1.0; GSeparator {}", testCase);
	}
	function test_load() {
		let testObject = createTestObject();
		verify(testObject, "Object loaded");
	}
	function test_orientation() {
		let testObject = createTestObject();
		compare(testObject.orientation, Qt.Horizontal, "Initial orientation: Qt.Horizontal");
		testObject.orientation = Qt.Vertical;
		compare(testObject.orientation, Qt.Vertical, "orientation: Qt.Vertical");
	}

	name: "test_GSeparator"
	visible: true
	when: windowShown
}
