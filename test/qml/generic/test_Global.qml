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

		verify(Constants.titlebar_padding > 0)
		verify(Constants.titlebar_spacing > 0)
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
}
