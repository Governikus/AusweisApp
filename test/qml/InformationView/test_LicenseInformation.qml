/*
 * \copyright Copyright (c) 2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12
import QtTest 1.12

import Governikus.Global 1.0

TestCase {
	id: testCase

	name: "test_LicenseInformation"
	visible: true
	when: windowShown

	readonly property var tabbedPane: Item {
		readonly property var availableHeight: 1000
	}

	function createTestObject() {
		return createTemporaryQmlObject("import Governikus.InformationView 1.0; LicenseInformation {}", testCase)
	}

	function test_load() {
		let testObject = createTestObject()
		verify(testObject, "Object loaded")
	}

}
