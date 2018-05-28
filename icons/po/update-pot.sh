#!/bin/sh


echo -n 'Preparing files...'
cd ..

rm -f novprog.desktop.in
cp novprog.desktop novprog.desktop.in
sed -e '/^Name\[/ d' \
	-e '/^GenericName\[/ d' \
	-e '/^Comment\[/ d' \
	-e '/^Icon/ d' \
	-e '/^Keywords/ d' \
	-i novprog.desktop.in

rm -f novprog.appdata.xml.in
cp novprog.appdata.xml novprog.appdata.xml.in
sed -e '/p xml:lang/ d' \
	-e '/summary xml:lang/ d' \
	-e '/name xml:lang/ d' \
	-e '/<developer_name>/ d' \
	-i novprog.appdata.xml.in

cd po
echo ' DONE'


echo -n 'Extracting messages...'
xgettext --from-code=UTF-8 --output=description.pot \
	--package-name='NovProg' --copyright-holder='Graeme Gott' \
	../*.in
sed 's/CHARSET/UTF-8/' -i description.pot
echo ' DONE'


echo -n 'Cleaning up...'
cd ..

rm -f novprog.desktop.in
rm -f novprog.appdata.xml.in

echo ' DONE'
