/**
 * Copyright (c) 2020-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtTest
import Governikus.Global

TestCase {
	id: testCase

	function createTestObject() {
		return createTemporaryQmlObject("import Governikus.Global; GScrollBar {}", testCase);
	}
	function test_click() {
		let testObject = createTestObject();
		mousePress(testObject);
		tryVerify(function () {
				return testObject.pressed;
			});
		mouseRelease(testObject);
		tryVerify(function () {
				return !testObject.pressed;
			});
	}
	function test_highlight() {
		let testObject = createTestObject();
		tryVerify(function () {
				return !testObject.highlighted;
			});
		testObject.highlight();
		verify(testObject.highlighted, "highlighted: true");
		tryVerify(function () {
				return !testObject.highlighted;
			});
	}
	function test_load() {
		let testObject = createTestObject();
		verify(testObject, "Object loaded");
	}

	name: "test_GScrollBar"
	visible: true
	when: windowShown
}
