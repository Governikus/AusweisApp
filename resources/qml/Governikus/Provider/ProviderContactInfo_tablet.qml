import QtQuick 2.6
import QtQuick.Layouts 1.2

import Governikus.Global 1.0

Rectangle {
	id: baseItem
	property alias contactModel: contactListView.model

	onWidthChanged: info.resetSize()
	onHeightChanged: info.resetSize()

	Column {
		id: info
		width: parent.width

		property int sizeRecudctor: 0
		function resetSize() {
			if (childrenRect.height < baseItem.height) {
				sizeRecudctor = 0
			}
		}
		function approximateSize() {
			if (childrenRect.height > baseItem.height) {
				sizeRecudctor = sizeRecudctor +1
			}
		}

		onWidthChanged: info.approximateSize()
		onHeightChanged: info.approximateSize()
		onVisibleChanged: { info.resetSize(); info.approximateSize() }
		onChildrenRectChanged: info.approximateSize()

		Text {
			text: qsTr("Contact") + settingsModel.translationTrigger
			padding: Constants.component_spacing
			font.pixelSize: Constants.header_font_size
			color: "white"
		}
		Rectangle {
			// The delegates have space in between which is
			// therefore the color of this very Rectangle.
			anchors.left: parent.left
			anchors.right: parent.right
			height: contactListView.height
			color: "white"

			ListView {
				id: contactListView
				width: parent.width
				height: contentHeight
				interactive: false
				spacing: 2
				delegate:  ProviderContactInfoItem_tablet {
					anchors.left: parent.left
					width: contactListView.width
					color: baseItem.color
					imageSource: Qt.resolvedUrl(model.iconSource)
					itemText: (!!model.text ? model.text : qsTr("Unknown")) + settingsModel.translationTrigger
					link: model.link
					sizeRecudctor: info.sizeRecudctor
				}
			}
		}
	}
}
