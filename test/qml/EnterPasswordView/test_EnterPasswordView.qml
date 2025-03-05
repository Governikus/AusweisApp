/**
 * Copyright (c) 2017-2025 Governikus GmbH & Co. KG, Germany
 */
import QtTest
import Governikus.Global

TestCase {
	id: parent

	function test_load_EnterPasswordView() {
		let item = createTemporaryQmlObject("
			import Governikus.EnterPasswordView
			import Governikus.Type
			EnterPasswordView {
				passwordType: NumberModel.PasswordType.PIN
				title: \"EnterPasswordView\"
			}
			", parent);
		item.destroy();
	}

	name: "ModuleImportTest"
}
