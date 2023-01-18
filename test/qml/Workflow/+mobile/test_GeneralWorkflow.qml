/*
 * \copyright Copyright (c) 2018-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import QtTest 1.15
import Governikus.Global 1.0
import Governikus.Type.ReaderPlugIn 1.0

TestCase {
	id: testCase
	function createTestObject() {
		return createTemporaryQmlObject("
			import QtQuick 2.15
			import Governikus.Workflow 1.0
			import Governikus.Type.ReaderPlugIn 1.0
			GeneralWorkflow {
				workflowModel: Item {
					property var readerPlugInType: ReaderPlugIn.NFC
					property bool isSmartCardAllowed: false
					property var supportedPlugInTypes: [ReaderPlugIn.NFC, ReaderPlugIn.REMOTE_IFD, ReaderPlugIn.SMART]
				}
			}
			", testCase);
	}
	function test_load() {
		let testObject = createTestObject();
		verify(testObject, "Object loaded");
	}

	name: "test_GeneralWorkflow"
	visible: true
	when: windowShown
}
