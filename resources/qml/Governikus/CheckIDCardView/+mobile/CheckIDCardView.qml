/**
 * Copyright (c) 2020-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Governikus.Global
import Governikus.SelfAuthenticationView
import Governikus.Style
import Governikus.TitleBar
import Governikus.View
import Governikus.Type.ApplicationModel
import Governikus.Type.CheckIDCardModel
import Governikus.Type.ReaderPlugIn
import Governikus.Type.UiModule

FlickableSectionPage {
	id: root

	signal startAuth

	spacing: Constants.component_spacing

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

	Component {
		id: selfAuthView

		SelfAuthenticationView {
			hideTechnologySwitch: true
			initialPlugIn: ReaderPlugIn.NFC

			onBack: {
				setLockedAndHidden(false);
				pop();
				show(UiModule.DEFAULT);
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
	TintableIcon {
		Layout.alignment: Qt.AlignHCenter
		source: "qrc:///images/mobile/device.svg"
		sourceSize.height: Style.dimens.header_icon_size
		tintColor: Style.color.control
	}
	GText {
		Layout.alignment: Qt.AlignHCenter
		Layout.margins: Constants.pane_padding
		horizontalAlignment: Text.AlignHCenter
		//: LABEL ANDROID IOS
		text: qsTr("To use the eID function, your device must meet certain technical requirements. Furthermore, the eID function must be activated.")
	}
	GText {
		Layout.alignment: Qt.AlignHCenter
		Layout.margins: Constants.pane_padding
		horizontalAlignment: Text.AlignHCenter
		//: LABEL ANDROID IOS
		text: qsTr("Check if your smartphone and ID card are ready for use.")
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
			if (ApplicationModel.nfcState === ApplicationModel.NFC_UNAVAILABLE) {
				setLockedAndHidden();
				push(checkIDCardResultView, {
						"result": CheckIDCardModel.NO_NFC
					});
			} else {
				d.startCheck();
			}
		}
	}
}
