/*
 * \copyright Copyright (c) 2019 Governikus GmbH & Co. KG, Germany
 */

import QtTest 1.10

TestCase {
	name: "ModuleImportTest"
	id: parent

	function test_load_SelfAuthenticationView() {
		var item = createTemporaryQmlObject("
			import Governikus.SelfAuthenticationView 1.0;
			SelfAuthenticationView {}
			", parent);
		item.destroy();
	}
}
