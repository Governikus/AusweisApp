import QtTest 1.10

TestCase {
	name: "ModuleLoadingInformationView"
	id: parent

	function test_load_Information() {
		var item = createTemporaryQmlObject("
			import Governikus.InformationView 1.0;
			Information {}
			", parent);
		item.destroy();
	}

	function test_load_VersionInformationView() {
		var item = createTemporaryQmlObject("
			import Governikus.InformationView 1.0;
			VersionInformation {}
			", parent);
		item.destroy();
	}
}
