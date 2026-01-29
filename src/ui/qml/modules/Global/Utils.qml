/**
 * Copyright (c) 2019-2026 Governikus GmbH & Co. KG, Germany
 */

pragma Singleton

import QtQuick

import Governikus.Type

QtObject {
	function findGFlickable(pItem) {
		if (pItem === null) {
			return null;
		}
		if (pItem instanceof GFlickable) {
			return pItem;
		}
		return findGFlickable(pItem.parent);
	}
	function getSecondPRSHintText(pGlobalStatusCode) {
		if (PinResetInformationModel.hasPinResetService && pGlobalStatusCode === GlobalStatusCode.Card_Pin_Deactivated)
			return PinResetInformationModel.activateOnlineFunctionAtAuthorityHint;

		if (PinResetInformationModel.hasPinResetService && pGlobalStatusCode === GlobalStatusCode.Card_Puk_Blocked)
			return PinResetInformationModel.resetPinAtAuthorityHint;

		return "";
	}
	function platformAgnosticLinkOpenText(pLink, pName) {
		if (Qt.platform.os === "ios" || Qt.platform.os === "android") {
			//: MOBILE Hint that a link is present, which will open in the browser
			return qsTr("Tap to open the following website in your browser: %1").arg(pLink);
		}
		//: DESKTOP Hint that a link is present, which will open in the browser
		let desktopDescription = qsTr("Press space to open the following website in your browser: %1").arg(pLink);
		if (Qt.platform.os === "osx") {
			return "%1, %2".arg(pName).arg(desktopDescription);
		}
		return desktopDescription;
	}
	function positionFlickableAtItem(pFlickable, pItem, pPositionItemAtMiddle = false) {
		let castItem = (pItem as Item);
		let referenceItem = castItem.parent ? (castItem.parent as Item) : castItem;
		let mappedPosition = pFlickable.mapFromItem(referenceItem, castItem.x, castItem.y);

		if (pPositionItemAtMiddle) {
			const absY = castItem.mapToItem(pFlickable.contentItem, 0, 0).y;
			pFlickable.contentY = absY - Math.floor(pFlickable.height * 0.5);
			return;
		}
		if (Qt.platform.os === "ios") {
			UtilsIOS.handleFlickable(pFlickable, castItem);
			return;
		}
		if (mappedPosition.y < 0) {
			pFlickable.contentY = Math.max(0, pFlickable.contentY + mappedPosition.y);
			return;
		}
		let viewDifference = mappedPosition.y + castItem.height - pFlickable.height;
		if (viewDifference > 0) {
			pFlickable.contentY = Math.min(pFlickable.contentY + viewDifference, pFlickable.contentHeight - pFlickable.height);
		}
	}
	function positionViewAtItem(pItem, pParent = pItem.parent, pPositionItemAtMiddle = false) {
		if (pParent === null) {
			return;
		}
		if (pParent instanceof Flickable) {
			positionFlickableAtItem(pParent, pItem, pPositionItemAtMiddle);
			return;
		}
		positionViewAtItem(pItem, pParent.parent, pPositionItemAtMiddle);
	}
	function scrollPageDownOnGFlickable(pItem) {
		let gflickable = findGFlickable(pItem);
		if (gflickable) {
			gflickable.scrollPageDown();
		}
	}
	function scrollPageUpOnGFlickable(pItem) {
		let gflickable = findGFlickable(pItem);
		if (gflickable) {
			gflickable.scrollPageUp();
		}
	}
	function shuffle(pArray) {
		for (let i = pArray.length - 1; i > 0; i--) {
			let j = ApplicationModel.randomInt(0, i);
			let x = pArray[i];
			pArray[i] = pArray[j];
			pArray[j] = x;
		}
		return pArray;
	}
	function useSpecialAppleTabRole(pRole) {
		if ((Qt.platform.os === "osx" || Qt.platform.os === "ios") && !ApplicationModel.screenReaderRunning)
			return Accessible.Link;

		return pRole;
	}
}
