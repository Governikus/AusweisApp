/**
 * Copyright (c) 2025 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtTest

TestCase {
	id: testCase

	function createTestObject() {
		return createTemporaryQmlObject("
			import QtQuick
			import Governikus.Type

			Item {
				readonly property int testStatus: GlobalStatusCode.No_Error
				readonly property int modelStatus: AuthModel.statusCode
				readonly property string testStatusString: \"Error code: Unknown_Error\"
				readonly property string modelStatusString: AuthModel.statusCodeDisplayString
			}
		", testCase);
	}
	function test_load() {
		let testObject = createTestObject();
		verify(testObject, "Object loaded");
		verify(testObject.testStatus, testObject.modelStatus);
		verify(testObject.testStatusString, testObject.statusCodeDisplayString);
	}

	name: "test_AuthModelQml"
	visible: true
	when: windowShown
}
