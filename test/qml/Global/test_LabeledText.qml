/**
 * Copyright (c) 2020-2025 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtTest
import Governikus.Global

TestCase {
	id: testCase

	function createTestObject() {
		return createTemporaryQmlObject("import Governikus.Global; LabeledText { text: \"testText\"; label: \"testLabel\"}", testCase);
	}
	function test_load() {
		let testObject = createTestObject();
		verify(testObject, "Object loaded");
	}
	function test_text() {
		let testObject = createTestObject();
		compare(testObject.text, "testText", "Initial text set");
		testObject.text = "newText";
		compare(testObject.text, "newText", "text: newText");
		compare(testObject.label, "testLabel", "Initial label set");
		testObject.label = "newLabel";
		compare(testObject.label, "newLabel", "label: newLabel");
	}

	name: "test_LabeledText"
	visible: true
	when: windowShown
}
