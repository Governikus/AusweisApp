/**
 * Copyright (c) 2026 Governikus Service GmbH, Germany
 */

import QtQuick
import QtQuick.Layouts

import Governikus.Global
import Governikus.Style
import Governikus.Type

GPaneBackground {
	id: root

	property alias currentPin: pskText.text
	readonly property double remainingValiditySeconds: RemoteServiceModel.remainingPskValidity / 1000

	//: MOBILE %1 is replaced with the current PSK, %2 with the remaining seconds of the PSK validity
	Accessible.name: currentPin === "" ? "" : qsTr("Pairing code: %1, valid for %2 seconds").arg(Utils.splitCharacters(root.currentPin)).arg(Math.floor(activeFocus ? d.a11ySeconds : remainingValiditySeconds))
	Accessible.role: Accessible.StaticText
	implicitHeight: rowLayout.implicitHeight + rowLayout.anchors.topMargin + rowLayout.anchors.bottomMargin
	implicitWidth: rowLayout.implicitWidth + rowLayout.anchors.leftMargin + rowLayout.anchors.rightMargin

	onActiveFocusChanged: {
		if (activeFocus) {
			d.a11ySeconds = remainingValiditySeconds;
		}
	}
	onRemainingValiditySecondsChanged: {
		if (visible && remainingValiditySeconds > 0) {
			d.notifyScreenReaderInIntervals(remainingValiditySeconds);
		}
	}
	onVisibleChanged: {
		if (visible) {
			root.forceActiveFocus(Qt.MouseFocusReason);
		}
	}

	QtObject {
		id: d

		property int a11ySeconds: 0
		property var intervals: new Map([[5, false], [10, false], [30, false]])

		function notifyScreenReaderInIntervals(pRemainingValidity) {
			let didAnnounce = false;
			for (const [interval, intervalAnnounced] of intervals.entries()) {
				if ((interval - 1) <= pRemainingValidity && pRemainingValidity <= (interval + 1)) {
					if (!didAnnounce && !intervalAnnounced) {
						// MOBILE %1 is replaced with the remaining seconds of current validity, %2 with the current pairing code
						root.Accessible.announce(qsTr("%1 seconds left before the pairing code %2 expires").arg(interval).arg(Utils.splitCharacters(root.currentPin)), Accessible.Assertive);
						didAnnounce = true;
					}

					intervals.set(interval, true);
				}
			}
		}
		function resetIntervals() {
			intervals.forEach((a11yAnnounced, interval) => {
				intervals.set(interval, false);
			});
		}
	}
	Connections {
		function onFirePskChanged(pPsk, pInitialPsk) {
			d.resetIntervals();

			if (RemoteServiceModel.isPairing && !pInitialPsk) {
				//: MOBILE %1 is replaced with the new PSK
				root.Accessible.announce(qsTr("The pairing code has expired and a new code was generated: %1").arg(Utils.splitCharacters(root.currentPin)));
			}
		}

		target: RemoteServiceModel
	}
	RowLayout {
		id: rowLayout

		anchors.fill: parent
		anchors.margins: Style.dimens.pane_padding

		GText {
			id: pskText

			Accessible.ignored: true
			textStyle: Style.text.headline
		}
		GSpacer {
			Layout.fillWidth: true
		}
		CircularProgressIndicator {
			Accessible.ignored: true
			reverse: true
			text: {
				let formattedMinutes = Math.floor(root.remainingValiditySeconds / 60).toString().padStart(2, "0");
				let formattedSeconds = Math.floor(root.remainingValiditySeconds % 60).toString().padStart(2, "0");
				return formattedMinutes + ":" + formattedSeconds;
			}
			value: RemoteServiceModel.remainingPskValidity / RemoteServiceModel.pskValidity
		}
	}
}
