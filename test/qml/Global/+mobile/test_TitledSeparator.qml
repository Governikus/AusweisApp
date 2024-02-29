/**
 * Copyright (c) 2020-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtTest
import Governikus.Global

TestCase {
	id: testCase

	function createTestObject() {
		return createTemporaryQmlObject("import Governikus.Global; TitledSeparator {}", testCase);
	}
	function test_load() {
		let testObject = createTestObject();
		verify(testObject, "Object loaded");
	}
	function test_title() {
		let testObject = createTestObject();
		compare(testObject.title, "", "Initial title: empty");
		testObject.title = "test";
		compare(testObject.title, "test", "title: test");
	}

	name: "test_TitledSeparator"
	visible: true
	when: windowShown
}
