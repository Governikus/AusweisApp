/*
 * \copyright Copyright (c) 2017-2020 Governikus GmbH & Co. KG, Germany
 */

import QtTest 1.12

TestCase {
	name: "ModuleImportTest"
	id: parent

	function test_load_SetupAssistantView() {
		var item = createTemporaryQmlObject("
			import QtQuick 2.12
			import Governikus.TutorialView 1.0;

			Item {
				id: appWindow

				property var menuBar: Item {
					function updateActions() {}
				}

				SetupAssistantView {}
			}
			", parent);
		item.destroy();
	}
}
