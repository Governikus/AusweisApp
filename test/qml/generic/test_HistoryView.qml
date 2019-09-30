/*
 * \copyright Copyright (c) 2017-2019 Governikus GmbH & Co. KG, Germany
 */

import QtTest 1.10

TestCase {
	name: "ModuleImportTest"
	id: parent

	function test_load_HistoryView() {
		var item = createTemporaryQmlObject("
			import Governikus.HistoryView 1.0;
			HistoryView {}
			", parent);
		item.destroy();
	}
}
