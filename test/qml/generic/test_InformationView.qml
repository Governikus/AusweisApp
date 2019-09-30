/*
 * \copyright Copyright (c) 2019 Governikus GmbH & Co. KG, Germany
 */

import QtTest 1.10

TestCase {
	name: "ModuleImportTest"
	id: parent

	function test_load_InformationView() {
		var item = createTemporaryQmlObject("
			import Governikus.InformationView 1.0;
			InformationView {}
			", parent);
		item.destroy();
	}
}
