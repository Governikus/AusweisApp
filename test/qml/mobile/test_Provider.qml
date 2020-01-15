/*
 * \copyright Copyright (c) 2018-2020 Governikus GmbH & Co. KG, Germany
 */

import QtTest 1.10

TestCase {
	name: "ModuleLoadingProvider"
	id: parent

	function test_load_ProviderContactTab() {
		var item = createTemporaryQmlObject("
			import Governikus.Provider 1.0;
			ProviderContactTab {}
			", parent);
		item.destroy();
	}

	function test_load_ProviderHeader() {
		var item = createTemporaryQmlObject("
			import Governikus.Provider 1.0;
			ProviderHeader {}
			", parent);
		item.destroy();
	}

	function test_load_ProviderModelItem() {
		var item = createTemporaryQmlObject("
			import Governikus.Provider 1.0;
			ProviderModelItem {}
			", parent);
		item.destroy();
	}
}
