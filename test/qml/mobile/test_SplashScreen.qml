import QtTest 1.10

TestCase {
	name: "ModuleImportTest"
	id: parent

	function test_load_SplashScreen() {
		var item = createTemporaryQmlObject("
			import Governikus.SplashScreen 1.0;
			SplashScreen {}
			", parent);
		item.destroy();
	}
}
