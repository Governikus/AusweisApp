/**
 * Copyright (c) 2020-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import QtTest 1.15
import Governikus.Global 1.0
import Governikus.Style 1.0

TestCase {
	id: testCase
	function createTestObject() {
		return createTemporaryQmlObject("import Governikus.Global 1.0; GText {}", testCase);
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
	function test_textStyle() {
		let testObject = createTestObject();
		compare(testObject.textStyle, Style.text.normal, "Initial textStyle: normal");
		testObject.textStyle = Style.text.hint_warning;
		compare(testObject.textStyle, Style.text.hint_warning, "textStyle: hint_warning");
	}

	name: "test_GText"
	visible: true
	when: windowShown

	GText {
		id: testObject
		onLinkActivated: testObject.text = "link activated"

		TestCase {
			function test_spacePress() {
				testObject.text = "Test <a href=\"#\">Test<\a> Test";
				testObject.forceActiveFocus();
				keyClick(Qt.Key_Space);
				compare(testObject.text, "link activated", "Link activated");
			}

			when: windowShown
		}
	}
}
