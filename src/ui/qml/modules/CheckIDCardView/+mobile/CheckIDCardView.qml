/**
 * Copyright (c) 2020-2025 Governikus GmbH & Co. KG, Germany
 */

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts

import Governikus.Global
import Governikus.RemoteServiceView
import Governikus.SelfAuthenticationView
import Governikus.Style
import Governikus.TitleBar
import Governikus.Type
import Governikus.View

FlickableSectionPage {
	id: root

	readonly property int detectedReaderType: {
		if (SettingsModel.preferredTechnology === ReaderManagerPluginType.SIMULATOR) {
			return ApplicationModel.nfcState === ApplicationModel.NfcState.UNAVAILABLE ? ReaderManagerPluginType.REMOTE_IFD : ReaderManagerPluginType.NFC;
		}
		return SettingsModel.preferredTechnology;
	}
	readonly property int readerType: selectedReaderType !== ReaderManagerPluginType.UNKNOWN ? selectedReaderType : detectedReaderType
	property int selectedReaderType: ReaderManagerPluginType.UNKNOWN

	signal changeTransportPin
	signal checkSuccess

	//: LABEL ANDROID IOS
	title: qsTr("Check device and ID card")

	navigationAction: NavigationAction {
		action: NavigationAction.Action.Back

		onClicked: root.show(UiModule.DEFAULT)
	}

	Component.onDestruction: d.stopScan()
	onCheckSuccess: {
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
			initialPlugin: root.readerType

			onBack: {
				setLockedAndHidden(false);
				pop();
				show(UiModule.DEFAULT);
			}
			onChangeTransportPin: root.changeTransportPin()
			onWorkflowFinished: pModuleToShow => {
				show(pModuleToShow);
				popAll();
			}
		}
	}
	QtObject {
		id: d

		function cancel() {
			stopScan();
			root.setLockedAndHidden(false);
			root.popAll();
		}
		function startCheck() {
			if (root.readerType === ReaderManagerPluginType.REMOTE_IFD) {
				RemoteServiceModel.startDetection();
			}
			root.setLockedAndHidden();
			root.push(checkIDCardWorkflow);
		}
		function stopScan() {
			if (root.readerType === ReaderManagerPluginType.REMOTE_IFD) {
				RemoteServiceModel.stopDetection(true);
			}
		}
	}
	Component {
		id: noNfcSuggestion

		CheckIDCardNoNfcSuggestion {
			title: root.title

			navigationAction: NavigationAction {
				action: NavigationAction.Action.Back

				onClicked: root.pop()
			}

			onConnectSmartphone: root.push(remoteServiceSettings)
		}
	}
	Component {
		id: remoteServiceSettings

		RemoteServiceSettings {
			allowUsage: true

			Component.onCompleted: RemoteServiceModel.startDetection()
			Component.onDestruction: {
				if (root.readerType !== ReaderManagerPluginType.REMOTE_IFD) {
					RemoteServiceModel.stopDetection(true);
				}
			}
			onPairedDeviceFound: {
				root.selectedReaderType = ReaderManagerPluginType.REMOTE_IFD;
				d.startCheck();
			}
		}
	}
	Component {
		id: checkIDCardWorkflow

		CheckIDCardWorkflow {
			readerType: root.readerType

			Component.onDestruction: root.selectedReaderType = ReaderManagerPluginType.UNKNOWN
			onCancel: d.cancel()
			onCheckSuccess: {
				d.stopScan();
				root.checkSuccess();
			}
			onLeaveView: d.cancel()
			onRestartCheck: {
				root.popAll();
				d.startCheck();
			}
			onShowRemoteServiceSettings: root.push(remoteServiceSettings)
		}
	}
	ColumnLayout {
		Layout.leftMargin: Style.dimens.pane_padding
		Layout.rightMargin: Style.dimens.pane_padding
		spacing: Style.dimens.pane_spacing

		TintableIcon {
			Layout.alignment: Qt.AlignHCenter
			source: "qrc:///images/mobile/device.svg"
			sourceSize.height: Style.dimens.header_icon_size
			tintColor: Style.color.image
		}
		GText {
			//: LABEL ANDROID IOS
			text: qsTr("Check if your device & ID card are ready for use")
			textStyle: Style.text.subline
		}
		GText {
			//: LABEL ANDROID IOS
			text: qsTr("To use the eID function, your device must meet certain technical requirements. Furthermore, the eID function must be activated.")
		}
		GSpacer {
			Layout.fillHeight: true
		}
		GButton {
			Layout.alignment: Qt.AlignHCenter
			icon.source: "qrc:///images/device_button.svg"

			//: LABEL ANDROID IOS
			text: qsTr("Start check")
			tintIcon: true

			onClicked: {
				if (ApplicationModel.nfcState === ApplicationModel.NfcState.UNAVAILABLE && root.readerType !== ReaderManagerPluginType.REMOTE_IFD) {
					root.setLockedAndHidden();
					root.push(noNfcSuggestion);
				} else {
					d.startCheck();
				}
			}
		}
	}
}
