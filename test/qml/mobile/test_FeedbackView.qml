import QtTest 1.10

TestCase {
	name: "ModuleImportTest"
	id: parent

	function test_load_FeedbackView() {
		var item = createTemporaryQmlObject("
			import Governikus.FeedbackView 1.0;
			Feedback {}
			", parent);
		item.destroy();
	}

	function test_load_Log() {
		var item = createTemporaryQmlObject("
			import Governikus.FeedbackView 1.0;
			Log {}
			", parent);
		item.destroy();
	}
}
