/**
 * Copyright (c) 2020-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtTest
import Governikus.Global

TestCase {
	id: testCase

	function createTestObject() {
		return createTemporaryQmlObject("import Governikus.Global; GSeparator {}", testCase);
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
