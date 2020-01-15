/*
 * \copyright Copyright (c) 2017-2020 Governikus GmbH & Co. KG, Germany
 */

import QtTest 1.10

TestCase {
	name: "ModuleImportTest"
	id: parent

	function test_load_IdentifyView() {
		var item = createTemporaryQmlObject("
			import Governikus.IdentifyView 1.0;
			IdentifyView {}
			", parent);
		item.destroy();
	}
}
