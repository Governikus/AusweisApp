/*
 * \copyright Copyright (c) 2019-2020 Governikus GmbH & Co. KG, Germany
 */

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.Type.SettingsModel 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.TitleBar 1.0
import Governikus.Type.SelfAuthModel 1.0
import Governikus.View 1.0
import QtQuick 2.10

SectionPage {

	titleBarAction: TitleBarAction {
		//: LABEL DESKTOP_QML
		text: qsTr("Identify") + SettingsModel.translationTrigger
		helpTopic: "selfauthentication"
	}

	//: LABEL DESKTOP_QML
	Accessible.name: qsTr("Self-Authentication.") + SettingsModel.translationTrigger
	Accessible.description: qsTr("This is the self-authentication view of the AusweisApp2.") + SettingsModel.translationTrigger
	Keys.onReturnPressed: startWorkflowButton.onClicked()
	Keys.onEnterPressed: startWorkflowButton.onClicked()

	Column {
		width: Style.dimens.max_text_width
		anchors.centerIn: parent

		spacing: Constants.pane_spacing
		Item {
			height: childrenRect.height
			width: parent.width

			Image {
				id: useNpa

				width: ApplicationModel.scaleFactor * 400
				anchors.left: parent.left

				fillMode: Image.PreserveAspectFit
				source: "qrc:///images/siteWithLogo.png"
				mipmap: true
			}

			GText {
				anchors.verticalCenter: useNpa.verticalCenter
				anchors.leftMargin: Constants.component_spacing
				anchors.rightMargin: Constants.component_spacing
				anchors.left: useNpa.right
				anchors.right: parent.right

				activeFocusOnTab: true

				textStyle: Style.text.header_inverse

				//: LABEL DESKTOP_QML
				text: qsTr("You can use your ID card anywhere you see this logo.") + SettingsModel.translationTrigger

				FocusFrame {}
			}
		}
		Pane {
			id: textPane

			anchors {
				left: parent.left
				right: parent.right
			}

			GText {
				id: info

				width: parent.width

				activeFocusOnTab: true

				textStyle: Style.text.normal
				//: LABEL DESKTOP_QML
				text: qsTr("Use the button 'See my personal data' to display the data stored on your ID card."
							+ " An Internet connection is required to display the data.")
							+ "<br><br><b>"
							//: LABEL DESKTOP_QML
							+ qsTr("Your personal data is neither saved nor processed in any way. Please see our %1 for details on how your personal data is processed.")
								.arg('<a href="' +
								//: LABEL DESKTOP_QML
								qsTr("https://www.ausweisapp.bund.de/en/privacy/") + '">' +
								//: LABEL DESKTOP_QML
								qsTr("data privacy statement") + '</a>')
							+ "</b>"
							+ SettingsModel.translationTrigger

				FocusFrame {
					borderColor: Style.color.focus_indicator
				}
			}

			GButton {
				id: startWorkflowButton

				anchors.right: parent.right

				icon.source: "qrc:///images/npa.svg"
				buttonColor: SettingsModel.useSelfauthenticationTestUri ? Constants.red : Style.color.button
				//: LABEL DESKTOP_QML
				text: qsTr("See my personal data") + SettingsModel.translationTrigger
				onClicked: SelfAuthModel.startWorkflow()
			}
		}
	}

}
