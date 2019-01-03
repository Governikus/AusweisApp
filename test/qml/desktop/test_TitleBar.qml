import QtTest 1.10

TestCase {
	name: "ModuleImportTest"
	id: parent

	function test_load_CancelAction() {
		var item = createTemporaryQmlObject("
			import Governikus.TitleBar 1.0;
			CancelAction {}
			", parent);
		item.destroy();
	}
}
