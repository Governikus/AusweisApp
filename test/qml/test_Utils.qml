import QtQuick 2.3
import QtTest 1.0

// import Governikus.Global 1.0

TestCase {
	name: "MathTests"


	function test_escapeHtml() {
		// compare(Utils.escapeHtml("a&b"), "a&amp;b", "escape &")
		// compare(Utils.escapeHtml("<br/>"), "&lt;br/&gt;", "escape < and >")
		// compare(Utils.escapeHtml("\"Hello\""), "&quot;Hello&quot;", "escape \"")
	}

}
