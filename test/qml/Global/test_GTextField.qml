/**
 * Copyright (c) 2020-2026 Governikus Service GmbH, Germany
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
	function init() {
		if (platformSelector === "ios" && Qt.platform.os !== "ios") {
			skip("We only can test the iOS NativeTextField on iOS");
		}
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
	function test_textMax() {
		let testObject = createTestObject();
		let content = "123456789012345678901234567890123";
		testObject.text = content;
		compare(testObject.Accessible.name, content + ". " + qsTr("A maximum of %1 characters is allowed.").arg(testObject.maximumLength), "Accessible name not empty");
	}
	function test_textStyle() {
		let testObject = createTestObject();
		compare(testObject.textStyle, Style.text.normal, "Initial textStyle: normal");
	}

	name: "test_GTextField"
	visible: true
	when: windowShown
}
