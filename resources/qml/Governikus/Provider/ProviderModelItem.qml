/*
 * \copyright Copyright (c) 2016-2022 Governikus GmbH & Co. KG, Germany
 */

import QtQuick 2.15

import Governikus.Type.ApplicationModel 1.0

/*
 * Convenience utility to access properties of a ProviderModel item
 * This ensures having always a defined string, i.e. a non-null string object.
 */
Item {
	id: baseItem
	property var modelItem

	readonly property string category: !!modelItem && !!modelItem.providerCategory ? modelItem.providerCategory : ""
	readonly property string shortName: !!modelItem && !!modelItem.providerShortName ? modelItem.providerShortName : ""
	readonly property string longName: !!modelItem && !!modelItem.providerLongName ? modelItem.providerLongName : ""
	readonly property string longDescription: !!modelItem && !!modelItem.providerLongDescription ? modelItem.providerLongDescription : ""
	readonly property string address: !!modelItem && !!modelItem.providerAddress ? modelItem.providerAddress : ""
	readonly property string addressDomain: !!modelItem && !!modelItem.providerAddressDomain ? modelItem.providerAddressDomain : ""
	readonly property string homepage: !!modelItem && !!modelItem.providerHomepage ? modelItem.providerHomepage : ""
	readonly property string homepageBase: !!modelItem && !!modelItem.providerHomepageBase ? modelItem.providerHomepageBase : ""
	readonly property string phone: !!modelItem && !!modelItem.providerPhone ? modelItem.providerPhone : ""
	readonly property string phoneCost: !!modelItem && !!modelItem.providerPhoneCost ? modelItem.providerPhoneCost : ""
	readonly property string email: !!modelItem && !!modelItem.providerEmail ? modelItem.providerEmail : ""
	readonly property string postalAddress: !!modelItem && !!modelItem.providerPostalAddress ? modelItem.providerPostalAddress : ""
	readonly property string icon: !!modelItem && !!modelItem.providerIcon ? modelItem.providerIcon : ""
	readonly property string image: !!modelItem && !!modelItem.providerImage ? modelItem.providerImage : ""

	readonly property ListModel contactModel: ListModel {
		readonly property alias homepage: baseItem.homepage
		readonly property alias homepageBase: baseItem.homepageBase
		readonly property alias email: baseItem.email
		readonly property alias phone: baseItem.phone
		readonly property alias phoneCost: baseItem.phoneCost
		readonly property alias postalAddress: baseItem.postalAddress

		readonly property string phoneDisplayString: {
			var s = ""
			if (!!phone) {
				s = '<a href="tel:' + phone + '">' + phone + "</a>"
				if (!!phoneCost) {
					s += "<br/>" + phoneCost
				}
			}
			return s
		}

		function updateHomepage() {
			setProperty(0, "text", !!homepage ? '<a href="' + homepage + '">' + homepageBase + "</a>" : "")
			setProperty(0, "accessibleText", !!homepageBase
				//: INFO ALL_PLATFORMS A11y action text appended to provider homepage to be read read by screen reader.
				? homepageBase + " . " + qsTr("Click to open homepage.")
				: ""
			)
			setProperty(0, "link", homepage)
		}

		onHomepageChanged: updateHomepage()
		onHomepageBaseChanged: updateHomepage()

		onEmailChanged: {
			setProperty(1, "text", !!email ? '<a href="mailto:' + email + '">' + email + "</a>" : "")
			setProperty(1, "accessibleText", email
				//: INFO ALL_PLATFORMS A11y action text appended to provider mail to be read read by screen reader.
				? email + " . " + qsTr("Click to send email.")
				: ""
			)
			setProperty(1, "link", !!email ? "mailto:" + email : "")
		}
		onPhoneDisplayStringChanged: {
			setProperty(2, "text", phoneDisplayString)
			//: LABEL DESKTOP
			var coststring = !!phoneCost ? ", " + qsTr("Costs") + ": " + phoneCost : ""
			setProperty(2, "accessibleText", !!phone
				//: INFO ALL_PLATFORMS A11y action text appended to provider phone number to be read read by screen reader.
				? phone + coststring + " . " + qsTr("Click to call.")
				: ""
			)
			setProperty(2, "link", !!phone? "tel:" + phone : "")
		}
		onPostalAddressChanged: {
			var dest
			if (Qt.platform.os === "android") {
				dest = 'geo:0,0?q='
			}
			else if (Qt.platform.os === "ios") {
				dest = 'maps:0,0?q='
			}
			else {
				dest = 'https://www.google.com/maps?q='
			}
			let nonHtmlAddress = !!postalAddress ? postalAddress.replace(/<br\s*[\/]?>/gi,", ") : ""
			dest = !!nonHtmlAddress ? dest + encodeURIComponent(nonHtmlAddress) : ""
			setProperty(3, "text", !!postalAddress ? '<a href="' + dest + '">' + postalAddress + "</a>" : "")
			setProperty(3, "accessibleText", !!nonHtmlAddress
				//: INFO ALL_PLATFORMS A11y action text appended to provider address maps url to be read read by screen reader.
				? nonHtmlAddress + " . " + qsTr("Click to open map.")
				: ""
			)
			setProperty(3, "link", dest)
		}

		ListElement {
			iconSource: "qrc:///images/material_open_in_new.svg"
			label: QT_TR_NOOP("Homepage")
			text: ""
			accessibleText: ""
			link: ""
		}

		ListElement {
			iconSource: "qrc:///images/material_mail.svg"
			label: QT_TR_NOOP("E-Mail")
			text: ""
			accessibleText: ""
			link: ""
		}

		ListElement {
			iconSource: "qrc:///images/material_phone.svg"
			label: QT_TR_NOOP("Phone")
			text: ""
			accessibleText: ""
			link: ""
		}

		ListElement {
			iconSource: "qrc:///images/material_location.svg"
			label: QT_TR_NOOP("Address")
			text: ""
			accessibleText: ""
			link: ""
		}
	}
}
