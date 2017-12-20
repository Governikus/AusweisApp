import QtQuick 2.6
import QtQuick.Layouts 1.2

import AusweisApp2.Global 1.0


Rectangle {
	id: baseItem

	property alias contactModel: contactListView.model

	Column {
		id: info
		width: baseItem.width / info.scaleFactor // fill whole width
		transform: Scale { yScale: info.scaleFactor; xScale: info.scaleFactor }

		readonly property real scaleFactor: {
			var unscaledElementsAccumulatedHeight = 3
			var factor =  height > baseItem.height ? (baseItem.height - unscaledElementsAccumulatedHeight) / height : 1
			if (factor <= 0) {
				return 1
			}
			return factor
		}

		Text {
			text: qsTr("Contact") + settingsModel.translationTrigger
			padding: Constants.component_spacing
			font.pixelSize: Constants.header_font_size
			color: "white"
		}
		Rectangle {
			anchors.left: parent.left
			anchors.right: parent.right
			height: contactListView.height
			ListView {
				id: contactListView
				width: parent.width
				height: contentHeight
				interactive: false
				spacing: 2
				delegate:  ProviderContactInfoItem {
					width: contactListView.width
					color: baseItem.color
					imageSource: Qt.resolvedUrl(model.iconSource)
					itemText: (!!model.text ? model.text : qsTr("Unknown")) + settingsModel.translationTrigger
					link: model.link
				}
			}
		}
	}
}
