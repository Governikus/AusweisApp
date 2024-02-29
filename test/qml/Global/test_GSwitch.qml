/**
 * Copyright (c) 2020-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtTest
import Governikus.Global

TestCase {
	id: testCase

	function createTestObject() {
		return createTemporaryQmlObject("import Governikus.Global; GSwitch {}", testCase);
	}
	function test_checked() {
		let testObject = createTestObject();
		verify(!testObject.checked, "Initial checked: false");
		testObject.checked = true;
		verify(testObject.checked, "checked: true");
		testObject.checked = false;
		verify(!testObject.checked, "checked: false");
	}
	function test_description() {
		let testObject = createTestObject();
		compare(testObject.description, "", "Initial description: empty");
		testObject.description = "test";
		compare(testObject.description, "test", "description: test");
	}
	function test_load() {
		let testObject = createTestObject();
		verify(testObject, "Object loaded");
	}
	function test_text() {
		let testObject = createTestObject();
		compare(testObject.text, "", "Initial text: empty");
		testObject.text = "test";
		compare(testObject.text, "test", "text: test");
	}

	name: "test_GSwitch"
	visible: true
	when: windowShown
}
