/**
 * Copyright (c) 2024-2026 Governikus GmbH & Co. KG, Germany
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

	spacing: Style.dimens.pane_spacing

	titleBarSettings: TitleBarSettings {
		navigationAction: NavigationAction.Action.Back
		startEnabled: false

		onNavigationActionClicked: root.leaveView()
	}

	ReaderScanEnabler {
		pluginType: ReaderManagerPluginType.PCSC
	}
	Heading {
		Layout.bottomMargin: Style.dimens.pane_spacing
		Layout.topMargin: -root.spacing
		//: DESKTOP
		text: qsTr("Set up card reader")
	}
	Subheading {
		//: DESKTOP
		text: qsTr("Connect an USB card reader")
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
	GContinueButton {
		enabled: readerView.hasConnectedReader

		onClicked: root.continueOnboarding()
	}
}
