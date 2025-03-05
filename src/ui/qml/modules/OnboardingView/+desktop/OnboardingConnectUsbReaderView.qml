/**
 * Copyright (c) 2024-2025 Governikus GmbH & Co. KG, Germany
 */

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts

import Governikus.Global
import Governikus.SettingsView
import Governikus.Style
import Governikus.TitleBar
import Governikus.Type

BaseOnboardingView {
	id: root

	function continueWhenReaderFound() {
		if (readerView.hasConnectedReader)
			root.continueOnboarding();
	}

	spacing: Style.dimens.pane_spacing

	titleBarSettings: TitleBarSettings {
		navigationAction: NavigationAction.Back
		startEnabled: false

		onNavigationActionClicked: root.leaveView()
	}

	Keys.onEnterPressed: root.continueWhenReaderFound()
	Keys.onEscapePressed: root.leaveView()
	Keys.onReturnPressed: root.continueWhenReaderFound()

	ReaderScanEnabler {
		pluginType: ReaderManagerPluginType.PCSC
	}
	GText {
		Layout.alignment: Qt.AlignHCenter
		Layout.bottomMargin: Style.dimens.pane_spacing
		horizontalAlignment: Text.AlignHCenter
		//: LABEL DESKTOP
		text: qsTr("Set up card reader")
		textStyle: Style.text.headline
	}
	GText {
		//: LABEL DESKTOP
		text: qsTr("Connect an USB card reader")
		textStyle: Style.text.subline
	}
	GText {
		text: readerView.hintTextBase
	}
	CardReaderView {
		id: readerView

		Layout.fillWidth: true
		enablePaneOptic: false
		showHint: false
	}
	GSpacer {
		Layout.fillHeight: true
	}
	GContinueButton {
		enabled: readerView.hasConnectedReader

		onClicked: root.continueOnboarding()
	}
}
