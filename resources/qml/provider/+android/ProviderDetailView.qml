import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.2
import QtQuick.Controls 2.0 as QtControls

import "../"
import "../global"

SectionPage {
	id: baseItem

	leftTitleBarAction: TitleBarMenuAction { state: "back"; onClicked: pop() }

	headerTitleBarAction: TitleBarAction { text: shortName }

	titleBarColor: Category.displayColor(selectedCategory)

	property var selectedProviderModel: ({})

	property string selectedCategory

	readonly property real headerHeight: Utils.dp(200)

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

	header: ProviderHeader {
		width: baseItem.width
		height: headerHeight

		address: baseItem.address
		providerIcon: baseItem.providerIcon !== "" ? baseItem.providerIcon : Category.buttonImageSource(baseItem.selectedCategory)
		providerImage: baseItem.providerImage !== "" ? baseItem.providerImage : Category.backgroundImageSource(baseItem.selectedCategory)
		transparentColor: Category.displayColor(selectedCategory)
	}

	content: Item {
		width: baseItem.width
		height: baseItem.height

		QtControls.TabBar {
			id: swipeBar
			height: firstButton.height
			anchors.top: parent.top
			anchors.topMargin: Utils.dp(20)
			anchors.left: parent.left
			anchors.right: parent.right

			currentIndex: swipeView.currentIndex

			QtControls.TabButton {
				id: firstButton
				padding: Utils.dp(10)
				contentItem: Text {
					text: qsTr("DESCRIPTION")
					font.pixelSize: Constants.normal_font_size
					elide: Text.ElideRight
					opacity: enabled ? 1 : 0.3
					color: !parent.checked ? "black" : parent.pressed ? "black" : Constants.blue
					horizontalAlignment: Text.AlignHCenter
					verticalAlignment: Text.AlignVCenter
				}
			}

			QtControls.TabButton {
				padding: Utils.dp(10)
				contentItem: Text {
					text: qsTr("CONTACT")
					font.pixelSize: Constants.normal_font_size
					elide: Text.ElideRight
					opacity: enabled ? 1 : 0.3
					color: !parent.checked ? "black" : parent.pressed ? "black" : Constants.blue
					horizontalAlignment: Text.AlignHCenter
					verticalAlignment: Text.AlignVCenter
				}
			}
		}

		Rectangle {
			anchors.fill: flickable
		}

		Flickable {
			id: flickable
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
				currentIndex: swipeBar.currentIndex
				clip: true

				Text {
					text: longDescription ? longDescription : qsTr("Description not available")

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
}
