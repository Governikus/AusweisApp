/*
 * \copyright Copyright (c) 2017-2019 Governikus GmbH & Co. KG, Germany
 */

import QtTest 1.10

TestCase {
	name: "ModuleImportTest"
	id: parent

	function test_load_GRadioButton() {
		var item = createTemporaryQmlObject("
			import Governikus.Global 1.0;
			GRadioButton {}
			", parent);
		item.destroy();
	}

	function test_load_GSwitch() {
		var item = createTemporaryQmlObject("
			import Governikus.Global 1.0;
			GSwitch {}
			", parent);
		item.destroy();
	}

	function test_load_LocationButton() {
		var item = createTemporaryQmlObject("
			import Governikus.Global 1.0;
			LocationButton {}
			", parent);
		item.destroy();
	}

	function test_load_ListItem() {
		var item = createTemporaryQmlObject("
			import Governikus.Global 1.0;
			ListItem {}
			", parent);
		item.destroy();
	}

	function test_load_GFlickable() {
		var item = createTemporaryQmlObject("
			import Governikus.Global 1.0;
			GFlickable {}
			", parent);
		item.destroy();
	}

	function test_load_GListView() {
		var item = createTemporaryQmlObject("
			import Governikus.Global 1.0;
			GListView {}
			", parent);
		item.destroy();
	}

	function test_load_GGridView() {
		var item = createTemporaryQmlObject("
			import Governikus.Global 1.0;
			GGridView {}
			", parent);
		item.destroy();
	}

	function test_load_GScrollBar() {
		var item = createTemporaryQmlObject("
			import Governikus.Global 1.0;
			GScrollBar {}
			", parent);
		item.destroy();
	}
}
