/**
 * Copyright (c) 2021-2026 Governikus GmbH & Co. KG, Germany
 */

import QtQuick
import QtTest

TestCase {
	id: testCase

	function createTestObject() {
		return createTemporaryQmlObject("import Governikus.Global; GFileDialog {}", testCase);
	}
	function test_load() {
		let testObject = createTestObject();
		verify(testObject, "Object loaded");
	}
	function test_selectFile() {
		let testObject = createTestObject();
		compare(testObject.selectedFile.toString(), "");
		testObject.selectFile("test");
		let endsWithTest = /test$/;
		verify(endsWithTest.test(testObject.selectedFile));
	}

	name: "test_GFileDialog"
	visible: true
	when: windowShown
}
