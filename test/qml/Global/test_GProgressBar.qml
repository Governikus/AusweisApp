/**
 * Copyright (c) 2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtTest
import Governikus.Global
import Governikus.Type.SettingsModel

TestCase {
	id: testCase

	function createTestObject() {
		return createTemporaryQmlObject("import Governikus.Global; GProgressBar {}", testCase);
	}
	function test_load() {
		let testObject = createTestObject("");
		verify(testObject, "Object loaded");
	}
	function test_text() {
		let testObject = createTestObject();
		compare(testObject.text, "", "Initial empty text");
		testObject.text = "test";
		compare(testObject.text, "test", "Text: test");
	}
	function test_value(data) {
		SettingsModel.useAnimations = data.useAnimations;
		let testObject = createTestObject();
		testObject.value = data.value;
		tryCompare(testObject, "effectiveVisualPosition", data.effectiveVisualPosition);
	}
	function test_value_data() {
		return [{
				"tag": "With Animation, 0",
				"useAnimations": true,
				"value": 0,
				"effectiveVisualPosition": 0
			}, {
				"tag": "With Animation, 10",
				"useAnimations": true,
				"value": 10,
				"effectiveVisualPosition": 0.1
			}, {
				"tag": "With Animation, 50",
				"useAnimations": true,
				"value": 50,
				"effectiveVisualPosition": 0.5
			}, {
				"tag": "With Animation, 100",
				"useAnimations": true,
				"value": 100,
				"effectiveVisualPosition": 1
			}, {
				"tag": "Without Animation, 0",
				"useAnimations": false,
				"value": 0,
				"effectiveVisualPosition": 0.25
			}, {
				"tag": "Without Animation, 10",
				"useAnimations": false,
				"value": 10,
				"effectiveVisualPosition": 0.25
			}, {
				"tag": "Without Animation, 50",
				"useAnimations": false,
				"value": 50,
				"effectiveVisualPosition": 0.25
			}, {
				"tag": "Without Animation, 100",
				"useAnimations": false,
				"value": 100,
				"effectiveVisualPosition": 1
			}];
	}

	name: "test_GProgressBar"
	visible: true
	when: windowShown
}
