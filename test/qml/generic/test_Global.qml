/*
 * \copyright Copyright (c) 2017-2019 Governikus GmbH & Co. KG, Germany
 */

import QtTest 1.10

import Governikus.Global 1.0

TestCase {
	name: "ModuleImportTest"
	id: parent


	function test_load_Global() {
		var item = createTemporaryQmlObject("
			import QtQuick 2.0;
			import Governikus.Global 1.0;
			Item {}
			", parent);
		item.destroy();
	}

	function test_Constants() {
		compare(Constants.black, "#000000")
		compare(Constants.white, "#ffffff")
	}

	function test_load_StatusIcon() {
		var item = createTemporaryQmlObject("
			import Governikus.Global 1.0;
			StatusIcon {}
			", parent);
		item.destroy();
	}

	function test_load_LabeledText() {
		var item = createTemporaryQmlObject("
			import Governikus.Global 1.0;
			LabeledText {}
			", parent);
		item.destroy();
	}

	function test_load_GButton() {
		var item = createTemporaryQmlObject("
			import Governikus.Global 1.0;
			GButton {}
			", parent);
		item.destroy();
	}

	function test_load_GText() {
		var item = createTemporaryQmlObject("
			import Governikus.Global 1.0;
			GText {}
			", parent);
		item.destroy();
	}

	function test_load_PaneTitle() {
		var item = createTemporaryQmlObject("
			import Governikus.Global 1.0;
			PaneTitle {}
			", parent);
		item.destroy();
	}

	function test_load_Pane() {
		var item = createTemporaryQmlObject("
			import Governikus.Global 1.0;
			Pane {}
			", parent);
		item.destroy();
	}

	function test_load_GCheckBox() {
		var item = createTemporaryQmlObject("
			import Governikus.Global 1.0;
			GCheckBox {}
			", parent);
		item.destroy();
	}

	function test_load_GTextField() {
		var item = createTemporaryQmlObject("
			import Governikus.Global 1.0;
			GTextField {}
			", parent);
		item.destroy();
	}

	function test_load_GComboBox() {
		var item = createTemporaryQmlObject("
			import Governikus.Global 1.0;
			GComboBox {}
			", parent);
		item.destroy();
	}

	function test_load_ConfirmationPopup() {
		var item = createTemporaryQmlObject("
			import Governikus.Global 1.0;
			ConfirmationPopup {}
			", parent);
		item.destroy();
	}

	function test_load_SearchBar() {
		var item = createTemporaryQmlObject("
			import Governikus.Global 1.0;
			SearchBar {}
			", parent);
		item.destroy();
	}

	function test_load_GSeparator() {
		var item = createTemporaryQmlObject("
			import Governikus.Global 1.0

			GSeparator {
				width: parent.width
				anchors.verticalCenter: parent.verticalCenter
			}
			", parent);
		item.destroy();
	}
}
