/**
 * Copyright (c) 2023-2025 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtTest
import Governikus.Global

TestCase {
	id: testCase

	function createTestObject() {
		return createTemporaryQmlObject("import Governikus.Global; GMenuItem { title: \"testTitle\"}", testCase);
	}
	function test_load() {
		let testObject = createTestObject();
		verify(testObject, "Object loaded");
	}
	function test_title() {
		let testObject = createTestObject();
		compare(testObject.title, "testTitle", "Initial title must be set");
		testObject.title = "newTitle";
		compare(testObject.title, "newTitle", "title: newTitle");
	}

	name: "test_GMenuItem"
	visible: true
	when: windowShown
}
