/*
 * \copyright Copyright (c) 2018-2020 Governikus GmbH & Co. KG, Germany
 */

import QtTest 1.10

TestCase {
	name: "ModuleImportTest"
	id: parent

	function test_load_TitleBar() {
		var item = createTemporaryQmlObject("
			import Governikus.TitleBar 1.0;
			TitleBar {}
			", parent);
		item.destroy();
	}
}
