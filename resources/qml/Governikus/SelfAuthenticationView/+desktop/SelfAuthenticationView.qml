/*
 * \copyright Copyright (c) 2019-2023 Governikus GmbH & Co. KG, Germany
 */
import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.Type.SettingsModel 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.TitleBar 1.0
import Governikus.Type.SelfAuthModel 1.0
import Governikus.View 1.0
import QtQuick 2.15

SectionPage {
	titleBarAction: TitleBarAction {
		helpTopic: "selfauthentication"
		//: LABEL DESKTOP
		text: qsTr("Identify")
	}

	Keys.onEnterPressed: startWorkflowButton.onClicked()
	Keys.onReturnPressed: startWorkflowButton.onClicked()

	Column {
		anchors.centerIn: parent
		spacing: Constants.pane_spacing
		width: Style.dimens.max_text_width

		Item {
			height: childrenRect.height
			width: parent.width

			Image {
				id: useNpa
				anchors.left: parent.left
				fillMode: Image.PreserveAspectFit
				height: width
				mipmap: true
				source: "qrc:///images/siteWithLogo.png"
				width: ApplicationModel.scaleFactor * 400
			}
			GText {
				//: LABEL DESKTOP A11y description of eID logo displayed next to the logo itself
				Accessible.name: qsTr("You can use your ID card anywhere you find the logo of the electronic identification function.")
				activeFocusOnTab: true
				anchors.left: useNpa.right
				anchors.leftMargin: Constants.component_spacing
				anchors.right: parent.right
				anchors.rightMargin: Constants.component_spacing
				anchors.verticalCenter: useNpa.verticalCenter

				//: LABEL DESKTOP
				text: qsTr("You can use your ID card anywhere you see this logo.")
				textStyle: Style.text.header

				FocusFrame {
				}
			}
		}
		GPane {
			id: textPane
			anchors {
				left: parent.left
				right: parent.right
			}
			GText {
				activeFocusOnTab: true
				//: LABEL DESKTOP
				text: qsTr("Use the button \"See my personal data\" to start the self-authentication service of the manufacturer of the %1 to display the data stored in the chip of your ID card.").arg(Qt.application.name)
				textStyle: Style.text.normal
				width: parent.width

				FocusFrame {
				}
			}
			PrivacyStatement {
				activeFocusOnTab: true
				textStyle: Style.text.normal
				width: parent.width

				FocusFrame {
				}
			}
			GButton {
				id: startWorkflowButton
				anchors.right: parent.right
				buttonColor: SettingsModel.useSelfauthenticationTestUri ? Constants.red : Style.color.button
				icon.source: "qrc:///images/identify.svg"
				//: LABEL DESKTOP
				text: qsTr("See my personal data")
				tintIcon: true

				onClicked: SelfAuthModel.startWorkflow()
			}
		}
	}
}
