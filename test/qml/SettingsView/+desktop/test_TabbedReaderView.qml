/**
 * Copyright (c) 2021-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtTest

TestCase {
	id: testCase

	function createTestObject() {
		return createTemporaryQmlObject("
			import QtQuick
			import QtQuick.Controls
			import Governikus.SettingsView

			ApplicationWindow {
				menuBar: Item {
					function updateActions() {}
				}

				TabbedReaderView {}
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
