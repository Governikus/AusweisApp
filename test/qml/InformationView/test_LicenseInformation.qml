/**
 * Copyright (c) 2020-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtTest
import Governikus.Global

TestCase {
	id: testCase

	readonly property var tabbedPane: Item {
		readonly property var availableHeight: 1000
	}

	function createTestObject() {
		return createTemporaryQmlObject("import Governikus.InformationView; LicenseInformation {}", testCase);
	}
	function test_load() {
		let testObject = createTestObject();
		verify(testObject, "Object loaded");
	}

	name: "test_LicenseInformation"
	visible: true
	when: windowShown
}
