/*
 * \copyright Copyright (c) 2020-2022 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import QtTest 1.15
import Governikus.Global 1.0

TestCase {
	id: testCase
	function createTestObject() {
		return createTemporaryQmlObject("import Governikus.Global 1.0; LabeledSwitch {}", testCase);
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
	function test_title() {
		let testObject = createTestObject();
		compare(testObject.title, "", "Initial title: empty");
		testObject.title = "test";
		compare(testObject.title, "test", "title: test");
	}

	name: "test_LabeledSwitch"
	visible: true
	when: windowShown
}
