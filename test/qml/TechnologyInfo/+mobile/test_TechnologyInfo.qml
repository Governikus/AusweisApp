/**
 * Copyright (c) 2017-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtTest

TestCase {
	id: testCase

	function createTestObject() {
		return createTemporaryQmlObject("import Governikus.TechnologyInfo; TechnologyInfo {}", testCase);
	}
	function test_load() {
		let testObject = createTestObject();
		verify(testObject, "Object loaded");
	}

	name: "test_TechnologyInfo"
	visible: true
	when: windowShown
}
