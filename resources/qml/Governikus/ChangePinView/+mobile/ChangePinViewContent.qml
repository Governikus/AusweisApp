/*
 * \copyright Copyright (c) 2016-2020 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.12
import QtQuick.Layouts 1.12

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.Type.ChangePinModel 1.0
import Governikus.Type.NumberModel 1.0
import Governikus.Type.ApplicationModel 1.0

Item {
	id: root

	signal noPinAvailable()
	signal moreInformationRequested()

	ColumnLayout {
		id: upperColumn

		anchors.fill: parent
		anchors.margins: Constants.pane_padding

		spacing: 0

		TintableIcon {
			id: pinIcon

			Layout.alignment: Qt.AlignCenter
			Layout.maximumHeight: Style.dimens.header_icon_size
			Layout.fillWidth: true
			Layout.fillHeight: true
			Layout.margins: Constants.component_spacing

			source: "qrc:///images/material_lock.svg"
			sourceSize.height: Style.dimens.huge_icon_size
			tintColor: Style.color.accent
		}

		GText {
			id: pinDescWhatType

			Layout.alignment: Qt.AlignCenter
			Layout.fillWidth: true
			Layout.maximumWidth: Style.dimens.max_text_width

			//: LABEL ANDROID IOS
			text: qsTr("What kind of PIN do you have?")
			textStyle: Style.text.header_accent
			horizontalAlignment: Text.AlignHCenter
			wrapMode: Text.WordWrap
		}

		GText {
			id: pinDesc

			Layout.alignment: Qt.AlignCenter
			Layout.fillWidth: true
			Layout.maximumWidth: Style.dimens.max_text_width
			Layout.topMargin: Constants.text_spacing

			//: LABEL ANDROID IOS
			text: qsTr("Here you have the opportunity to set or change the PIN of your ID card.")
			textStyle: Style.text.normal_secondary
			horizontalAlignment: Text.AlignHCenter
			wrapMode: Text.WordWrap
		}

		GText {
			id: moreInformation

			Layout.alignment: Qt.AlignCenter
			Layout.fillWidth: true
			Layout.maximumWidth: Style.dimens.max_text_width
			Layout.topMargin: Constants.component_spacing

			text: qsTr("More information")

			horizontalAlignment: Text.AlignHCenter
			wrapMode: Text.WordWrap
			textStyle: Style.text.normal_accent
			font.underline: true

			MouseArea {
				anchors.fill: parent

				onClicked: root.moreInformationRequested()
			}
		}

		GSpacer { Layout.fillHeight: true }

		DynamicLayout {
			id: buttonLayout

			readonly property double buttonWidth: Math.max(sixDigitButton.implicitWidth, fiveDigitButton.implicitWidth, dontKnowButton.implicitWidth)

			Layout.alignment: Qt.AlignCenter
			Layout.topMargin: Constants.component_spacing

			spacing: Constants.component_spacing

			GButton {
				id: sixDigitButton

				Layout.minimumWidth: buttonLayout.buttonWidth

				//: LABEL ANDROID_TABLET IOS_TABLET
				text: qsTr("Six-digit PIN")
				onClicked: {
					NumberModel.requestTransportPin = false
					ChangePinModel.startWorkflow()
				}
			}

			GButton {
				id: fiveDigitButton

				Layout.minimumWidth: buttonLayout.buttonWidth

				//: LABEL ANDROID_TABLET IOS_TABLET
				text: qsTr("Five-digit Transport PIN")
				onClicked: {
					NumberModel.requestTransportPin = true
					ChangePinModel.startWorkflow()
				}
			}

			GButton {
				id: dontKnowButton

				Layout.minimumWidth: buttonLayout.buttonWidth

				//: LABEL ANDROID_TABLET IOS_TABLET
				text: qsTr("No PIN")
				onClicked: root.noPinAvailable()
			}
		}
	}
}
