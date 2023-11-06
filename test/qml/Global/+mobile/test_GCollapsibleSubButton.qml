/**
 * Copyright (c) 2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtTest
import Governikus.Global

TestCase {
	id: testCase

	function createTestObject() {
		return createTemporaryQmlObject("import Governikus.Global; GCollapsibleSubButton {}", testCase);
	}
	function test_image() {
		let testObject = createTestObject();
		compare(testObject.image, "", "Initial no image");
		testObject.image = "qrc:///images/material_check.svg";
		compare(testObject.image, "qrc:///images/material_check.svg", "Image: qrc:///images/material_check.svg");
	}
	function test_load() {
		let testObject = createTestObject();
		verify(testObject, "Object loaded");
	}
	function test_text() {
		let testObject = createTestObject();
		compare(testObject.text, "", "Initial title: empty");
		testObject.text = "test";
		compare(testObject.text, "test", "text: test");
	}

	name: "test_GCollapsibleSubButton"
	visible: true
	when: windowShown
}
