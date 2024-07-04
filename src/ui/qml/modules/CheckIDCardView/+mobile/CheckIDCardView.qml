/**
 * Copyright (c) 2020-2024 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Governikus.Global
import Governikus.SelfAuthenticationView
import Governikus.Style
import Governikus.TitleBar
import Governikus.View
import Governikus.Type

FlickableSectionPage {
	id: root

	signal startAuth

	//: LABEL ANDROID IOS
	title: qsTr("Check device and ID card")

	navigationAction: NavigationAction {
		action: NavigationAction.Action.Back

		onClicked: show(UiModule.DEFAULT)
	}

	onStartAuth: {
		popAll();
		push(selfAuthView);
	}

	FadeInAnimation {
		target: root
	}
	Component {
		id: selfAuthView

		SelfAuthenticationView {
			hideTechnologySwitch: true
			initialPlugin: ReaderManagerPluginType.NFC

			onBack: {
				setLockedAndHidden(false);
				pop();
				show(UiModule.DEFAULT);
			}
			onWorkflowFinished: pModuleToShow => {
				show(pModuleToShow);
				popAll();
			}
		}
	}
	QtObject {
		id: d

		function cancel() {
			setLockedAndHidden(false);
			popAll();
		}
		function restartCheck() {
			popAll();
			startCheck();
		}
		function startCheck() {
			setLockedAndHidden();
			push(checkIDCardWorkflow);
		}
	}
	Component {
		id: checkIDCardResultView

		CheckIDCardResultView {
			title: root.title

			onCancelClicked: d.cancel()
			onRestartCheck: d.restartCheck()
			onStartAuth: root.startAuth()
		}
	}
	Component {
		id: checkIDCardWorkflow

		CheckIDCardWorkflow {
			onCancel: d.cancel()
			onRestartCheck: d.restartCheck()
			onStartAuth: root.startAuth()
		}
	}
	ColumnLayout {
		Layout.leftMargin: Constants.pane_padding
		Layout.rightMargin: Constants.pane_padding
		spacing: Constants.component_spacing

		TintableIcon {
			Layout.alignment: Qt.AlignHCenter
			source: "qrc:///images/mobile/device.svg"
			sourceSize.height: Style.dimens.header_icon_size
			tintColor: Style.color.image
		}
		GText {
			Layout.alignment: Qt.AlignHCenter
			//: LABEL ANDROID IOS
			text: qsTr("Check if your device & ID card are ready for use")
			textStyle: Style.text.subline
		}
		GText {
			Layout.alignment: Qt.AlignHCenter
			//: LABEL ANDROID IOS
			text: qsTr("To use the eID function, your device must meet certain technical requirements. Furthermore, the eID function must be activated.")
		}
		GSpacer {
			Layout.fillHeight: true
		}
		GButton {
			Layout.alignment: Qt.AlignHCenter
			icon.source: "qrc:///images/mobile/device_button.svg"

			//: LABEL ANDROID IOS
			text: qsTr("Start check")
			tintIcon: true

			onClicked: {
				if (ApplicationModel.nfcState === ApplicationModel.NfcState.UNAVAILABLE) {
					setLockedAndHidden();
					push(checkIDCardResultView, {
						"result": CheckIDCardModel.Result.NO_NFC
					});
				} else {
					d.startCheck();
				}
			}
			onFocusChanged: if (focus)
				root.positionViewAtItem(this)
		}
	}
}
