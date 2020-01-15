/*
 * \copyright Copyright (c) 2015-2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.TitleBar 1.0
import Governikus.View 1.0
import Governikus.Type.SettingsModel 1.0
import Governikus.Type.LogModel 1.0


SectionPage {
	id: baseItem
	navigationAction: NavigationAction { state: "cancel"; onClicked: baseItem.clicked() }

	enum Type {
		IsSuccess,
		IsError,
		IsInfo
	}

	property alias text: resultText.text
	property alias buttonText: buttonLeft.text
	property alias showMailButton: buttonRight.visible
	property int resultType: Type.IsSuccess
	signal clicked

	Rectangle {
		anchors.fill: parent
		color: Style.color.background
	}

	StatusIcon {
		id: resultIcon
		height: 100
		anchors.horizontalCenter: parent.horizontalCenter
		anchors.top: parent.top
		anchors.topMargin: 60
		source: {
			switch (resultType) {
				case ResultView.Type.IsSuccess:
					return "qrc:///images/status_ok.svg"
				case ResultView.Type.IsInfo:
					return "qrc:///images/status_info.svg"
				case ResultView.Type.IsError:
					return "qrc:///images/status_error.svg"
			}
		}
	}

	GText {
		id: resultText
		anchors.horizontalCenter: parent.horizontalCenter
		anchors.top: resultIcon.bottom
		anchors.bottom: buttonRow.top
		width: parent.width - (2 * Constants.pane_padding)

		horizontalAlignment: Text.AlignHCenter
		verticalAlignment: Text.AlignVCenter
		textStyle: resultType !== ResultView.Type.IsError ? Style.text.header_accent : Style.text.header_warning
	}

	Row {
		id: buttonRow

		anchors.bottom: parent.bottom
		anchors.horizontalCenter: parent.horizontalCenter
		anchors.bottomMargin: 30
		spacing: Constants.component_spacing

		GButton {
			id: buttonLeft

			//: LABEL ANDROID IOS
			text: qsTr("OK") + SettingsModel.translationTrigger
			onClicked: baseItem.clicked()
		}

		GButton {
			id: buttonRight
			visible: false

			//: LABEL ANDROID IOS
			text: qsTr("Send logfile") + SettingsModel.translationTrigger
			onClicked: LogModel.mailLog()
		}
	}
}
