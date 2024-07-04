/**
 * Copyright (c) 2018-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtTest
import Governikus.Global
import Governikus.Type

TestCase {
	id: testCase

	function createTestObject() {
		return createTemporaryQmlObject("
			import QtQuick
			import QtQuick.Controls
			import Governikus.Workflow
			import Governikus.Type

			ApplicationWindow {
				menuBar: Item {}
				GeneralWorkflow {
					workflowModel: Item {
						property var readerPluginType: ReaderManagerPluginType.NFC
						property bool isCurrentSmartCardAllowed: false
						property var supportedPluginTypes: [ReaderManagerPluginType.NFC, ReaderManagerPluginType.REMOTE_IFD, ReaderManagerPluginType.SMART]
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
