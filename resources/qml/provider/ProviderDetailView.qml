import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.2
import QtQuick.Controls 2.0 as QtControls

import "../"
import "../global"

SectionPage
{
	id: baseItem

	leftTitleBarAction: TitleBarMenuAction { state: "back"; onClicked: pop() }
	headerTitleBarAction: TitleBarAction { text: shortName }
	titleBarColor: Category.displayColor(selectedCategory)

	property var selectedProviderModel: ({})

	property string selectedCategory

	// Details
	property string shortName
	property string longName
	property string shortDescription
	property string longDescription
	property string address
	property string homepage
	property string phone
	property string email
	property string postalAddress
	property string providerIcon
	property string providerImage

	ProviderHeader {
		id: header

		height: Utils.dp(200)

		address: parent.address
		providerIcon: parent.providerIcon !== "" ? parent.providerIcon : Category.imageSource(parent.selectedCategory)
		providerImage: parent.providerImage !== "" ? parent.providerImage : Category.backgroundImageSource(parent.selectedCategory)
	}


	Rectangle {
		id: swipeBar
		anchors.top: header.bottom
		anchors.topMargin: Utils.dp(20)
		anchors.horizontalCenter: parent.horizontalCenter

		width: descriptionTab.width + contactTab.width + 2 * border.width
		height: descriptionTab.height  + 2 * border.width

		border.color: Constants.blue
		border.width: Utils.dp(1)
		radius: Utils.dp(3)
		clip: true

		Row {
			id: row
			readonly property int maxContentWidth: Math.max(descriptionText.contentWidth, contactText.contentWidth)

			anchors.centerIn: parent
			Rectangle {
				id: descriptionTab
				width: row.maxContentWidth + Utils.dp(6)
				height: descriptionText.contentHeight + Utils.dp(6)
				color: swipeView.currentIndex === 0 ? Constants.blue : descriptiontMouseArea.pressed ? Constants.blue_light : "transparent"
				Text {
					id: descriptionText
					anchors.centerIn: parent
					color: swipeView.currentIndex === 0 ? "white" : Constants.blue
					text: qsTr("Description")
				}
				MouseArea {
					id: descriptiontMouseArea
					anchors.fill: parent
					onClicked: swipeView.currentIndex = 0
				}
			}
			Rectangle {
				id: contactTab
				width: row.maxContentWidth + Utils.dp(6)
				height: contactText.contentHeight + Utils.dp(6)
				color: swipeView.currentIndex === 1 ? Constants.blue : contactMouseArea.pressed ? Constants.blue_light : "transparent"
				Text {
					id: contactText
					anchors.centerIn: parent
					color: swipeView.currentIndex === 1 ? "white" : Constants.blue
					text: qsTr("Contact")
				}
				MouseArea {
					id: contactMouseArea
					anchors.fill: parent
					onClicked: swipeView.currentIndex = 1
				}
			}
		}
	}


	Rectangle {
		anchors.fill: flickable
	}

	Flickable {
		id: flickable
		anchors.topMargin: Utils.dp(10)
		anchors.top: swipeBar.bottom
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.bottom: parent.bottom
		flickableDirection: Flickable.VerticalFlick
		clip: true

		QtControls.SwipeView {
			id: swipeView
			anchors.fill: parent
			anchors.margins: Utils.dp(20)
			clip: true

			Text {
				text: longDescription ? longDescription : qsTr("Description not avaible")

				wrapMode: Text.WordWrap
				font.pixelSize: Utils.dp(16)
			}

			ProviderContactTab {
				homepage: baseItem.homepage ? baseItem.homepage : ""
				phone: baseItem.phone ? baseItem.phone : ""
				email: baseItem.email ? baseItem.email : ""
				postalAddress: baseItem.postalAddress ? baseItem.postalAddress : ""
			}
		}
	}
}
