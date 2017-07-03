import QtQml.Models 2.2
import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.2

import "../"
import "../global"

Rectangle {

	ProviderStyle {
		id: providerStyle
	}

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
		anchors.rightMargin: providerStyle.providerListItemRightMargin
		anchors.top: parent.top
		anchors.topMargin: providerStyle.providerListItemTopMargin
		anchors.bottom: parent.bottom
		anchors.bottomMargin: providerStyle.providerListItemBottomMargin

		Item {
			property int childrenHeight: subjectText.height + addressText.height

			id: contentItem
			height: Constants.provider_section_height
			anchors.verticalCenter: parent.verticalCenter

			anchors.left: parent.left
			anchors.right: parent.right
			anchors.leftMargin: providerStyle.itemLeftMargin
			anchors.rightMargin: Utils.dp(15)

			property int detailsLinkWidth: height / providerStyle.infoItemWidthFactor

			Text {
				id: subjectText

				anchors.top: parent.top
				anchors.left: parent.left
				anchors.right: detailsLink.left

				anchors.topMargin: (Constants.provider_section_height - contentItem.childrenHeight) / 3
				anchors.rightMargin: Utils.dp(5)

				verticalAlignment: Text.AlignVCenter
				font.pixelSize: Utils.sp(14)
				color: providerStyle.subjectTextColor
				font.bold: providerStyle.subjectTextFontBold
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
				font.pixelSize: providerStyle.addressTextFontSize
				color: providerStyle.addressTextColor
				elide: Text.ElideRight
				text: addressDomain
			}

			Item {
				id: detailsLink
				anchors.right: parent.right
				anchors.verticalCenter: providerStyle.providerListDetailsLinkPosition === "top" ?
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

					border.color: providerStyle.providerListDetailsLinkBorder
					border.width: 1
					radius: width

					color: providerStyle.providerListDetailsLinkBackground

					Text {
						anchors.centerIn: parent
						text: "i"
						font.bold: providerStyle.providerListDetailsLinkBold
						color: providerStyle.providerListDetailsLinkColor
					}
				}
			}
		}
	}

	MouseArea {
		anchors.fill: parent
		onClicked: {
			push(providerDetailView,
							  {
								  selectedCategory: category,
								  shortName: model.shortName,
								  longName: model.longName,
								  shortDescription: model.shortDescription,
								  longDescription: model.longDescription,
								  address: model.address,
								  homepage: model.homepage,
								  phone: model.phone,
								  email: model.email,
								  postalAddress: model.postalAddress,
								  providerIcon: model.icon,
								  providerImage: model.image
							  })
		}
	}

	Rectangle {
		width: parent.width
		anchors.top: parent.bottom
		anchors.topMargin: -height
		anchors.leftMargin: providerStyle.itemLeftMargin
		anchors.left: parent.left
		anchors.right: parent.right
		visible: providerStyle.providerListItemsHaveBorder
		height: visible ? 1 : 0
		color: Constants.grey
	}
}
