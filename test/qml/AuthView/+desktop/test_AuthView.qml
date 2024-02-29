/**
 * Copyright (c) 2017-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtTest
import Governikus.Global

TestCase {
	id: testCase

	function createTestObject() {
		return createTemporaryQmlObject("
			import QtQuick
			import QtQuick.Controls
			import Governikus.AuthView

			ApplicationWindow {
				menuBar: Item {
					function updateActions() {}
				}

				AuthView {}
			}
		", testCase);
	}
	function test_load() {
		if (hasBindingLoop && Constants.is_desktop) {
			skip("Skip test because of QTBUG-110899");
		}
		let testObject = createTestObject();
		verify(testObject, "Object loaded");
	}

	name: "test_AuthView"
	visible: true
	when: windowShown
}
