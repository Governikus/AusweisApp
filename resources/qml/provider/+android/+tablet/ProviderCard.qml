import QtQuick 2.6
import QtQuick.Layouts 1.2

import "../../../global"

Rectangle {
	id: baseItem

	readonly property int sidePadding: width * 0.0512
	readonly property int nameFontSize: Constants.normal_font_size
	readonly property int homepageFontSize: Constants.normal_font_size

	readonly property int headerHeight: height * 0.4752
	readonly property int nameHeight: height * 0.3168
	readonly property int footerHeight: height - (headerHeight + nameHeight)

	property string providerCategory: ""
	property string headerImage: ""
	property string headerIcon: ""
	property string providerName: ""
	property string providerShortDescription: ""
	property string providerLongDescription: ""
	property string providerAddress: ""
	property string providerHomepageBase: ""
	property string providerEmail: ""
	property string providerPhone: ""
	property string providerPostalAddress: ""

	property var pushFunction: function(providerDetails) {}

	border.width: 2
	border.color: "black"

	Column {
		height: baseItem.height
		width: baseItem.width

		Image {
			source: baseItem.headerImage !== "" ? baseItem.headerImage :
					Category.backgroundImageSource(baseItem.providerCategory)

			height: baseItem.headerHeight
			width: parent.width
			fillMode: Image.Stretch
			anchors.horizontalCenter: parent.horizontalCenter
		}

		ProviderCardNameRow {
			headerIcon: baseItem.headerIcon
			nameHeight: baseItem.nameHeight
			providerCategory: baseItem.providerCategory
			sidePadding: baseItem.sidePadding
		}

		Rectangle {
			color: Category.displayColor(baseItem.providerCategory)
			height: baseItem.footerHeight
			width: parent.width

			Text {
				text: baseItem.providerHomepageBase

				anchors.centerIn: parent

				leftPadding: baseItem.sidePadding
				rightPadding: baseItem.sidePadding
				elide: Text.ElideRight
				maximumLineCount: 1

				font.pixelSize: baseItem.homepageFontSize
				color: "white"
			}
		}
	}

	MouseArea {
		anchors.fill: parent
		onClicked: baseItem.pushFunction(
							  {
								  selectedCategory: baseItem.providerCategory,
								  shortName: baseItem.providerName,
								  longName: baseItem.providerName,
								  shortDescription: baseItem.providerShortDescription,
								  longDescription: baseItem.providerLongDescription,
								  address: baseItem.providerAddress,
								  homepage: "",
								  homepageBase: baseItem.providerHomepageBase,
								  phone: baseItem.providerPhone,
								  email: baseItem.providerEmail,
								  postalAddress: baseItem.providerPostalAddress,
								  providerIcon: baseItem.headerIcon,
								  providerImage: baseItem.headerImage
							  })
	}
}
