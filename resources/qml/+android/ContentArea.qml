import QtQuick 2.5

import "global"

Item {
	id: baseItem

	function getVisibleItem() {
		return baseItem.visibleChildren[0]
	}

	TabBarView {
		id: identifyView
		anchors.fill: parent
		visible: baseItem.state === "identify"
		source: "identify/IdentifyView.qml"
	}

	TabBarView {
		anchors.fill: parent
		visible: baseItem.state === "provider"
		source: "provider/ProviderView.qml"
	}

	TabBarView {
		anchors.fill: parent
		visible: baseItem.state === "history"
		source: "history/HistoryView.qml"
	}

	TabBarView {
		id: pinView
		anchors.fill: parent
		visible: baseItem.state === "pin"
		source: "pin/PinView.qml"
	}

	TabBarView {
		anchors.fill: parent
		visible: baseItem.state === "versionInformation"
		source: "more/VersionInformation.qml"
	}

	TabBarView {
		anchors.fill: parent
		visible: baseItem.state === "developeroptions"
		source: "more/DeveloperView.qml"
	}
}
