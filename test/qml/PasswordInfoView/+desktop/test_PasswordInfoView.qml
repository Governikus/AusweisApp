/**
 * Copyright (c) 2020-2023 Governikus GmbH & Co. KG, Germany
 */
import QtTest 1.15
import Governikus.PasswordInfoView 1.0

TestCase {
	id: parent
	function test_load_PasswordInfoView(data) {
		var item = createTemporaryQmlObject("
			import Governikus.PasswordInfoView 1.0;
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
			}, {
				"contentType": PasswordInfoContent.Type.SMARTPHONE_AS_CARD_READER
			}];
	}

	visible: true
	when: windowShown
}
