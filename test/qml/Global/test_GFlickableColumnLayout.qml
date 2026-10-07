/**
 * Copyright (c) 2021-2026 Governikus Service GmbH, Germany
 */

import QtQuick
import QtTest
import Governikus.Global

TestCase {
	id: testCase

	function createTestObject() {
		return createTemporaryQmlObject("import Governikus.Global; GFlickableColumnLayout {}", testCase);
	}
	function test_load() {
		let testObject = createTestObject();
		verify(testObject, "Object loaded");
	}

	name: "test_GFlickableColumnLayout"
	visible: true
	when: windowShown
}
