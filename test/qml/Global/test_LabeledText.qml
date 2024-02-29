/**
 * Copyright (c) 2020-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtTest
import Governikus.Global

TestCase {
	id: testCase

	function createTestObject() {
		return createTemporaryQmlObject("import Governikus.Global; LabeledText {}", testCase);
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
		compare(testObject.label, "", "Initial label: empty");
		testObject.label = "test";
		compare(testObject.label, "test", "label: test");
	}

	name: "test_LabeledText"
	visible: true
	when: windowShown
}
