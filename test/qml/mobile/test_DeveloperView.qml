import QtTest 1.10

TestCase {
	name: "ModuleImportTest"
	id: parent

	function test_load_DeveloperView() {
		var item = createTemporaryQmlObject("
			import Governikus.DeveloperView 1.0;
			DeveloperView {}
			", parent);
		item.destroy();
	}
}
