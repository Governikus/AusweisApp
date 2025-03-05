/**
 * Copyright (c) 2024-2025 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtTest
import Governikus.Type

TestCase {
	id: testCase

	readonly property string warnMessage: "Unsupported ReaderManagerPluginType to start scan: "

	function createTestObject(pPluginType) {
		return createTemporaryQmlObject("
			import Governikus.CheckIDCardView;
			import Governikus.Type;

			CheckIDCardWorkflow {
				readerType: %1
			}".arg(pPluginType), testCase);
	}
	function test_load(data) {
		if (data.tag === "unsupported") {
			ignoreWarning(warnMessage + data.typeName);
		} else {
			failOnWarning(warnMessage + data.typeName);
		}
		let testObject = createTestObject(data.type);
		verify(testObject, "Object loaded");
	}
	function test_load_data() {
		return [
			{
				tag: "nfc",
				type: ReaderManagerPluginType.NFC,
				typeName: "NFC"
			},
			{
				tag: "pcsc",
				type: ReaderManagerPluginType.PCSC,
				typeName: "PCSC"
			},
			{
				tag: "remote_ifd",
				type: ReaderManagerPluginType.REMOTE_IFD,
				typeName: "REMOTE_IFD"
			},
			{
				tag: "unsupported",
				type: ReaderManagerPluginType.LOCAL_IFD,
				typeName: "LOCAL_IFD"
			}
		];
	}

	name: "test_CheckIDCardWorkflow"
	visible: true
	when: windowShown
}
