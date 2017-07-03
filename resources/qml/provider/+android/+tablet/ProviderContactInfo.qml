import QtQuick 2.6
import QtQuick.Layouts 1.2

import "../global"


Rectangle {
	id: baseItem

	height: parent.height

	readonly property int providerContactInfoItemHeight: Utils.dp(60)

	property color backgroundColor: PlatformConstants.blue
	property string homepage: ""
	property string email: ""
	property string phone: ""
	property string postalAddress: ""

	Column {
		height: parent.height
		width: parent.width

		Text {
			id: contactText
			text: qsTr("Contact")

			padding: Utils.dp(19)
			font.pixelSize: Constants.header_font_size
			color: "white"
		}

		Flickable {
			id: info
			height: parent.height - contactText.height
			width: parent.width

			contentHeight: baseItem.providerContactInfoItemHeight * 4 + 3
			contentWidth: width

			clip: true
			flickableDirection: Flickable.VerticalFlick
			boundsBehavior: Flickable.StopAtBounds

			Item {
				height: info.contentHeight
				width: parent.width

				Rectangle {
					anchors.fill: infoTable
					color: baseItem.backgroundColor
				}

				Column {
					id: infoTable

					height: info.contentHeight
					width: parent.width
					anchors.centerIn: parent

					ProviderContactInfoItem {
						height: baseItem.providerContactInfoItemHeight
						backgroundColor: baseItem.backgroundColor
						imageSource: Utils.providerIconSource("url")
						itemText: baseItem.homepage
					}

					Rectangle {
						anchors.left: infoTable.left
						height: 1
						width: parent.width
						color: "white"
					}

					ProviderContactInfoItem {
						height: baseItem.providerContactInfoItemHeight
						backgroundColor: baseItem.backgroundColor
						imageSource: Utils.providerIconSource("mail")
						itemText: baseItem.email
					}

					Rectangle {
						anchors.left: infoTable.left
						height: 1
						width: parent.width
						color: "white"
					}

					ProviderContactInfoItem {
						height: baseItem.providerContactInfoItemHeight
						backgroundColor: baseItem.backgroundColor
						imageSource: Utils.providerIconSource("telefon")
						itemText: baseItem.phone
					}

					Rectangle {
						anchors.left: infoTable.left
						height: 1
						width: parent.width
						color: "white"
					}

					ProviderContactInfoItem {
						height: baseItem.providerContactInfoItemHeight
						backgroundColor: baseItem.backgroundColor
						imageSource: Utils.providerIconSource("adresse")
						itemText: baseItem.postalAddress
					}
				}
			}
		}
	}
}
