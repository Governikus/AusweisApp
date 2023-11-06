/**
 * Copyright (c) 2020-2023 Governikus GmbH & Co. KG, Germany
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
		return [{
				"contentType": PasswordInfoContent.Type.PIN
			}, {
				"contentType": PasswordInfoContent.Type.CAN
			}, {
				"contentType": PasswordInfoContent.Type.PUK
			}];
	}

	visible: true
	when: windowShown
}
