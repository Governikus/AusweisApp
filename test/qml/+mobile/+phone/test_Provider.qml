/*
 * \copyright Copyright (c) 2018-2021 Governikus GmbH & Co. KG, Germany
 */

import QtTest 1.12

TestCase {
	name: "ModuleLoadingProvider"
	id: parent

	function test_load_ProviderHeader() {
		var item = createTemporaryQmlObject("
			import Governikus.Provider 1.0;
			ProviderHeader {}
			", parent);
		item.destroy();
	}

}
