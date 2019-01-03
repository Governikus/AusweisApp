import QtTest 1.10

TestCase {
	name: "ModuleLoadingView"
	id: parent

	function test_load_TabBarView() {
		var item = createTemporaryQmlObject("
			import Governikus.View 1.0;
			TabBarView {}
			", parent);
		item.destroy();
	}

	function test_load_ContentArea() {
		var item = createTemporaryQmlObject("
			import Governikus.View 1.0;
			ContentArea {}
			", parent);
		item.destroy();
	}
}
