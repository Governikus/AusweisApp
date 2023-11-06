/**
 * Copyright (c) 2020-2023 Governikus GmbH & Co. KG, Germany
 */
import QtTest
import Governikus.Type.PasswordType

TestCase {
	id: parent

	function test_load_EnterPasswordView(data) {
		let item = createTemporaryQmlObject("
			import Governikus.EnterPasswordView
			EnterPasswordView {
				passwordType: %1
			}
			".arg(data.passwordType), parent);
		item.destroy();
	}
	function test_load_EnterPasswordView_data() {
		return [{
				"passwordType": PasswordType.PIN
			}, {
				"passwordType": PasswordType.CAN
			}, {
				"passwordType": PasswordType.PUK
			}, {
				"passwordType": PasswordType.REMOTE_PIN
			}];
	}

	visible: true
	when: windowShown
}
