/*
 * \copyright Copyright (c) 2020-2022 Governikus GmbH & Co. KG, Germany
 */

import QtTest 1.15

TestCase {
	name: "ModuleImportTest"
	id: parent

	function test_load_MainView() {
		var item = createTemporaryQmlObject("
			import Governikus.Global 1.0;
			DynamicLayout {}
			", parent);
		item.destroy();
	}
}
