/*
 * \copyright Copyright (c) 2019 Governikus GmbH & Co. KG, Germany
 */

import QtTest 1.10

TestCase {
	name: "ModuleImportTest"
	id: parent

	function test_load_SettingsView() {
		var item = createTemporaryQmlObject("
			import Governikus.SettingsView 1.0;
			SettingsView {}
			", parent);
		item.destroy();
	}

	function test_load_ReaderView() {
		var item = createTemporaryQmlObject("
			import Governikus.SettingsView 1.0;
			TabbedReaderView {}
			", parent);
		item.destroy();
	}
}
