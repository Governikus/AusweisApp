import QtTest 1.10

TestCase {
	name: "ModuleImportTest"
	id: parent

	function test_load_TutorialView() {
		var item = createTemporaryQmlObject("
			import Governikus.TutorialView 1.0;
			TutorialView {}
			", parent);
		item.destroy();
	}
}
