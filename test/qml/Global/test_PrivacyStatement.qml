/**
 * Copyright (c) 2021-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtTest
import Governikus.Global

TestCase {
	id: testCase

	function createTestObject() {
		return createTemporaryQmlObject("import Governikus.Global; PrivacyStatement {}", testCase);
	}
	function test_load() {
		let testObject = createTestObject();
		verify(testObject, "Object loaded");
	}

	name: "test_PrivacyStatement"
	visible: true
	when: windowShown
}
