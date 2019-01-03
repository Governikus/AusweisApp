import QtTest 1.10

TestCase {
	name: "ModuleImportTest"
	id: parent

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

	function test_load_GTextField() {
		var item = createTemporaryQmlObject("
			import Governikus.Global 1.0;
			GTextField {}
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
}
