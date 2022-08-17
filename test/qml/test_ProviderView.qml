/*
 * \copyright Copyright (c) 2017-2022 Governikus GmbH & Co. KG, Germany
 */

import QtTest 1.15

TestCase {
	name: "ModuleImportTest"
	id: parent

	function test_load_ProviderView() {
		var item = createTemporaryQmlObject("
			import Governikus.ProviderView 1.0;
			ProviderView {}
			", parent);
		item.destroy();
	}
}
