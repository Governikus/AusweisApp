/*
 * \copyright Copyright (c) 2017-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.15
import QtQuick.Layouts 1.15

import Governikus.Global 1.0
import Governikus.Style 1.0
import Governikus.TitleBar 1.0
import Governikus.View 1.0
import Governikus.Type.ApplicationModel 1.0
import Governikus.Type.SettingsModel 1.0
import Governikus.Type.SelfAuthModel 1.0
import Governikus.Type.UiModule 1.0

SectionPage {
	id: root

	navigationAction: NavigationAction {
		action: NavigationAction.Action.Back
		onClicked: show(UiModule.DEFAULT)
	}
	//: LABEL ANDROID IOS
	title: qsTr("Identify")

	sectionPageFlickable: contentItem
	GFlickableColumnLayout {
		id: contentItem

		readonly property var minIconHeight: Style.dimens.medium_icon_size
		readonly property var maxIconHeight: Style.dimens.header_icon_size

		anchors.fill: parent

		minimumContentHeight: implicitContentHeight - (maxIconHeight - minIconHeight)
		maximumContentWidth: Style.dimens.max_text_width
		spacing: 0

		TintableIcon {
			Layout.alignment: Qt.AlignHCenter
			Layout.fillHeight: true
			Layout.maximumHeight: contentItem.maxIconHeight
			Layout.minimumHeight: contentItem.minIconHeight

			source: "qrc:///images/mydata.svg"
			sourceSize.height: contentItem.maxIconHeight
			tintColor: Style.color.accent

			PkiSwitch {
				anchors.fill: parent
			}
		}

		GPane {
			Layout.alignment: Qt.AlignHCenter
			Layout.fillWidth: true
			Layout.topMargin: Constants.component_spacing

			GText {
				width: parent.width

				wrapMode: Text.WordWrap
				//: LABEL ANDROID IOS
				text: qsTr("Use the button \"See my personal data\" to start the self-authentication service of the manufacturer of the %1 to display the data stored in the chip of your ID card.")
					.arg(Qt.application.name)
			}

			PrivacyStatement {
				width: parent.width
			}
		}

		GSpacer { Layout.fillHeight: true }

		Hint {
			Layout.fillWidth: true
			Layout.topMargin: Constants.component_spacing

			RowLayout {
				width: parent.width
				spacing: Constants.text_spacing

				Image {
					Layout.fillWidth: true
					Layout.preferredHeight: Style.dimens.medium_icon_size

					source: "qrc:///images/siteWithLogo.png"
					sourceSize.height: Style.dimens.large_icon_size
					fillMode: Image.PreserveAspectFit
					asynchronous: true
				}

				GText {
					Layout.fillWidth: true

					//: LABEL ANDROID IOS A11y description of eID logo displayed next to the logo itself
					Accessible.name: qsTr("You can use your ID card anywhere you find the logo of the electronic identification function.")

					//: LABEL ANDROID IOS
					text: qsTr("You can use your ID card anywhere you see this logo.")
				}
			}
		}

		GButton {
			Layout.alignment: Qt.AlignHCenter
			Layout.topMargin: Constants.component_spacing

			icon.source: "qrc:///images/identify.svg"
			buttonColor: SettingsModel.useSelfauthenticationTestUri ? Style.color.warning_text : Style.color.accent
			//: LABEL ANDROID IOS
			text: qsTr("See my personal data")
			tintIcon: true

			enabled: ApplicationModel.currentWorkflow !== ApplicationModel.WORKFLOW_AUTHENTICATION
			onClicked: SelfAuthModel.startWorkflow()
		}
	}
}
