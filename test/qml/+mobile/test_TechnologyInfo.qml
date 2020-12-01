/*
 * \copyright Copyright (c) 2017-2020 Governikus GmbH & Co. KG, Germany
 */

import QtTest 1.12

TestCase {
	name: "ModuleImportTest"
	id: parent

	function test_load_TechnologyInfo() {
		var item = createTemporaryQmlObject("
			import Governikus.TechnologyInfo 1.0;
			TechnologyInfo {}
			", parent);
		item.destroy();
	}
}
