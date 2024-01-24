/**
 * Copyright (c) 2020-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtTest
import Governikus.Global
import Governikus.Style

TestCase {
	id: testCase

	function createTestObject() {
		return createTemporaryQmlObject("import Governikus.Global; GTextField {}", testCase);
	}
	function test_enabled() {
		let testObject = createTestObject();
		verify(testObject.enabled, "Initial enabled true");
		testObject.enabled = false;
		verify(!testObject.enabled, "enabled false");
	}
	function test_enterKeyType() {
		let testObject = createTestObject();
		compare(testObject.enterKeyType, Qt.EnterKeyDefault, "Initial enterKeyType: Qt.EnterKeyDefault");
		testObject.enterKeyType = Qt.EnterKeyDone;
		compare(testObject.enterKeyType, Qt.EnterKeyDone, "enterKeyType Qt.EnterKeyDone");
	}
	function test_load() {
		let testObject = createTestObject();
		verify(testObject, "Object loaded");
	}
	function test_text() {
		let testObject = createTestObject();
		compare(testObject.text, "", "Initial text empty");
		testObject.text = "test";
		compare(testObject.text, "test", "text: test");
	}
	function test_textStyle() {
		let testObject = createTestObject();
		compare(testObject.textStyle, Style.text.normal, "Initial textStyle: normal");
		testObject.textStyle = Style.text.normal_warning;
		compare(testObject.textStyle, Style.text.normal_warning, "textStyle: hint_warning");
	}
	function test_valid() {
		let testObject = createTestObject();
		verify(testObject.valid, "Initial valid true");
		testObject.valid = false;
		verify(!testObject.valid, "valid false");
	}

	name: "test_GTextField"
	visible: true
	when: windowShown
}
