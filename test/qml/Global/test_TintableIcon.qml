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
		return createTemporaryQmlObject("import Governikus.Global; TintableIcon {}", testCase);
	}
	function test_icon() {
		let testObject = createTestObject();
		compare(testObject.source, "", "Initial no image");
		testObject.source = "qrc:///images/material_check.svg";
		compare(testObject.source, "qrc:///images/material_check.svg", "Image: qrc:///images/material_check.svg");
	}
	function test_load() {
		let testObject = createTestObject();
		verify(testObject, "Object loaded");
	}
	function test_tintColor() {
		let testObject = createTestObject();
		compare(testObject.tintColor, Style.color.textNormal.basic, "Initial tintColor: text");
		testObject.tintColor = "#000000";
		compare(testObject.tintColor, "#000000", "Set tintColor");
	}
	function test_tintEnabled() {
		let testObject = createTestObject();
		verify(testObject.tintEnabled, "Initial tintEnabled: true");
		testObject.tintEnabled = false;
		verify(!testObject.tintEnabled, "Set tintEnabled: false");
	}

	name: "test_TintableIcon"
	visible: true
	when: windowShown
}
