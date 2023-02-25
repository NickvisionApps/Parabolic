#!/usr/bin/env python3

import os
import re
import sys
import xml.etree.ElementTree as ET
from pathlib import Path

script_dir = Path(__file__).parent
resx_dir = (script_dir / '../NickvisionTubeConverter.Shared/Resources/').resolve()
install_prefix = sys.argv[1] if len(sys.argv) > 1 else '/usr'

regex = re.compile(r'Strings\.(.+)\.resx')
desktop_comments = []
desktop_keywords = []
meta_summaries = []
meta_descriptions = []
for filename in os.listdir(resx_dir):
    regex_match = regex.search(filename)
    if regex_match:
        lang_code = regex_match.group(1)
        tree = ET.parse(f'{resx_dir}/{filename}')
        root = tree.getroot()
        for item in root.findall('./data'):
            if item.attrib['name'] == 'Description':
                text = item.find('value').text
                if text:
                    desktop_comments.append(f'Comment[{lang_code}]={text}')
                    meta_descriptions.append(f'    <p xml:lang="{lang_code}">\n      {text}\n    </p>')
            elif item.attrib['name'] == 'Summary.GTK':
                text = item.find('value').text
                if text:
                    meta_summaries.append(f'  <summary xml:lang="{lang_code}">{text}</summary>')
            elif item.attrib['name'] == 'Keywords.GTK':
                text = item.find('value').text
                if text:
                    desktop_keywords.append(f'Keywords[{lang_code}]={text}')
desktop_comments.sort()
desktop_keywords.sort()
meta_summaries.sort()
meta_descriptions.sort()

with open(f'{install_prefix}/share/applications/org.nickvision.tubeconverter.desktop', 'r') as f:
    contents = f.readlines()
    new_contents = contents.copy()
j = 0
for i in range(len(contents)):
    if contents[i].startswith('Comment='):
        new_contents.insert(j + 1, "\n".join(desktop_comments) + "\n")
        j += 1
    elif contents[i].startswith('Keywords='):
        new_contents.insert(j + 1, "\n".join(desktop_keywords) + "\n")
        j += 1
    j += 1
with open(f'{install_prefix}/share/applications/org.nickvision.tubeconverter.desktop', 'w') as f:
    new_contents = "".join(new_contents)
    f.write(new_contents)

with open(f'{install_prefix}/share/metainfo/org.nickvision.tubeconverter.metainfo.xml', 'r') as f:
    contents = f.readlines()
    new_contents = contents.copy()
j = 0
for i in range(len(contents)):
    if contents[i].find('<summary>') > -1:
        new_contents.insert(j + 1, "\n".join(meta_summaries) + "\n")
        j += 1
    elif contents[i].find('<description>') > -1:
        new_contents.insert(j + 4, "\n".join(meta_descriptions) + "\n")
        break
    j += 1
with open(f'{install_prefix}/share/metainfo/org.nickvision.tubeconverter.metainfo.xml', 'w') as f:
    new_contents = "".join(new_contents)
    f.write(new_contents)
