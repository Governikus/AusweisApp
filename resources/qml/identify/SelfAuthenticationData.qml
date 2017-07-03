import QtQuick 2.5

import "../"
import "../global"

SectionPage {
	id: root
	leftTitleBarAction: TitleBarMenuAction { state: "cancel"; onClicked: root.close() }
	headerTitleBarAction: TitleBarAction { text: qsTr("Identify"); font.bold: true }

	function close() {
		selfAuthenticationModel.continueWorkflow()
		pop(null)
		navBar.lockedAndHidden = false
	}

	Item {
		anchors.fill: parent

		Item {
			id: message
			height: Utils.dp(120)
			width: resultImage.width + successText.width
			anchors.horizontalCenter: parent.horizontalCenter
			anchors.top: parent.top

			Image {
				id: resultImage
				anchors.left: parent.left
				anchors.verticalCenter: parent.verticalCenter
				height: Utils.dp(100)
				width: height
				fillMode: Image.PreserveAspectFit
				source: "qrc:///images/iOS/gruener_Haken.svg"
			}

			Text {
				id: successText
				anchors.left: resultImage.right
				anchors.verticalCenter: resultImage.verticalCenter
				text: qsTr("Successfull reading data")
				font.pixelSize: PlatformConstants.is_tablet ? Constants.header_font_size : Constants.normal_font_size
				color: Constants.blue
			}
		}

		Pane {
			id: pane
			anchors.top: message.bottom
			anchors.bottom: okButton.top
			anchors.bottomMargin: Utils.dp(30)

			Flickable {
				id: paneflick
				height: pane.height - 2 * pane.margin
				width: pane.width - 2 * pane.margin
				contentWidth: grid.width
				contentHeight: grid.height

				Grid {
					id: grid
					columns: PlatformConstants.is_tablet ? 3 : 1
					spacing: Utils.dp(15)
					verticalItemAlignment: Grid.AlignBottom
					Repeater {
						model: selfAuthenticationModel

						LabeledText {
							label: name
							text: value === "" ? "---" : value
							width: (pane.width - 2 * pane.margin - (grid.columns - 1) * grid.spacing) / grid.columns
						}
					}
				}
			}
		}

		Image {
			source: "qrc:///images/android/android_arrow_back.svg"
			height: Utils.dp(60)
			width: Utils.dp(60)
			anchors.horizontalCenter: pane.horizontalCenter
			anchors.verticalCenter: pane.top
			visible: paneflick.atYBeginning && !paneflick.atYEnd && !paneflick.moving
			rotation: 90
			fillMode: Image.PreserveAspectFit
		}

		Button {
			id: okButton
			anchors.horizontalCenter: parent.horizontalCenter
			anchors.bottom: parent.bottom
			anchors.bottomMargin: Utils.dp(30)
			text: qsTr("Ok")
			onClicked: root.close()
		}
	}
}
