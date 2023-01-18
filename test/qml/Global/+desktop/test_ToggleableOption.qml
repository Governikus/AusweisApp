/*
 * \copyright Copyright (c) 2020-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import QtTest 1.15
import Governikus.Global 1.0

TestCase {
	id: testCase
	function createTestObject() {
		return createTemporaryQmlObject("import Governikus.Global 1.0; ToggleableOption {}", testCase);
	}
	function test_checked() {
		let testObject = createTestObject();
		verify(!testObject.checked, "Initial checked: false");
		testObject.checked = true;
		verify(testObject.checked, "checked: true");
		testObject.checked = false;
		verify(!testObject.checked, "checked: false");
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

	name: "test_ToggleableOption"
	visible: true
	when: windowShown
}
