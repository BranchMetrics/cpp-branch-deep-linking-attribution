import json, os, shutil
from xml.dom import minidom
import xml.etree.ElementTree as ET
from xml.etree.ElementTree import Comment, Element, SubElement

# https://pymotw.com/2/xml/etree/ElementTree/create.html
def prettify(elem):
    """Return a pretty-printed XML string for the Element.
    """
    rough_string = ET.tostring(elem, 'utf-8')
    reparsed = minidom.parseString(rough_string)
    return reparsed.toprettyxml(indent="  ")

# Output header elements
root = Element("Wix", {"xmlns": "http://schemas.microsoft.com/wix/2006/wi"})
product = SubElement(root, "Product", {
    "Id": "*",
    "Name": "Branch C++ SDK for Windows",
    "Language": "1033",
    "Version": "0.1.0",
    "Manufacturer": "Branch Metrics, Inc.",
    "UpgradeCode": "29b1dc08-2190-48f0-bc3c-7455381f2156"
    })
SubElement(product, "Package", {
    "InstallerVersion": "200",
    "Compressed": "yes",
    "InstallScope": "perMachine"
    })
SubElement(product, "MajorUpgrade", {
    "DowngradeErrorMessage": "A newer version of [ProductName] is already installed."
    })
SubElement(product, "Media", {
    "Id": "1",
    "Cabinet": "cab1.cab",
    "EmbedCab": "yes"
    })
SubElement(product, "Icon", {
    "Id": "branch.ico",
    "SourceFile": "branch-badge-dark.ico"
    })
SubElement(product, "Property", {
    "Id": "ARPPRODUCTION",
    "Value": "branch.ico"
    })

feature = SubElement(product, "Feature", {
    "Id": "ProductFeature",
    "Title": "Branch Installer",
    "Level": "1"
    })

SubElement(feature, "ComponentGroupRef", {"Id": "BranchHeaders"})
SubElement(feature, "ComponentGroupRef", {"Id": "BranchLibrariesX64"})
SubElement(feature, "ComponentGroupRef", {"Id": "BranchLibrariesX86"})
SubElement(feature, "ComponentGroupRef", {"Id": "ThirdPartyHeaders"})
SubElement(feature, "ComponentGroupRef", {"Id": "ThirdPartyLibrariesX64"})
SubElement(feature, "ComponentGroupRef", {"Id": "ThirdPartyLibrariesX86"})

output = prettify(root)
print(output)
