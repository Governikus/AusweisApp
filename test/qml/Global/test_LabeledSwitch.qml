/**
 * Copyright (c) 2020-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtTest
import Governikus.Global

TestCase {
	id: testCase

	function createTestObject() {
		return createTemporaryQmlObject("import Governikus.Global; LabeledSwitch {}", testCase);
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
		if (!Constants.is_desktop) {
			skip("Skip test because of flaky behavior for mobile on Linux");
		}
		let testObject = createTestObject();
		compare(testObject.title, "", "Initial title: empty");
		testObject.title = "test";
		compare(testObject.title, "test", "title: test");
	}

	name: "test_LabeledSwitch"
	visible: true
	when: windowShown
}
