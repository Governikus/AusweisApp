import QtQuick 2.10

import Governikus.TitleBar 1.0

Item {
	enum Views {
		Main = 1,
		Identify,
		Provider
	}

	anchors.fill: parent

	signal nextView(int pName)

	readonly property bool sectionPageTypeMarker: true

	property TitleBarAction titleBarAction: null
	property Item navSuccessor: null
}
