/*
 * \copyright Copyright (c) 2018-2020 Governikus GmbH & Co. KG, Germany
 */

import QtTest 1.10

TestCase {
	name: "ModuleLoadingProvider"
	id: parent

	function test_load_ProviderDetailView() {
		var item = createTemporaryQmlObject("
			import Governikus.Provider 1.0;
			ProviderDetailView {}
			", parent);
		item.destroy();
	}

	function test_load_ProviderInfoSection() {
		var item = createTemporaryQmlObject("
			import Governikus.Provider 1.0;
			ProviderInfoSection {}
			", parent);
		item.destroy();
	}
}
