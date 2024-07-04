/**
 * Copyright (c) 2020-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtTest

TestCase {
	id: testCase

	function createTestObject() {
		return createTemporaryQmlObject("
			import Governikus.FeedbackView
			ListItem {
				width: 200
				height: 50
			}
		", testCase);
	}
	function test_icon() {
		let testObject = createTestObject();
		compare(testObject.icon, "", "Initial icon: empty");
		testObject.icon = "qrc:///images/material_check.svg";
		compare(testObject.icon, "qrc:///images/material_check.svg", "icon: qrc:///images/material_check.svg");
		verify(!testObject.tintIcon, "Initial tintIcon: false");
		testObject.tintIcon = true;
		verify(testObject.tintIcon, "tintIcon: true");
	}
	function test_load() {
		let testObject = createTestObject();
		verify(testObject, "Object loaded");
	}
	function test_showSeparator() {
		let testObject = createTestObject();
		verify(testObject.showSeparator, "Initial showSeparator: true");
		testObject.showSeparator = false;
		verify(!testObject.showSeparator, "showSeparator: false");
	}
	function test_text() {
		let testObject = createTestObject();
		compare(testObject.text, "", "Initial text: empty");
		testObject.text = "test";
		compare(testObject.text, "test", "text: test");
		compare(testObject.headerText, "", "Initial headerText: empty");
		testObject.headerText = "test";
		compare(testObject.headerText, "test", "headerText: test");
		compare(testObject.footerText, "", "Initial footerText: empty");
		testObject.footerText = "test";
		compare(testObject.footerText, "test", "footerText: test");
	}

	name: "test_ListItem"
	visible: true
	when: windowShown
}
