/**
 * Copyright (c) 2017-2025 Governikus GmbH & Co. KG, Germany
 */
import QtTest

TestCase {
	id: testCase

	function createTestObject() {
		return createTemporaryQmlObject("
			import QtQuick
			import QtQuick.Controls
			import Governikus.OnboardingView

			ApplicationWindow {
				menuBar: Item {
					function updateActions() {}
				}

				TrayIconView { title: \"testCase\" }
			}
		", testCase);
	}
	function test_load() {
		let testObject = createTestObject();
		verify(testObject, "Object loaded");
	}

	name: "test_TrayIconView"
	visible: true
	when: windowShown
}
