/*
 * \copyright Copyright (c) 2018-2020 Governikus GmbH & Co. KG, Germany
 */

import QtTest 1.12

TestCase {
	name: "ModuleImportTest"
	id: parent

	function test_load_MainView() {
		var item = createTemporaryQmlObject("
			import Governikus.MainView 1.0;
			MainView {}
			", parent);
		item.destroy();
	}
}
