/**
 * Copyright (c) 2020-2025 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtTest
import Governikus.Global

TestCase {
	id: testCase

	function createTestObject() {
		return createTemporaryQmlObject("
			import Governikus.CheckIDCardView;
			import Governikus.Type;

			CheckIDCardView {
			}", testCase);
	}
	function test_load() {
		let testObject = createTestObject();
		verify(testObject, "Object loaded");
	}

	name: "test_CheckIDCardView"
	visible: true
	when: windowShown
}
