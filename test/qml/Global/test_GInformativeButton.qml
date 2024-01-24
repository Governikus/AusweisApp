/**
 * Copyright (c) 2022-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtTest
import Governikus.Global
import Governikus.Style

TestCase {
	id: testCase

	function createTestObject() {
		return createTemporaryQmlObject("import Governikus.Global; GInformativeButton {}", testCase);
	}
	function test_description() {
		let testObject = createTestObject();
		compare(testObject.description, "", "Initial empty description");
		testObject.description = "test";
		compare(testObject.description, "test", "description: test");
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
	function test_scaleIcon() {
		let testObject = createTestObject();
		compare(testObject.scaleIcon, 1.0, "Initial scale factor 1.0");
		testObject.scaleIcon = 0.75;
		compare(testObject.scaleIcon, 0.75, "scaleIcon: 1.0");
	}
	function test_text() {
		let testObject = createTestObject();
		compare(testObject.text, "", "Initial empty text");
		testObject.text = "test";
		compare(testObject.text, "test", "text: test");
	}
	function test_textStyle() {
		let testObject = createTestObject();
		compare(testObject.textStyle, Style.text.button, "Initial textStyle: button");
		testObject.textStyle = Style.text.normal_warning;
		compare(testObject.textStyle, Style.text.normal_warning, "textStyle: hint_warning");
	}

	name: "test_GInformativeButton"
	visible: true
	when: windowShown

	GInformativeButton {
		id: testObject

		icon.source: "qrc:///images/material_check.svg"
		scaleIcon: 0.5
		text: "test"

		TestCase {
			function test_click() {
				verify(!testObject.pressed, "pressed: false");
				mousePress(testObject);
				tryVerify(function () {
						return testObject.pressed;
					});
				mouseRelease(testObject);
				tryVerify(function () {
						return !testObject.pressed;
					});
			}
			function test_tintIcon() {
				verify(!testObject.tintIcon, "Initial tintIcon: false");
				testObject.tintIcon = true;
				verify(testObject.tintIcon, "tintIcon: true");
				testObject.tintIcon = false;
				verify(!testObject.tintIcon, "tintIcon: false");
			}

			when: windowShown
		}
	}
}
