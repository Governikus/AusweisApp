import QtTest 1.10

TestCase {
	name: "ModuleImportTest"
	id: parent

	function test_load_MoreView() {
		var item = createTemporaryQmlObject("
			import Governikus.MoreView 1.0;
			MoreView {}
			", parent);
		item.destroy();
	}
}
