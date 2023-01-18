/*
 * \copyright Copyright (c) 2020-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import QtTest 1.15
import Governikus.Global 1.0

TestCase {
	id: testCase
	function createTestObject() {
		return createTemporaryQmlObject("import Governikus.Global 1.0; GPane {}", testCase);
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

	name: "test_GPane"
	visible: true
	when: windowShown
}
