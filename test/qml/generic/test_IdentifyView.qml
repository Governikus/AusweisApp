import QtTest 1.10

TestCase {
	name: "ModuleImportTest"
	id: parent

	function test_load_IdentifyView() {
		var item = createTemporaryQmlObject("
			import Governikus.IdentifyView 1.0;
			IdentifyView {}
			", parent);
		item.destroy();
	}
}
