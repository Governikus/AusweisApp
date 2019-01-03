import QtQuick 2.10
import QtQuick.Controls 2.3

import Governikus.Global 1.0
import Governikus.TitleBar 1.0
import Governikus.View 1.0
import Governikus.Type.LogModel 1.0


SectionPage
{
	id: root
	leftTitleBarAction: TitleBarAction { state: "back"; onClicked: firePop() }
	headerTitleBarAction: TitleBarAction { text: qsTr("Log") + settingsModel.translationTrigger; font.bold: true }
	rightTitleBarAction: LogTitleBarControls {
		allowRemove: comboBox.currentIndex > 0

		onShare: {
			LogModel.shareLog()
		}

		onRemove: {
			confirmationPopup.deleteAll = false
			confirmationPopup.open()
		}

		onRemoveAll: {
			confirmationPopup.deleteAll = true
			confirmationPopup.open()
		}
	}

	Item {
		id: logSelector
		height: comboBox.height
		width: parent.width - 2 * Constants.pane_padding
		anchors.top: parent.top
		anchors.topMargin: Constants.pane_padding
		anchors.horizontalCenter: parent.horizontalCenter

		Text {
			id: comboText
			anchors.left: parent.left
			anchors.verticalCenter: parent.verticalCenter

			color: Constants.black
			font.pixelSize: Constants.normal_font_size
			text: qsTr("Log:") + settingsModel.translationTrigger
		}

		GComboBox {
			id: comboBox
			width: parent.width - comboText.width - Constants.component_spacing
			anchors.right: parent.right
			model: LogModel.logFiles
			onActivated: LogModel.setLogfile(comboBox.currentIndex)
		}
	}

	Pane {
		id: pane
		anchors.top: logSelector.bottom
		anchors.bottom: parent.bottom
		anchors.margins: Constants.pane_padding

		Item {
			height: pane.height - 2 * Constants.pane_padding
			width: pane.width - 2 * Constants.pane_padding

			ListView {
				id: logView
				clip: true
				model: LogModel
				anchors.fill: parent
				maximumFlickVelocity: Constants.scrolling_speed

				delegate: Text {
					width: pane.width - 2* Constants.pane_padding
					wrapMode: Text.Wrap
					font.pixelSize: Constants.small_font_size
					font.bold: !(index % 2)
					text: model.display
					textFormat: Text.PlainText
				}

				onAtYEndChanged: LogModel.autoFlick = atYEnd

				onContentYChanged: {
					if (visibleArea.yPosition < 0.1) {
						LogModel.moveView(-20)
						return
					}

					if (visibleArea.yPosition + visibleArea.heightRatio > 0.9) {
						LogModel.moveView(20)
					}
				}

				Connections {
					target: LogModel
					onFireNewLogMsg: logView.positionViewAtEnd()
				}
			}

			ScrollBar {
				id: mainScroll
				width: Utils.dp(8)
				anchors.top: parent.top
				anchors.bottom: parent.bottom
				anchors.horizontalCenter: parent.right
				anchors.horizontalCenterOffset: Constants.pane_padding / 2

				active: true
				orientation: Qt.Vertical
				minimumSize: 2 * (width / height)
				size: LogModel.heightRatio * logView.visibleArea.heightRatio
				position: LogModel.yPosition + LogModel.heightRatio * logView.visibleArea.yPosition
				onPressedChanged: {
					if (pressed) {
						logView.cancelFlick()
					} else {
						active = true
						LogModel.yPosition = position
					}
				}
			}
		}
	}

	ConfirmationPopup {
		id: confirmationPopup

		property bool deleteAll: true

		baseItem: root
		title: (deleteAll ? qsTr("Delete all") : qsTr("Delete")) + settingsModel.translationTrigger
		text: (deleteAll ? qsTr("Please confirm that you want to delete your old logfiles.")
						 : qsTr("Please confirm that you want to delete the logfile.")
			   ) + settingsModel.translationTrigger
		confirmText: qsTr("Delete") + settingsModel.translationTrigger
		onConfirmed: deleteAll ? LogModel.removeOtherLogfiles() : LogModel.removeCurrentLogfile()
	}
}
