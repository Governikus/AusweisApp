/**
 * Copyright (c) 2024-2025 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtTest
import Governikus.RemoteServiceView
import Governikus.Type

TestCase {
	id: testCase

	function createTestObject() {
		return createTemporaryQmlObject("import Governikus.Animations; LinkQualityAnimation {}", testCase);
	}
	function test_load() {
		let testObject = createTestObject();
		verify(testObject, "Object loaded");
	}

	name: "test_LinkQuality"
	visible: true
	when: windowShown
}
