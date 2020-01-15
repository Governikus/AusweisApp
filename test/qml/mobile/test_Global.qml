/*
 * \copyright Copyright (c) 2017-2020 Governikus GmbH & Co. KG, Germany
 */

import QtTest 1.10

TestCase {
	name: "ModuleImportTest"
	id: parent

	function test_load_GSwitch() {
		var item = createTemporaryQmlObject("
			import Governikus.Global 1.0;
			GSwitch {}
			", parent);
		item.destroy();
	}

	function test_load_ListItem() {
		var item = createTemporaryQmlObject("
			import Governikus.Global 1.0;
			ListItem {}
			", parent);
		item.destroy();
	}
}
