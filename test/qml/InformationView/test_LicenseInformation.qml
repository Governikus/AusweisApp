/**
 * Copyright (c) 2020-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import QtTest 1.15
import Governikus.Global 1.0

TestCase {
	id: testCase

	readonly property var tabbedPane: Item {
		readonly property var availableHeight: 1000
	}

	function createTestObject() {
		return createTemporaryQmlObject("import Governikus.InformationView 1.0; LicenseInformation {}", testCase);
	}
	function test_load() {
		let testObject = createTestObject();
		verify(testObject, "Object loaded");
	}

	name: "test_LicenseInformation"
	visible: true
	when: windowShown
}
