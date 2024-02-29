/**
 * Copyright (c) 2018-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtTest
import Governikus.Global
import Governikus.Type.ReaderPlugIn

TestCase {
	id: testCase

	function createTestObject() {
		return createTemporaryQmlObject("
			import QtQuick
			import QtQuick.Controls
			import Governikus.Workflow
			import Governikus.Type.ReaderPlugIn

			ApplicationWindow {
				menuBar: Item {}
				GeneralWorkflow {
					workflowModel: Item {
						property var readerPlugInType: ReaderPlugIn.NFC
						property bool isCurrentSmartCardAllowed: false
						property var supportedPlugInTypes: [ReaderPlugIn.NFC, ReaderPlugIn.REMOTE_IFD, ReaderPlugIn.SMART]
					}
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
