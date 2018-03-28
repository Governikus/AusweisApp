import QtQuick 2.7

import Governikus.Global 1.0

MouseArea {
	property string text: ""
	property alias font: titleBarText.font

	height: Constants.titlebar_height
	width: titleBarText.width
	anchors.centerIn: parent

	TitleBarText {
		id: titleBarText
		anchors.centerIn: parent

		text: (	parent.text !== "" ? parent.text :
				parent.state === "cancel" ? qsTr("Cancel") :
				parent.state === "edit" ? qsTr("Edit") :
				parent.state === "back" ? qsTr("< back") :
				parent.state === "hidden" ? "" :
				"") + settingsModel.translationTrigger
	}
}
