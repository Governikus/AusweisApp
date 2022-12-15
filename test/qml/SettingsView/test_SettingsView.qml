/*
 * \copyright Copyright (c) 2017-2022 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import QtTest 1.15

TestCase {
	id: testCase
	function createTestObject() {
		return createTemporaryQmlObject("
			import QtQuick 2.15
			import QtQuick.Controls 2.15
			import Governikus.SettingsView 1.0

			ApplicationWindow {
				menuBar: Item {
					function updateActions() {}
				}

				SettingsView {}
			}
		", testCase);
	}
	function test_load() {
		let testObject = createTestObject();
		verify(testObject, "Object loaded");
	}

	name: "test_SettingsView"
	visible: true
	when: windowShown
}
