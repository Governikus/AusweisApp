import QtTest 1.10

TestCase {
	name: "ModuleLoadingProvider"
	id: parent

	function test_load_ProviderInfoSection() {
		var item = createTemporaryQmlObject("
			import Governikus.Provider 1.0;
			ProviderInfoSection {}
			", parent);
		item.destroy();
	}
}
