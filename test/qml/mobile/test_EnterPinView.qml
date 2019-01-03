import QtTest 1.10

TestCase {
	name: "ModuleImportTest"
	id: parent

	function test_load_EnterPinView() {
		var item = createTemporaryQmlObject("
			import Governikus.EnterPinView 1.0;
			EnterPinView {}
			", parent);
		item.destroy();
	}
}
