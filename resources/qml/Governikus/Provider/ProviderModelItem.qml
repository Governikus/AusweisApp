/*
 * \copyright Copyright (c) 2016-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import Governikus.Type.ApplicationModel 1.0

/*
 * Convenience utility to access properties of a ProviderModel item
 * This ensures having always a defined string, i.e. a non-null string object.
 */
Item {
	id: baseItem

	readonly property string address: !!modelItem && !!modelItem.providerAddress ? modelItem.providerAddress : ""
	readonly property string addressDomain: !!modelItem && !!modelItem.providerAddressDomain ? modelItem.providerAddressDomain : ""
	readonly property string category: !!modelItem && !!modelItem.providerCategory ? modelItem.providerCategory : ""
	readonly property ListModel contactModel: ListModel {
		readonly property alias email: baseItem.email
		readonly property alias homepage: baseItem.homepage
		readonly property alias homepageBase: baseItem.homepageBase
		readonly property alias phone: baseItem.phone
		readonly property alias phoneCost: baseItem.phoneCost
		readonly property string phoneDisplayString: {
			var s = "";
			if (!!phone) {
				s = '<a href="tel:' + phone + '">' + phone + "</a>";
				if (!!phoneCost) {
					s += "<br/>" + phoneCost;
				}
			}
			return s;
		}
		readonly property alias postalAddress: baseItem.postalAddress

		function updateHomepage() {
			setProperty(0, "text", !!homepage ? '<a href="' + homepage + '">' + homepageBase + "</a>" : "");
			setProperty(0, "accessibleText", !!homepageBase ?
				//: INFO ALL_PLATFORMS A11y action text appended to provider homepage to be read read by screen reader.
				homepageBase + " . " + qsTr("Click to open homepage.") : "");
			setProperty(0, "link", homepage);
		}

		onEmailChanged: {
			setProperty(1, "text", !!email ? '<a href="mailto:' + email + '">' + email + "</a>" : "");
			setProperty(1, "accessibleText", email ?
				//: INFO ALL_PLATFORMS A11y action text appended to provider mail to be read read by screen reader.
				email + " . " + qsTr("Click to send email.") : "");
			setProperty(1, "link", !!email ? "mailto:" + email : "");
		}
		onHomepageBaseChanged: updateHomepage()
		onHomepageChanged: updateHomepage()
		onPhoneDisplayStringChanged: {
			setProperty(2, "text", phoneDisplayString);
			//: LABEL DESKTOP
			var coststring = !!phoneCost ? ", " + qsTr("Costs") + ": " + phoneCost : "";
			setProperty(2, "accessibleText", !!phone ?
				//: INFO ALL_PLATFORMS A11y action text appended to provider phone number to be read read by screen reader.
				phone + coststring + " . " + qsTr("Click to call.") : "");
			setProperty(2, "link", !!phone ? "tel:" + phone : "");
		}
		onPostalAddressChanged: {
			var dest;
			if (Qt.platform.os === "android") {
				dest = 'geo:0,0?q=';
			} else if (Qt.platform.os === "ios") {
				dest = 'maps:0,0?q=';
			} else {
				dest = 'https://www.google.com/maps?q=';
			}
			let nonHtmlAddress = !!postalAddress ? postalAddress.replace(/<br\s*[\/]?>/gi, ", ") : "";
			dest = !!nonHtmlAddress ? dest + encodeURIComponent(nonHtmlAddress) : "";
			setProperty(3, "text", !!postalAddress ? '<a href="' + dest + '">' + postalAddress + "</a>" : "");
			setProperty(3, "accessibleText", !!nonHtmlAddress ?
				//: INFO ALL_PLATFORMS A11y action text appended to provider address maps url to be read read by screen reader.
				nonHtmlAddress + " . " + qsTr("Click to open map.") : "");
			setProperty(3, "link", dest);
		}

		ListElement {
			accessibleText: ""
			iconSource: "qrc:///images/material_open_in_new.svg"
			label: QT_TR_NOOP("Homepage")
			link: ""
			text: ""
		}
		ListElement {
			accessibleText: ""
			iconSource: "qrc:///images/material_mail.svg"
			label: QT_TR_NOOP("E-Mail")
			link: ""
			text: ""
		}
		ListElement {
			accessibleText: ""
			iconSource: "qrc:///images/material_phone.svg"
			label: QT_TR_NOOP("Phone")
			link: ""
			text: ""
		}
		ListElement {
			accessibleText: ""
			iconSource: "qrc:///images/material_location.svg"
			label: QT_TR_NOOP("Address")
			link: ""
			text: ""
		}
	}
	readonly property string email: !!modelItem && !!modelItem.providerEmail ? modelItem.providerEmail : ""
	readonly property string homepage: !!modelItem && !!modelItem.providerHomepage ? modelItem.providerHomepage : ""
	readonly property string homepageBase: !!modelItem && !!modelItem.providerHomepageBase ? modelItem.providerHomepageBase : ""
	readonly property string icon: !!modelItem && !!modelItem.providerIcon ? modelItem.providerIcon : ""
	readonly property string image: !!modelItem && !!modelItem.providerImage ? modelItem.providerImage : ""
	readonly property string longDescription: !!modelItem && !!modelItem.providerLongDescription ? modelItem.providerLongDescription : ""
	readonly property string longName: !!modelItem && !!modelItem.providerLongName ? modelItem.providerLongName : ""
	property var modelItem
	readonly property string phone: !!modelItem && !!modelItem.providerPhone ? modelItem.providerPhone : ""
	readonly property string phoneCost: !!modelItem && !!modelItem.providerPhoneCost ? modelItem.providerPhoneCost : ""
	readonly property string postalAddress: !!modelItem && !!modelItem.providerPostalAddress ? modelItem.providerPostalAddress : ""
	readonly property string shortName: !!modelItem && !!modelItem.providerShortName ? modelItem.providerShortName : ""
}
