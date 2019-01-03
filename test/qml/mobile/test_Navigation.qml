import QtTest 1.10

TestCase {
	name: "ModuleImportTest"
	id: parent

	function test_load_Navigation() {
		var item = createTemporaryQmlObject("
			import Governikus.Navigation 1.0;
			Navigation {}
			", parent);
		item.destroy();
	}
}
