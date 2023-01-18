/*
 * \copyright Copyright (c) 2017-2023 Governikus GmbH & Co. KG, Germany
 */
import QtTest 1.15

TestCase {
	id: parent
	function test_load_EnterPasswordView() {
		var item = createTemporaryQmlObject("
			import Governikus.EnterPasswordView 1.0;
			EnterPasswordView {}
			", parent);
		item.destroy();
	}

	name: "ModuleImportTest"
}
