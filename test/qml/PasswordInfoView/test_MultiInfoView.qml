/**
 * Copyright (c) 2020-2025 Governikus GmbH & Co. KG, Germany
 */
import QtTest
import Governikus.MultiInfoView

TestCase {
	id: parent

	function test_load_MultiInfoView(data) {
		let item = createTemporaryQmlObject("
			import Governikus.MultiInfoView
			MultiInfoView {
				infoContent: MultiInfoData {
					contentType: %1
				}
			}
			".arg(data.contentType), parent);
		item.destroy();
	}
	function test_load_MultiInfoView_data() {
		return [
			{
				"contentType": MultiInfoData.Type.PIN
			},
			{
				"contentType": MultiInfoData.Type.CAN
			},
			{
				"contentType": MultiInfoData.Type.PUK
			}
		];
	}

	visible: true
	when: windowShown
}
