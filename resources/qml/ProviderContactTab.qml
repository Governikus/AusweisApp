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

		onHomepageChanged: setProperty(0, "text", '<a href="' + homepage + '">' + homepage + "</a>")
		onPhoneChanged: setProperty(1, "text", '<a href="tel:' + phone + '">' + phone + "</a>")
		onEmailChanged: setProperty(2, "text", '<a href="mailto:' + email + '">' + email + "</a>")
		onPostalAddressChanged: setProperty(3, "text", postalAddress)

		ListElement {
			label: qsTr("Homepage")
			text: ""
		}
		ListElement {
			label: qsTr("Phone")
			text: ""
		}
		ListElement {
			label: qsTr("E-Mail")
			text: ""
		}
		ListElement {
			label: qsTr("Contact")
			text: ""
		}
	}


	Pane {
		id: pane
		width: baseItem.width

		Repeater {
			id: repeater
			model: providerInfoModel

			Item {
				height: info.height + Utils.dp(20)
				width: pane.width

				LabeledText {
					id: info
					label: model.label
					text: model.text
					anchors.verticalCenter: parent.verticalCenter
					width: parent.width
					margin: 0
					onLinkActivated: Qt.openUrlExternally(link)
				}
				Rectangle {
					visible: index != repeater.count - 1
					anchors.bottom: parent.bottom
					anchors.left: info.left
					anchors.right: parent.right
					height: 1
					color: Constants.grey
				}
			}
		}
	}
}
