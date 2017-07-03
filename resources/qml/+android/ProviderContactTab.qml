import QtQuick 2.5
import QtQuick.Layouts 1.2

import "../"
import "global"

Item {
	id: baseItem

	property string homepage
	property string phone
	property string email
	property string postalAddress

	ListModel {
		id: providerInfoModel

		property string homepage: baseItem.homepage
		property string phone: baseItem.phone
		property string email: baseItem.email
		property string postalAddress: baseItem.postalAddress

		property string cssBegin: "<html><style>a {text-decoration: none; color: '" + Qt.black + "'} </style><body>"
		property string cssEnd: "</body></html>"

		onHomepageChanged: setProperty(0, "text", cssBegin + '<a href="' + homepage + '">' + homepage + "</a>" + cssEnd)
		onEmailChanged: setProperty(1, "text", cssBegin + '<a href="mailto:' + email + '">' + email + "</a>" + cssEnd)
		onPhoneChanged: setProperty(2, "text", cssBegin + '<a href="tel:' + phone + '">' + phone + "</a>" + cssEnd)
		onPostalAddressChanged: setProperty(3, "text", cssBegin + '<a href="geo:0,0?q=' + encodeURIComponent(postalAddress.replace(/\<br>/g,' ')) + '">' + postalAddress + "</a>" + cssEnd)


		ListElement {
			imageSource: "qrc:///images/provider/url.png"
			text: ""
		}

		ListElement {
			imageSource: "qrc:///images/provider/mail.png"
			text: ""
		}

		ListElement {
			imageSource: "qrc:///images/provider/telefon.png"
			text: ""
		}

		ListElement {
			imageSource: "qrc:///images/provider/adresse.png"
			text: ""
		}
	}


	ListView {
		model: providerInfoModel
		anchors.fill: parent

		delegate: Item {
			id: delegateItem
			width: parent.width
			height: Math.max(textItem.height, Utils.dp(50))

			Image {
				id: imageItem
				fillMode: Image.PreserveAspectFit
				height: Utils.dp(24)
				width: Utils.dp(24)
				anchors.left: parent.left
				anchors.leftMargin: Utils.dp(15)
				anchors.verticalCenter: parent.verticalCenter
				source: model.imageSource
			}

			Text {
				id: textItem
				text: model.text
				verticalAlignment: Text.AlignVCenter
				anchors.left: imageItem.right
				anchors.leftMargin: Utils.dp(20)
				anchors.right: parent.right
				anchors.rightMargin: Utils.dp(10)
				anchors.verticalCenter: parent.verticalCenter
				font.pixelSize: Utils.dp(16)
				wrapMode: Text.WordWrap
				textFormat: Text.RichText
				onLinkActivated: Qt.openUrlExternally(link)
			}

			MouseArea {
				anchors.fill: delegateItem
				onClicked: {
					Qt.openUrlExternally(textItem.linkAt(textItem.x, textItem.y))
				}
			}

			Rectangle {
				anchors.bottom: parent.bottom
				anchors.left: parent.left
				anchors.right: parent.right
				height: 1
				color: Constants.grey
			}
		}
	}
}
