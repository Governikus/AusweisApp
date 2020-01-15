/*
 * \copyright Copyright (c) 2018-2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.10
import QtQuick.Layouts 1.3
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
	property int resultType: ResultView.Type.IsSuccess
	property alias buttonType: button.buttonType

	Accessible.name: qsTr("Result view") + SettingsModel.translationTrigger
	Accessible.description: qsTr("This is the result of an authentication.") + SettingsModel.translationTrigger
	Keys.onReturnPressed: button.onClicked()
	Keys.onEnterPressed: button.onClicked()
	Keys.onEscapePressed: button.onClicked()

	ColumnLayout {
		anchors.fill: parent

		StatusIcon {
			Layout.alignment: Qt.AlignHCenter
			Layout.preferredWidth: Style.dimens.status_icon_large
			Layout.preferredHeight: Style.dimens.status_icon_large

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
			Layout.alignment: Qt.AlignHCenter
			Layout.maximumWidth: Style.dimens.max_text_width

			activeFocusOnTab: true
			Accessible.name: resultText.text

			horizontalAlignment: Text.AlignHCenter
			verticalAlignment: Text.AlignVCenter
			textStyle: Style.text.title_inverse

			FocusFrame {}
		}

		GButton {
			id: emailButton

			visible: false
			Layout.alignment: Qt.AlignHCenter

			icon.source: "qrc:///images/provider/mail.svg"
			//: LABEL DESKTOP_QML
			text: qsTr("Send email") + SettingsModel.translationTrigger
			onClicked: baseItem.emailButtonPressed()
		}

		NavigationButton {
			id: button

			Layout.alignment: Qt.AlignHCenter
			Layout.preferredWidth: width
			Layout.preferredHeight: height

			activeFocusOnTab: true

			buttonType: NavigationButton.Type.Forward
			onClicked: baseItem.nextView(SectionPage.Views.Main)
		}
	}
}
