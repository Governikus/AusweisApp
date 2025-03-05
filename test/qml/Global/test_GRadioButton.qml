/**
 * Copyright (c) 2020-2025 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtTest
import Governikus.Global
import Governikus.Style

TestCase {
	id: testCase

	function createTestObject() {
		return createTemporaryQmlObject("import Governikus.Global; GRadioButton {}", testCase);
	}
	function test_icon() {
		let testObject = createTestObject();
		compare(testObject.icon.source, "", "Initial no image");
		testObject.icon.source = "qrc:///images/material_check.svg";
		compare(testObject.icon.source, "qrc:///images/material_check.svg", "Image: qrc:///images/material_check.svg");
	}
	function test_load() {
		let testObject = createTestObject();
		verify(testObject, "Object loaded");
	}
	function test_text() {
		let testObject = createTestObject();
		compare(testObject.text, "", "Initial empty text");
		testObject.text = "test";
		compare(testObject.text, "test", "text: test");
	}
	function test_tintIcon() {
		let testObject = createTestObject();
		verify(!testObject.tintIcon, "Initial tintIcon: false");
		testObject.tintIcon = true;
		verify(testObject.tintIcon, "tintIcon: true");
	}

	name: "test_GRadioButton"
	visible: true
	when: windowShown

	Item {
		GRadioButton {
			id: button_a

			checked: true
		}
		GRadioButton {
			id: button_b

		}
		TestCase {
			function test_selection() {
				verify(button_a.checked, "button_a checked: true");
				verify(!button_b.checked, "button_b checked: false");
				mouseClick(button_b);
				verify(!button_a.checked, "button_a checked: false");
				verify(button_b.checked, "button_b checked: true");
			}

			when: windowShown
		}
	}
}
