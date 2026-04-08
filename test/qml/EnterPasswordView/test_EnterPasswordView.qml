/**
 * Copyright (c) 2017-2026 Governikus GmbH & Co. KG, Germany
 */

import QtTest
import Governikus.Global
import Governikus.Type

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

	name: "test_EnterPasswordView"
}
