/**
 * Copyright (c) 2022-2024 Governikus GmbH & Co. KG, Germany
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
	function test_text() {
		let testObject = createTestObject();
		compare(testObject.text, "", "Initial empty text");
		testObject.text = "test";
		compare(testObject.text, "test", "text: test");
	}

	name: "test_GInformativeButton"
	visible: true
	when: windowShown

	GInformativeButton {
		id: testObject

		icon.source: "qrc:///images/material_check.svg"
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

			when: windowShown
		}
	}
}
