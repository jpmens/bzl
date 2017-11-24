#!/usr/bin/env python

# JP Mens
# Tested with 9.9.9

#import xml.etree.ElementTree
#e = xml.etree.ElementTree.parse('st.xml').getroot()

from xml.etree import cElementTree as ElementTree
e = ElementTree.parse('st.xml').getroot()

'''
<isc version="1.0">
  <bind>
    <statistics version="2.2">
      <views>
        <view>
          <name>_default</name>
          <zones>
            <zone>
              <name>example.com</name>
'''

for atype in e.findall('.//bind/statistics/views/view/zones/zone/name'):
    print atype.text
