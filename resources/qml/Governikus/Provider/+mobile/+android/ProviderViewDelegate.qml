import QtQuick 2.10

import Governikus.Global 1.0
import Governikus.Style 1.0

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
		anchors.rightMargin: ProviderStyle.providerListItemRightMargin
		anchors.top: parent.top
		anchors.topMargin: ProviderStyle.providerListItemTopMargin
		anchors.bottom: parent.bottom
		anchors.bottomMargin: ProviderStyle.providerListItemBottomMargin

		Item {
			property int childrenHeight: subjectText.height + addressText.height

			id: contentItem
			height: Constants.provider_section_height
			anchors.verticalCenter: parent.verticalCenter

			anchors.left: parent.left
			anchors.right: parent.right
			anchors.leftMargin: ProviderStyle.itemLeftMargin
			anchors.rightMargin: Utils.dp(15)

			property int detailsLinkWidth: height / ProviderStyle.infoItemWidthFactor

			Text {
				id: subjectText

				anchors.top: parent.top
				anchors.left: parent.left
				anchors.right: detailsLink.left

				anchors.topMargin: (Constants.provider_section_height - contentItem.childrenHeight) / 3
				anchors.rightMargin: Utils.dp(5)

				verticalAlignment: Text.AlignVCenter
				font.pixelSize: Constants.normal_font_size
				color: ProviderStyle.subjectTextColor
				font.bold: ProviderStyle.subjectTextFontBold
				elide: Text.ElideRight
				text: display
			}

			Text {
				id: addressText

				anchors.top: subjectText.bottom
				anchors.left: parent.left
				anchors.right: detailsLink.left

				anchors.topMargin: (Constants.provider_section_height - contentItem.childrenHeight) / 3
				anchors.bottomMargin: (Constants.provider_section_height - contentItem.childrenHeight) / 3

				verticalAlignment: Text.AlignVCenter
				font.pixelSize: ProviderStyle.addressTextFontSize
				color: ProviderStyle.addressTextColor
				elide: Text.ElideRight
				text: providerAddressDomain
			}

			Item {
				id: detailsLink
				anchors.right: parent.right
				anchors.verticalCenter: ProviderStyle.providerListDetailsLinkPosition === "top" ?
											subjectText.verticalCenter :
											parent.verticalCenter

				anchors.margins: Utils.dp(5)
				height: parent.height
				width: contentItem.detailsLinkWidth

				Rectangle {
					anchors.left: parent.left
					anchors.right: parent.right
					anchors.verticalCenter: parent.verticalCenter
					height: width

					border.color: ProviderStyle.providerListDetailsLinkBorder
					border.width: 1
					radius: width

					color: ProviderStyle.providerListDetailsLinkBackground

					Text {
						anchors.centerIn: parent
						text: "i"
						font.bold: ProviderStyle.providerListDetailsLinkBold
						color: ProviderStyle.providerListDetailsLinkColor
					}
				}
			}
		}
	}

	MouseArea {
		anchors.fill: parent
		onClicked: {
			providerDetailView.providerModelItem = model
			firePush(providerDetailView)
		}
	}

	Rectangle {
		width: parent.width
		anchors.top: parent.bottom
		anchors.topMargin: -height
		anchors.leftMargin: ProviderStyle.itemLeftMargin
		anchors.left: parent.left
		anchors.right: parent.right
		visible: ProviderStyle.providerListItemsHaveBorder
		height: visible ? 1 : 0
		color: Constants.grey
	}
}
