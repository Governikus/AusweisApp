/**
 * Copyright (c) 2020-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtTest
import Governikus.Global

TestCase {
	id: testCase

	function createTestObject() {
		return createTemporaryQmlObject("import Governikus.Global; NavigationButton {}", testCase);
	}
	function test_load() {
		let testObject = createTestObject();
		verify(testObject, "Object loaded");
	}
	function test_subtext() {
		let testObject = createTestObject();
		compare(testObject.text, "Continue", "Initial subText: Continue");
		testObject.subText = "test";
		compare(testObject.text, "test", "subText: test");
	}
	function test_type() {
		let testObject = createTestObject();
		compare(testObject.buttonType, NavigationButton.Type.Forward, "Initial type: Forward");
		testObject.buttonType = NavigationButton.Type.Back;
		compare(testObject.buttonType, NavigationButton.Type.Back, "type: Back");
		testObject.buttonType = NavigationButton.Type.Check;
		compare(testObject.buttonType, NavigationButton.Type.Check, "type: Check");
		testObject.buttonType = NavigationButton.Type.Cancel;
		compare(testObject.buttonType, NavigationButton.Type.Cancel, "type: Cancel");
	}

	name: "test_NavigationButton"
	visible: true
	when: windowShown
}
