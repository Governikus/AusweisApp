/*
 * \copyright Copyright (c) 2020-2021 Governikus GmbH & Co. KG, Germany
 */

import QtTest 1.12

import Governikus.Type.NumberModel 1.0

TestCase {
	id: parent

	visible: true
	when: windowShown

	function test_load_EnterPasswordView_data() {
		return [
			{ "passwordType": NumberModel.PASSWORD_PIN },
			{ "passwordType": NumberModel.PASSWORD_CAN },
			{ "passwordType": NumberModel.PASSWORD_PUK },
			{ "passwordType": NumberModel.PASSWORD_REMOTE_PIN },
		]
	}

	function test_load_EnterPasswordView(data) {
		var item = createTemporaryQmlObject("
			import Governikus.EnterPasswordView 1.0;
			EnterPasswordView {
				passwordType: %1
			}
			".arg(data.passwordType), parent);
		item.destroy();
	}
}
