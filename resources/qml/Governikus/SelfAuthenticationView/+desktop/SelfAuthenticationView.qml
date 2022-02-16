/*
 * \copyright Copyright (c) 2019-2022 Governikus GmbH & Co. KG, Germany
 */

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.Type.SettingsModel 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.TitleBar 1.0
import Governikus.Type.SelfAuthModel 1.0
import Governikus.View 1.0
import QtQuick 2.12

SectionPage {

	titleBarAction: TitleBarAction {
		//: LABEL DESKTOP
		text: qsTr("Identify")
		helpTopic: "selfauthentication"
	}

	//: LABEL DESKTOP
	Accessible.name: qsTr("Self-authentication.")
	Accessible.description: qsTr("This is the self-authentication view of the AusweisApp2.")
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

				//: LABEL DESKTOP
				text: qsTr("You can use your ID card anywhere you see this logo.")

				FocusFrame {}
			}
		}
		GPane {
			id: textPane

			anchors {
				left: parent.left
				right: parent.right
			}

			GText {
				width: parent.width

				activeFocusOnTab: true

				textStyle: Style.text.normal
				//: LABEL DESKTOP
				text: qsTr("Use the button \"See my personal data\" to start the self-authentication service of the manufacturer of the %1 to display the data stored in the chip of your ID card.")
					.arg(Qt.application.name)

				FocusFrame {
					borderColor: Style.color.focus_indicator
				}
			}

			GText {
				readonly property string privacyStatementUrl: "https://www.ausweisapp.bund.de/%1/aa2/privacy".arg(SettingsModel.language)
				//: LABEL DESKTOP Text of the html link inside of a sentence
				readonly property string privacyStatementDescription: qsTr("data privacy statement")
				readonly property string privacyStatementLink: "<a href=\"%1\">%2</a>".arg(privacyStatementUrl).arg(privacyStatementDescription)

				width: parent.width

				activeFocusOnTab: true

				textStyle: Style.text.normal
				font.bold: true
				//: LABEL DESKTOP
				text: qsTr("Your personal data is neither saved nor processed in any way. Please see our %1 for details on how your personal data is processed.")
						.arg(privacyStatementLink)

				FocusFrame {
					borderColor: Style.color.focus_indicator
				}
			}

			GButton {
				id: startWorkflowButton

				anchors.right: parent.right

				icon.source: "qrc:///images/identify.svg"
				tintIcon: true
				buttonColor: SettingsModel.useSelfauthenticationTestUri ? Constants.red : Style.color.button
				//: LABEL DESKTOP
				text: qsTr("See my personal data")
				onClicked: SelfAuthModel.startWorkflow()
			}
		}
	}

}
