/**
 * Copyright (c) 2020-2026 Governikus Service GmbH, Germany
 */

import QtQuick
import QtTest
import Governikus.Global

TestCase {
	id: testCase

	function createTestObject() {
		return createTemporaryQmlObject("import Governikus.Global; GListView {}", testCase);
	}
	function test_load() {
		let testObject = createTestObject();
		verify(testObject, "Object loaded");
	}

	name: "test_GListView"
	visible: true
	when: windowShown
}
