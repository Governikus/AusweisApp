/**
 * Copyright (c) 2020-2024 Governikus GmbH & Co. KG, Germany
 */
import QtTest
import Governikus.PasswordInfoView

TestCase {
	id: parent

	function test_load_PasswordInfoView(data) {
		let item = createTemporaryQmlObject("
			import Governikus.PasswordInfoView
			PasswordInfoView {
				infoContent: PasswordInfoData {
					contentType: %1
				}
			}
			".arg(data.contentType), parent);
		item.destroy();
	}
	function test_load_PasswordInfoView_data() {
		return [
			{
				"contentType": PasswordInfoData.Type.PIN
			},
			{
				"contentType": PasswordInfoData.Type.CAN
			},
			{
				"contentType": PasswordInfoData.Type.PUK
			}
		];
	}

	visible: true
	when: windowShown
}
