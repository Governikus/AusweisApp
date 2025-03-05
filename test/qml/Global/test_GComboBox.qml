/**
 * Copyright (c) 2020-2025 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtTest
import Governikus.Global
import Governikus.Style

TestCase {
	id: testCase

	function createTestObject() {
		return createTemporaryQmlObject("import Governikus.Global; GComboBox {}", testCase);
	}
	function test_initial() {
		let testObject = createTestObject();
		compare(testObject.count, 0, "count: -1");
		compare(testObject.currentIndex, -1, "currentIndex: -1");
		verify(testObject.indicator.visible, "Indicator visible has to be: true");
	}
	function test_load() {
		let testObject = createTestObject();
		verify(testObject, "Object loaded");
	}
	function test_textStyle() {
		let testObject = createTestObject();
		compare(testObject.textStyle, Style.text.normal, "Initial textStyle: normal");
	}

	name: "test_GComboBox"
	visible: true
	when: windowShown

	GComboBox {
		id: testObject

		model: ["a", "b", "c", "d"]

		TestCase {
			function test_click() {
				verify(!testObject.popup.visible, "Initial popup visible: false");
				mouseClick(testObject);
				tryVerify(function () {
					return testObject.popup;
				});
				compare(testObject.popup.visible, true);
				mouseClick(testObject);
				tryVerify(function () {
					return testObject.popup;
				});
				compare(testObject.popup.visible, false);
			}
			function test_model() {
				compare(testObject.count, 4, "count: 4");
				verify(testObject.indicator.visible, "Indicator visible: true");
				compare(testObject.currentIndex, 0, "currentIndex: 0");
				compare(testObject.currentText, "a", "currentText: a");
				testObject.currentIndex = 1;
				compare(testObject.currentIndex, 1, "currentIndex: 1");
				compare(testObject.currentText, "b", "currentText: b");
			}

			when: windowShown
		}
	}
}
