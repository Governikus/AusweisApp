/*
 * \copyright Copyright (c) 2017-2023 Governikus GmbH & Co. KG, Germany
 */
import QtTest 1.15

TestCase {
	id: parent
	function test_load_HistoryView() {
		var item = createTemporaryQmlObject("
			import Governikus.HistoryView 1.0;
			HistoryView {}
			", parent);
		item.destroy();
	}

	name: "ModuleImportTest"
}
