import QtTest 1.10

TestCase {
	name: "ModuleImportTest"
	id: parent

	function test_load_TechnologyInfo() {
		var item = createTemporaryQmlObject("
			import Governikus.TechnologyInfo 1.0;
			TechnologyInfo {}
			", parent);
		item.destroy();
	}
}
