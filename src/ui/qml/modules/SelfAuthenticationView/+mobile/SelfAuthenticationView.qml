/**
 * Copyright (c) 2017-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Layouts
import Governikus.AuthView
import Governikus.Global
import Governikus.Style
import Governikus.TitleBar
import Governikus.View
import Governikus.Type

FlickableSectionPage {
	id: root

	property bool autoInsertCard: false
	property bool hideTechnologySwitch: false
	property var initialPlugin

	signal back
	signal workflowFinished(int pModuleToShow)

	//: LABEL ANDROID IOS
	title: qsTr("Identify")

	navigationAction: NavigationAction {
		action: NavigationAction.Action.Back

		onClicked: root.back()
	}

	FadeInAnimation {
		target: root
	}
	Component {
		id: authView

		AuthView {
			property bool backRequested: false

			autoInsertCard: root.autoInsertCard
			hideTechnologySwitch: root.hideTechnologySwitch
			initialPlugin: root.initialPlugin

			Component.onCompleted: SelfAuthModel.startWorkflow(false)
			onRequestBack: backRequested = true
			onShowChangePinView: root.workflowFinished(UiModule.PINMANAGEMENT)
			onWorkflowFinished: root.workflowFinished(backRequested ? UiModule.SELF_AUTHENTICATION : UiModule.DEFAULT)
		}
	}
	ColumnLayout {
		Layout.leftMargin: Constants.pane_padding
		Layout.rightMargin: Constants.pane_padding
		spacing: Constants.component_spacing

		TintableIcon {
			Layout.alignment: Qt.AlignHCenter
			source: "qrc:///images/mobile/mydata.svg"
			sourceSize.height: Style.dimens.header_icon_size
			tintColor: Style.color.image

			PkiSwitch {
				anchors.fill: parent
				//: LABEL ANDROID IOS
				functionName: qsTr("Self-authentication")
			}
		}
		GText {
			Layout.alignment: Qt.AlignHCenter
			//: LABEL ANDROID IOS
			text: qsTr("View the data stored on your ID card")
			textStyle: Style.text.subline
			wrapMode: Text.WordWrap
		}
		PrivacyStatement {
			activeFocusOnTab: true

			FocusFrame {
			}
		}
	}
	GSpacer {
		Layout.fillHeight: true
		Layout.minimumHeight: Constants.component_spacing
	}
	Hint {
		Layout.fillWidth: true
		//: LABEL ANDROID IOS
		title: qsTr("Hint")

		RowLayout {
			anchors.left: parent.left
			anchors.right: parent.right
			spacing: Constants.groupbox_spacing

			Image {
				fillMode: Image.PreserveAspectFit
				source: "qrc:///images/siteWithLogo_%1.svg".arg(Style.currentTheme.name)
				sourceSize.width: Math.min(Style.dimens.header_icon_size, (parent.width - parent.spacing) / 3)
			}
			GText {
				//: LABEL ANDROID IOS A11y description of eID logo displayed next to the logo itself
				Accessible.name: qsTr("You can use your ID card anywhere you find the logo of the electronic identification function.")
				Layout.minimumWidth: 80

				//: LABEL ANDROID IOS
				text: qsTr("You can use your ID card anywhere you see this logo.")
			}
		}
	}
	GSpacer {
		Layout.fillHeight: true
		Layout.minimumHeight: Constants.component_spacing
	}
	GButton {
		Layout.alignment: Qt.AlignHCenter
		buttonColor: SettingsModel.useSelfauthenticationTestUri ? Style.color.error : Style.color.control.background.basic
		icon.source: "qrc:///images/identify.svg"
		//: LABEL ANDROID IOS
		text: qsTr("See my personal data")
		tintIcon: true

		onClicked: push(authView)
		onFocusChanged: if (focus)
			root.positionViewAtItem(this)
	}
}
