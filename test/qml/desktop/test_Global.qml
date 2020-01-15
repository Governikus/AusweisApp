/*
 * \copyright Copyright (c) 2018-2020 Governikus GmbH & Co. KG, Germany
 */

import QtTest 1.10

TestCase {
	name: "ModuleImportTest"
	id: parent

	function test_load_BinaryDecisionView() {
		var item = createTemporaryQmlObject("
			import Governikus.Global 1.0;
			BinaryDecisionView {}
			", parent);
		item.destroy();
	}

	function test_load_NavigationButton() {
		var item = createTemporaryQmlObject("
			import Governikus.Global 1.0;
			NavigationButton {
				buttonType: NavigationButton.Type.Forward
			}
			", parent);
		item.destroy();
	}

	function test_load_ScrollablePane() {
		var item = createTemporaryQmlObject("
			import Governikus.Global 1.0;
			ScrollablePane {}
			", parent);
		item.destroy();
	}

	function test_load_TabbedPane() {
		var item = createTemporaryQmlObject("
			import QtQuick 2.10
			import Governikus.Global 1.0
			import QtQml.Models 2.10;
			TabbedPane {
				id: pane

				anchors.fill: parent
				anchors.margins: Constants.pane_padding

				sectionsModel: ['Item 1', 'Item 2', 'Item 3']
				contentObjectModel: ObjectModel {
					Component { Rectangle { color: 'red' } }
					Component { Rectangle { color: 'blue' } }
					Component { Rectangle { color: 'black' } }
				}
			}
			", parent);
		item.destroy();
	}
}
