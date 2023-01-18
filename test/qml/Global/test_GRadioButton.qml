/*
 * \copyright Copyright (c) 2020-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import QtTest 1.15
import Governikus.Global 1.0
import Governikus.Style 1.0

TestCase {
	id: testCase
	function createTestObject() {
		return createTemporaryQmlObject("import Governikus.Global 1.0; GRadioButton {}", testCase);
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
	function test_textStyle() {
		let testObject = createTestObject();
		compare(testObject.textStyle, Style.text.normal, "Initial textStyle: normal");
		testObject.textStyle = Style.text.hint_warning;
		compare(testObject.textStyle, Style.text.hint_warning, "textStyle: hint_warning");
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
