import QtQml.Models 2.2
import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.2

import Governikus.Global 1.0

Rectangle {
	id: baseItem
	width: parent.width
	height: Constants.provider_section_height
	color: Constants.background_color
	clip: true

	Rectangle {
		id: background
		color: "white"
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.rightMargin:  Utils.dp(5)
		anchors.top: parent.top
		anchors.topMargin: Utils.dp(2)
		anchors.bottom: parent.bottom
		anchors.bottomMargin: Utils.dp(5)

		Item {
			height: subjectText.height + addressText.height
			anchors.verticalCenter: parent.verticalCenter

			anchors.left: parent.left
			anchors.leftMargin: Utils.dp(5)
			anchors.right: detailsLink.left
			anchors.rightMargin: Utils.dp(15)

			Text {
				id: subjectText
				color: Constants.secondary_text
				width: parent.width
				verticalAlignment: Text.AlignVCenter
				font.pixelSize: Constants.normal_font_size
				elide: Text.ElideRight
				text: display
			}
			Text {
				id: addressText
				anchors.top: subjectText.bottom
				width: parent.width

				verticalAlignment: Text.AlignVCenter
				font.pixelSize: Constants.small_font_size
				color: PlatformConstants.blue_dark
				elide: Text.ElideRight
				text: providerAddressDomain
			}
		}
		Item {
			id: detailsLink
			anchors.right: parent.right
			anchors.margins: Utils.dp(5)
			height: parent.height
			width: parent.height / 2

			Rectangle {
				anchors.left: parent.left
				anchors.right: parent.right
				anchors.verticalCenter: parent.verticalCenter
				height: width

				border.color: PlatformConstants.blue_dark
				border.width: 1
				radius: width

				Text {
					anchors.centerIn: parent
					text: qsTr("i") + settingsModel.translationTrigger
					color: parent.border.color
				}
			}
		}
	}

	MouseArea {
		anchors.fill: parent
		onClicked: firePush(providerDetailView, {providerModelItem: model})
	}
}
