/*
 * \copyright Copyright (c) 2019-2022 Governikus GmbH & Co. KG, Germany
 */

import QtTest 1.12

TestCase {
	name: "ModuleImportTest"
	id: parent

	function test_load_SettingsView() {
		var item = createTemporaryQmlObject("
			import QtQuick 2.12
			import Governikus.SettingsView 1.0;

			Item {
				id: appWindow

				property var menuBar: Item {
					function updateActions() {}
				}

				SettingsView {}
			}
			", parent);
		item.destroy();
	}

	function test_load_ReaderView() {
		var item = createTemporaryQmlObject("
			import QtQuick 2.12
			import Governikus.SettingsView 1.0;

			Item {
				id: appWindow

				property var menuBar: Item {
					function updateActions() {}
				}

				TabbedReaderView {}
			}
			", parent);
		item.destroy();
	}
}
