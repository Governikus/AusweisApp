/*
 * \copyright Copyright (c) 2018-2019 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10
import QtQuick.Controls 2.3

import Governikus.Global 1.0
import Governikus.TitleBar 1.0
import Governikus.Style 1.0
import Governikus.View 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.Type.SettingsModel 1.0


SectionPage {
	id: baseItem

	enum Type {
		IsSuccess,
		IsError,
		IsInfo
	}

	signal emailButtonPressed()
	property alias emailButtonVisible: emailButton.visible
	property alias text: resultText.text
	property int resultType: Type.IsSuccess

	Accessible.name: qsTr("Result view") + SettingsModel.translationTrigger
	Accessible.description: qsTr("This is the result of an authentication.") + SettingsModel.translationTrigger
	Keys.onReturnPressed: button.onClicked()
	Keys.onEnterPressed: button.onClicked()
	Keys.onEscapePressed: button.onClicked()

	StatusIcon {
		height: Style.dimens.status_icon_large
		anchors.horizontalCenter: parent.horizontalCenter
		anchors.verticalCenter: parent.top
		anchors.verticalCenterOffset: baseItem.height / 4

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

		visible: text !== ""
		width: Math.min(parent.width - (2 * Constants.pane_padding), Style.dimens.max_text_width)
		anchors.horizontalCenter: parent.horizontalCenter
		anchors.top: parent.verticalCenter

		activeFocusOnTab: true
		Accessible.role: Accessible.Heading
		Accessible.name: resultText.text

		horizontalAlignment: Text.AlignHCenter
		verticalAlignment: Text.AlignVCenter
		textStyle: Style.text.title

		onLinkActivated: Qt.openUrlExternally(link)

		FocusFrame {
			dynamic: false
		}
	}

	GButton {
		id: emailButton

		visible: false

		icon.source: "qrc:///images/provider/mail.svg"
		//: LABEL DESKTOP_QML
		text: qsTr("Send email") + SettingsModel.translationTrigger
		anchors {
			horizontalCenter: parent.horizontalCenter
			verticalCenter: parent.top
			verticalCenterOffset: baseItem.height * 3 / 4
		}
		onClicked: baseItem.emailButtonPressed()
	}

	NavigationButton {
		id: button

		anchors {
			margins: Constants.component_spacing
			bottom: parent.bottom
			horizontalCenter: parent.horizontalCenter
		}

		activeFocusOnTab: true

		buttonType: Qt.ForwardButton
		onClicked: baseItem.nextView(SectionPage.Views.Main)
	}
}
