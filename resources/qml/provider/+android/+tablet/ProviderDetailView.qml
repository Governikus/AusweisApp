import QtQuick 2.6

import "."
import ".."
import "../global"


Item {
	id: baseItem

	// Details are passed through modelItem when called from HistoryViewPage,
	// otherwise they must be set explicitly
	property var modelItem: null

	onModelItemChanged: {
		if (modelItem !== null) {
			selectedCategory = modelItem.provider_category
			shortName = modelItem.provider_shortname
			longName = modelItem.provider_longname
			shortDescription = modelItem.provider_shortdescription
			longDescription = modelItem.provider_longdescription
			address = modelItem.provider_address
			homepage = modelItem.provider_homepage
			homepageBase = modelItem.provider_homepage_base
			phone = modelItem.provider_phone
			email = modelItem.provider_email
			postalAddress = modelItem.provider_postaladdress
			providerIcon = modelItem.provider_icon
			providerImage = modelItem.provider_image
		}
	}

	property string selectedCategory
	property string shortName
	property string longName
	property string shortDescription
	property string longDescription
	property string address
	property string homepage
	property string homepageBase
	property string phone
	property string email
	property string postalAddress
	property string providerIcon
	property string providerImage

	function historyInfoIsOpen() {
		return providerDetailsHistoryInfo.visible
	}

	function closeHistoryInfo() {
		providerDetailsHistoryInfo.visible = false
	}

	property TitleBarMenuAction leftTitleBarAction: TitleBarMenuAction {
		state: "back"
		onClicked: {
			if (baseItem.historyInfoIsOpen()) {
				baseItem.closeHistoryInfo()
			}
			else {
				pop()
			}
		}
	}

	property TitleBarAction headerTitleBarAction: TitleBarAction { text: shortName; font.bold: true }
	property Item rightTitleBarAction: Item {}
	property color titleBarColor: Category.displayColor(selectedCategory)

	readonly property real titleBarOpacity: 1

	Column {
		id: mainContent

		height: parent.height
		width: parent.width

		readonly property real contactRatio: 0.5
		readonly property real buttonBarRatio: 0.25
		readonly property real extraInfoRatio: 0.25

		Row {
			id: contactInfo

			height: parent.height * mainContent.contactRatio
			width: parent.width

			Item {
				height: parent.height
				width: baseItem.width * 2 / 3
				anchors.top: parent.top

				Image {
					id: image
					source: baseItem.providerImage !== "" ? baseItem.providerImage :
							Category.backgroundImageSource(baseItem.selectedCategory)

					fillMode: Image.PreserveAspectFit
					height: parent.height
					anchors.left: parent.left
					anchors.verticalCenter: parent.verticalCenter
				}

				Image {
					id: imageBackgroundLeft
					source: Category.gradientImageSource(baseItem.selectedCategory)

					fillMode: Image.Stretch
					anchors.right: imageBackgroundRight.left
					anchors.top: parent.top
					height: parent.height
					width: height / 2
				}

				Rectangle {
					id: imageBackgroundRight
					anchors.right: parent.right
					anchors.top: parent.top
					height: parent.height
					width: height

					color: baseItem.titleBarColor
				}
			}

			ProviderContactInfo {
				backgroundColor: baseItem.titleBarColor
				color: baseItem.titleBarColor
				width: baseItem.width / 3

				homepage: baseItem.homepageBase
				email: baseItem.email
				phone: baseItem.phone
				postalAddress: baseItem.postalAddress
			}
		}

		Row {
			id: lowerRow

			height: mainContent.height * (mainContent.buttonBarRatio + mainContent.extraInfoRatio)
			width: baseItem.width

			Column {
				id: leftColumn

				height: lowerRow.height
				width: lowerRow.width * 2 / 3
				anchors.top: lowerRow.top

				ProviderDetailButtonBar {
					selectedCategory: baseItem.selectedCategory
					providerIcon: baseItem.providerIcon
					address: baseItem.address
					titleBarColor: baseItem.titleBarColor

					height: mainContent.height * mainContent.buttonBarRatio
					width: leftColumn.width
					anchors.left: leftColumn.left
				}

				ProviderDetailDescription {
					height: mainContent.height * mainContent.extraInfoRatio
					width: leftColumn.width
					anchors.left: leftColumn.left

					description: baseItem.longDescription
				}
			}

			ProviderDetailHistory {
				height: lowerRow.height
				width: lowerRow.width / 3
				anchors.top: lowerRow.top
				openHistoryInfoFunc: baseItem.openHistoryInfoFunc
			}
		}
	}

	property var openHistoryInfoFunc: function(entryInfo) {
		providerDetailsHistoryInfo.visible = true

		providerDetailsHistoryInfo.providerName = entryInfo['providerName']
		providerDetailsHistoryInfo.providerPostalAddress = entryInfo['providerPostalAddress']
		providerDetailsHistoryInfo.purposeText = entryInfo['purposeText']
		providerDetailsHistoryInfo.requestedDataText = entryInfo['requestedDataText']
		providerDetailsHistoryInfo.termsOfUsageText = entryInfo['termsOfUsageText']
	}

	ProviderDetailHistoryInfo {
		id: providerDetailsHistoryInfo

		height: parent.height
		width: parent.width

		anchors.top: mainContent.top
		anchors.left: mainContent.left

		visible: false
	}
}
