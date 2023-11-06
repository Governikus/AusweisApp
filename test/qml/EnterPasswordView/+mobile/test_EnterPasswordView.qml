/**
 * Copyright (c) 2017-2023 Governikus GmbH & Co. KG, Germany
 */
import QtTest

TestCase {
	id: parent

	function test_load_EnterPasswordView() {
		let item = createTemporaryQmlObject("
			import Governikus.EnterPasswordView
			import Governikus.Type.PasswordType
			EnterPasswordView {
				passwordType: PasswordType.PIN
				title: \"EnterPasswordView\"
			}
			", parent);
		item.destroy();
	}

	name: "ModuleImportTest"
}
