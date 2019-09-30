/*
 * \copyright Copyright (c) 2018-2019 Governikus GmbH & Co. KG, Germany
 */

import QtTest 1.10

TestCase {
	name: "ModuleLoadingInformationView"
	id: parent

	function test_load_VersionInformationView() {
		var item = createTemporaryQmlObject("
			import Governikus.InformationView 1.0;
			VersionInformation {}
			", parent);
		item.destroy();
	}
}
