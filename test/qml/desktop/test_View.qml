/*
 * \copyright Copyright (c) 2018-2020 Governikus GmbH & Co. KG, Germany
 */

import QtTest 1.10

TestCase {
	name: "ModuleLoadingView"
	id: parent

	function test_load_Controller() {
		var item = createTemporaryQmlObject("
			import Governikus.View 1.0;
			Controller {}
			", parent);
		item.destroy();
	}
}
