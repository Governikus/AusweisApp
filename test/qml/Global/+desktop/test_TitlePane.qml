/**
 * Copyright (c) 2023 Governikus GmbH & Co. KG, Germany
 */
import QtTest

TestCase {
	id: testCase

	function createTestObject() {
		return createTemporaryQmlObject("
			import \"qrc:/qml/Governikus/TitleBar/+desktop/\"
			TitlePane {}
		", testCase);
	}
	function test_load() {
		let testObject = createTestObject();
		verify(testObject, "Object loaded");
	}
}
