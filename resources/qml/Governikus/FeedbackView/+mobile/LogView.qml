/*
 * \copyright Copyright (c) 2018-2021 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.TitleBar 1.0
import Governikus.View 1.0
import Governikus.Type.LogModel 1.0


SectionPage
{
	id: root
	navigationAction: NavigationAction { state: "back"; onClicked: firePop() }
	//: LABEL ANDROID IOS
	title: qsTr("Log")
	rightTitleBarAction: LogTitleBarControls {
		allowRemove: comboBox.currentIndex > 0
		allowRemoveAll: comboBox.model.length > 1 // comboBox.count doesn't seem to update reliably

		onShare: LogModel.shareLog(popupPosition)

		onRemove: {
			confirmationPopup.deleteAll = false
			confirmationPopup.open()
		}

		onRemoveAll: {
			confirmationPopup.deleteAll = true
			confirmationPopup.open()
		}
	}

	sectionPageFlickable: logView

	Rectangle {
		id: logSelector

		height: comboBox.height + Constants.pane_padding
		width: parent.width
		z: 1

		color: Style.color.accent

		GComboBox {
			id: comboBox

			anchors {
				right: parent.right
				left: parent.left
				margins: Constants.text_spacing
				verticalCenter: parent.verticalCenter
			}

			Accessible.description: qsTr("Select log from list.")

			model: LogModel.logFiles
			onCurrentIndexChanged: LogModel.setLogfile(currentIndex)
		}
	}

	GListView {
		id: logView

		Connections {
			target: LogModel
			onFireNewLogMsg: if (logView.atYEnd) logView.positionViewAtEnd()
		}

		anchors {
			top: logSelector.bottom
			left: parent.left
			right: parent.right
			bottom: parent.bottom
		}

		model: LogModel
		delegate: ListItem {
			readonly property bool isLastItem: index === ListView.view.count - 1

			Accessible.onScrollDownAction: ListView.view.scrollPageDown()
			Accessible.onScrollUpAction: ListView.view.scrollPageUp()

			showLinkIcon: false
			showSeparator: !isLastItem
			fixedHeight: false
			mouseAreaEnabled: false
			headerText: origin
			text: message
		}
	}

	ConfirmationPopup {
		id: confirmationPopup

		property bool deleteAll: true

		title: (deleteAll ?
				//: LABEL ANDROID IOS
				qsTr("Delete all logs") :
				//: LABEL ANDROID IOS
				qsTr("Delete log")
			   )
		//: INFO ANDROID IOS All logfiles are about to be removed, user confirmation required.
		text: (deleteAll ? qsTr("All old logs will be deleted.")
						 //: INFO ANDROID IOS The current logfile is about to be removed, user confirmation required.
						 : qsTr("The log will be deleted.")
			   )
		//: LABEL ANDROID IOS
		okButtonText: qsTr("Delete")
		onConfirmed: deleteAll ? LogModel.removeOtherLogfiles() : LogModel.removeCurrentLogfile()
	}
}
