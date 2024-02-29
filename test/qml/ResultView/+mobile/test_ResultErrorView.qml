/**
 * Copyright (c) 2017-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtTest
import Governikus.Global

TestCase {
	id: testCase

	function createTestObject(pErrorCode = "") {
		return createTemporaryQmlObject("import Governikus.ResultView; ResultErrorView { title: \"ResultErrorView\"; errorCode: \"%1\"}".arg(pErrorCode), testCase);
	}
	function test_hasErrorDetails() {
		let testObject = createTestObject();
		compare(testObject.hasErrorDetails, false);
		let testObjectWithErrorCode = createTestObject("SomeErrorCode");
		compare(testObjectWithErrorCode.hasErrorDetails, true);
	}
	function test_load() {
		let testObject = createTestObject();
		verify(testObject, "Object loaded");
	}

	name: "test_ResultErrorView"
	visible: true
	when: windowShown
}
