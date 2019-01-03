import QtTest 1.10

TestCase {
	name: "ModuleImportTest"
	id: parent

	function test_load_ContinueButton() {
		var item = createTemporaryQmlObject("
			import Governikus.Global 1.0;
			ContinueButton {}
			", parent);
		item.destroy();
	}
}
