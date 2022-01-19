/*
 * \copyright Copyright (c) 2018-2022 Governikus GmbH & Co. KG, Germany
 */

import QtTest 1.12

TestCase {
	name: "ModuleLoadingView"
	id: parent

	function test_load_TabBarView() {
		var item = createTemporaryQmlObject("
			import Governikus.View 1.0;
			TabBarView {}
			", parent);
		item.destroy();
	}

	function test_load_ContentArea() {
		var item = createTemporaryQmlObject("
			import Governikus.View 1.0;
			ContentArea {}
			", parent);
		item.destroy();
	}
}
