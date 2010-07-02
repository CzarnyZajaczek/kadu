#!/bin/sh
# destination directory, Kadu.app will be created here
# katalog docelowy, w nim zostanie utworzony katalog Kadu.app
DEST=~/Desktop/kadu

QTDIR=/Users/tomek/Desktop/kadu/qt

# prefix of compiled libsndfile and openssl
# prefiks skompilowanych bibliotek libsndfile i libgadu
SNDFILEPATH=/Users/tomek/Desktop/kadu/libsndfile
LIBGADU=/Users/tomek/Desktop/kadu/libgadu

#OPENSSLPATH=/Users/tomek/Desktop/kadu/openssl
OPENSSLPATH=/usr
# version of openssl, without letters
# wersja openssla, bez liter
SSLVER=0.9.7

INSTALLED_DIR=/usr/local/

echo "Set paths in this file and remove this lines (including exit)"
echo "Ustaw sciezki w tym pliku i usun te linie (lacznie z exit)"
echo
exit

if [ ! -f VERSION ]; then
	echo "run this script from main Kadu directory"
	echo "uruchom ten skrypt z glownego katalogu Kadu"
	exit
fi

if [ ! -f ./kadu-core/kadu ]; then
	echo "compile Kadu first!"
	echo "wpierw skompiluj Kadu!"
	exit
fi

if [ ! -f ${INSTALLED_DIR}/bin/kadu ]; then
	echo "do make install first!"
	echo "wpierw zrob make install!"
	exit
fi

if [ ! -f ${QTDIR}/bin/qmake ]; then
	echo "wrong QTDIR"
	echo "QTDIR zle ustawione"
	exit
fi

KADUVERSION=`cat VERSION`
echo "log: removing old Kadu.app"
rm -rf ${DEST}/Kadu.app/
mkdir ${DEST}/Kadu.app/

echo "log: copying files"
cp -r ${INSTALLED_DIR}/share/kadu ${DEST}/Kadu.app
cp -r ${INSTALLED_DIR}/lib/kadu ${DEST}/Kadu.app

CNT_DIR=${DEST}/Kadu.app/Contents
mkdir ${CNT_DIR};

echo "log: creating PkgInfo"
echo "APPLKadu" > ${CNT_DIR}/PkgInfo

echo "log: creating Info.plist"
echo "<?xml version='1.0' encoding='UTF-8'?>
<plist version='1.0'>
<dict>
	<key>CFBundleDevelopmentRegion</key>
	<string>English</string>
	<key>CFBundleDisplayName</key>
	<string>Kadu</string>
	<key>CFBundleExecutable</key>
	<string>kadu</string>
	<key>CFBundleIconFile</key>
	<string>kadu.icns</string>
	<key>CFBundleIdentifier</key>
	<string>net.kadu</string>
	<key>CFBundleLongVersionString</key>
	<string>${KADUVERSION}</string>
	<key>CFBundleName</key>
	<string>Kadu</string>
	<key>CFBundlePackageType</key>
	<string>APPL</string>
	<key>CFBundleShortVersionString</key>
	<string>${KADUVERSION}</string>
	<key>CFBundleSignature</key>
	<string>Kadu</string>
	<key>CFBundleVersion</key>
	<string>${KADUVERSION}</string>
	<key>NSAppleScriptEnabled</key>
	<false/>
	<key>NSHumanReadableCopyright</key>
	<string>Copyright 2001-2007 Kadu Team</string>
</dict>
</plist>" >> ${CNT_DIR}/Info.plist

MACOS_DIR=${CNT_DIR}/MacOS;
mkdir ${MACOS_DIR}
echo "log: copying kadu binary"

RSC_DIR=${CNT_DIR}/Resources
mkdir ${RSC_DIR}
cp kadu-core/kadu.icns $RSC_DIR/
cp kadu-core/hi48-app-kadu.png ${DEST}/Kadu.app/kadu/kadu.png
cp ${INSTALLED_DIR}/bin/kadu ${MACOS_DIR}

echo "[Paths]
Plugins = plugins" > ${RSC_DIR}/qt.conf

mkdir -p ${CNT_DIR}/plugins/imageformats


FM_DIR=${CNT_DIR}/Frameworks
mkdir ${FM_DIR}

if [ -f ${OPENSSLPATH}/lib/libcrypto.${SSLVER}.dylib ]; then
	echo "log: copying ssl libraries"
	cp ${OPENSSLPATH}/lib/libcrypto.${SSLVER}.dylib ${FM_DIR}
	cp ${OPENSSLPATH}/lib/libssl.${SSLVER}.dylib ${FM_DIR}
	install_name_tool -id @executable_path/../Frameworks/libssl.${SSLVER}.dylib ${FM_DIR}/libssl.${SSLVER}.dylib
	install_name_tool -change ${OPENSSLPATH}/lib/libcrypto.${SSLVER}.dylib @executable_path/../Frameworks/libcrypto.${SSLVER}.dylib ${FM_DIR}/libssl.${SSLVER}.dylib
	install_name_tool -id @executable_path/../Frameworks/libcrypto.${SSLVER}.dylib ${FM_DIR}/libcrypto.${SSLVER}.dylib
fi

if [ -f ${QTDIR}/lib/Qt3Support.framework/versions/4/Qt3Support ]; then
	echo "log: copying Qt3Support library"
	cp ${QTDIR}/lib/Qt3Support.framework/versions/4/Qt3Support ${FM_DIR}
	install_name_tool -id @executable_path/../Frameworks/Qt3Support ${FM_DIR}/Qt3Support
fi

if [ -f ${QTDIR}/lib/QtGui.framework/versions/4/QtGui ]; then
	echo "log: copying QtGui library"
	cp ${QTDIR}/lib/QtGui.framework/versions/4/QtGui ${FM_DIR}
	install_name_tool -id @executable_path/../Frameworks/QtGui ${FM_DIR}/QtGui
fi

if [ -f ${QTDIR}/lib/QtWebKit.framework/versions/4/QtWebKit ]; then
	echo "log: copying QtWebKit library"
	cp ${QTDIR}/lib/QtWebKit.framework/versions/4/QtWebKit ${FM_DIR}
	install_name_tool -id @executable_path/../Frameworks/QtWebKit ${FM_DIR}/QtWebKit
fi

if [ -f ${QTDIR}/lib/QtXmlPatterns.framework/versions/4/QtXmlPatterns ]; then
	echo "log: copying QtXmlPatterns library"
	cp ${QTDIR}/lib/QtXmlPatterns.framework/versions/4/QtXmlPatterns ${FM_DIR}
	install_name_tool -id @executable_path/../Frameworks/QtXmlPatterns ${FM_DIR}/QtXmlPatterns
fi

if [ -f ${QTDIR}/lib/QtXml.framework/versions/4/QtXml ]; then
	echo "log: copying QtXml library"
	cp ${QTDIR}/lib/QtXml.framework/versions/4/QtXml ${FM_DIR}
	install_name_tool -id @executable_path/../Frameworks/QtXml ${FM_DIR}/QtXml
fi

if [ -f ${QTDIR}/lib/QtSql.framework/versions/4/QtSql ]; then
	echo "log: copying QtSql library"
	cp ${QTDIR}/lib/QtSql.framework/versions/4/QtSql ${FM_DIR}
	install_name_tool -id @executable_path/../Frameworks/QtSql ${FM_DIR}/QtSql
fi

if [ -f ${QTDIR}/lib/QtNetwork.framework/versions/4/QtNetwork ]; then
	echo "log: copying QtNetwork library"
	cp ${QTDIR}/lib/QtNetwork.framework/versions/4/QtNetwork ${FM_DIR}
	install_name_tool -id @executable_path/../Frameworks/QtNetwork ${FM_DIR}/QtNetwork	
fi

if [ -f ${QTDIR}/lib/QtCore.framework/versions/4/QtCore ]; then
	echo "log: copying QtCore library"
	cp ${QTDIR}/lib/QtCore.framework/versions/4/QtCore ${FM_DIR}
	install_name_tool -id @executable_path/../Frameworks/QtCore ${FM_DIR}/QtCore
fi

if [ -f ${QTDIR}/lib/QtDBus.framework/versions/4/QtDBus ]; then
	echo "log: copying QtDBus library"
	cp ${QTDIR}/lib/QtDBus.framework/versions/4/QtDBus ${FM_DIR}
	install_name_tool -id @executable_path/../Frameworks/QtDBus ${FM_DIR}/QtDBus
fi

if [ -f ${QTDIR}/lib/phonon.framework/versions/4/phonon ]; then
	echo "log: copying phonon library"
	cp ${QTDIR}/lib/phonon.framework/versions/4/phonon ${FM_DIR}
	install_name_tool -id @executable_path/../Frameworks/phonon.framework ${FM_DIR}/phonon
fi

if [ -f ${QTDIR}/plugins/imageformats/libqjpeg.dylib ]; then
	echo "log: copying Qt image plugins"
	cp -f  ${QTDIR}/plugins/imageformats/libqjpeg.dylib ${CNT_DIR}/plugins/imageformats/libqjpeg.dylib
	cp -f  ${QTDIR}/plugins/imageformats/libqgif.dylib ${CNT_DIR}/plugins/imageformats/libqgif.dylib
	cp -f  ${QTDIR}/plugins/imageformats/libqmng.dylib ${CNT_DIR}/plugins/imageformats/libqmng.dylib
	cp -f  ${QTDIR}/plugins/imageformats/libqsvg.dylib ${CNT_DIR}/plugins/imageformats/libqsvg.dylib
	cp -f  ${QTDIR}/plugins/imageformats/libqtiff.dylib ${CNT_DIR}/plugins/imageformats/libqtiff.dylib
	cp -f  ${QTDIR}/plugins/imageformats/libqico.dylib ${CNT_DIR}/plugins/imageformats/libqico.dylib
fi

if [ -f ${SNDFILEPATH}/lib/libsndfile.1.dylib ]; then
	echo "log: copying sndfile library"
	cp ${SNDFILEPATH}/lib/libsndfile.1.dylib ${FM_DIR}
	install_name_tool -id @executable_path/../Frameworks/libsndfile.1.dylib ${FM_DIR}/libsndfile.1.dylib
fi

if [ -f ${LIBGADU}/lib/libgadu.3.dylib ]; then
	echo "log: copying libgadu library"
	cp ${LIBGADU}/lib/libgadu.3.dylib ${FM_DIR}
	install_name_tool -id @executable_path/../Frameworks/libgadu.3.dylib ${FM_DIR}/libgadu.3.dylib
fi

if [ -f  /Library/Frameworks/Growl.framework/Versions/A/Growl ]; then
	echo "log: copying growl framework"
	cp /Library/Frameworks/Growl.framework/Versions/A/Growl ${FM_DIR}
	install_name_tool -change @executable_path/../Frameworks/Growl.framework/Versions/A/Growl @executable_path/../Frameworks/Growl ${FM_DIR}/Growl
fi

cd ${FM_DIR}
echo "log: changing framework bindings"
install_name_tool -change ${QTDIR}/lib/QtSql.framework/Versions/4/QtSql @executable_path/../Frameworks/QtSql ./QtCore
install_name_tool -change ${QTDIR}/lib/QtSql.framework/Versions/4/QtSql @executable_path/../Frameworks/QtSql ./Qt3Support
install_name_tool -change ${QTDIR}/lib/QtXml.framework/Versions/4/QtXml @executable_path/../Frameworks/QtXml ./Qt3Support
install_name_tool -change ${QTDIR}/lib/QtGui.framework/Versions/4/QtGui @executable_path/../Frameworks/QtGui ./Qt3Support
install_name_tool -change ${QTDIR}/lib/QtNetwork.framework/Versions/4/QtNetwork @executable_path/../Frameworks/QtNetwork ./Qt3Support
install_name_tool -change ${QTDIR}/lib/QtCore.framework/Versions/4/QtCore @executable_path/../Frameworks/QtCore ./Qt3Support
install_name_tool -change ${QTDIR}/lib/QtCore.framework/Versions/4/QtCore @executable_path/../Frameworks/QtCore ./QtSql
install_name_tool -change ${QTDIR}/lib/QtCore.framework/Versions/4/QtCore @executable_path/../Frameworks/QtCore ./QtXml
install_name_tool -change ${QTDIR}/lib/QtCore.framework/Versions/4/QtCore @executable_path/../Frameworks/QtCore ./QtGui
install_name_tool -change ${QTDIR}/lib/QtCore.framework/Versions/4/QtCore @executable_path/../Frameworks/QtCore ./QtWebKit
install_name_tool -change ${QTDIR}/lib/QtCore.framework/Versions/4/QtCore @executable_path/../Frameworks/QtCore ./QtWebKit
install_name_tool -change ${QTDIR}/lib/QtCore.framework/Versions/4/QtCore @executable_path/../Frameworks/QtCore ./QtNetwork
install_name_tool -change ${QTDIR}/lib/QtGui.framework/Versions/4/QtGui @executable_path/../Frameworks/QtGui ./QtWebKit
install_name_tool -change ${QTDIR}/lib/QtXmlPatterns.framework/Versions/4/QtXmlPatterns @executable_path/../Frameworks/QtXmlPatterns ./QtWebKit
install_name_tool -change ${QTDIR}/lib/QtNetwork.framework/Versions/4/QtNetwork @executable_path/../Frameworks/QtNetwork ./QtWebKit
install_name_tool -change ${QTDIR}/lib/phonon.framework/Versions/4/phonon @executable_path/../Frameworks/phonon ./QtWebKit
install_name_tool -change ${QTDIR}/lib/QtDBus.framework/Versions/4/QtDBus @executable_path/../Frameworks/QtDBus ./QtWebKit
install_name_tool -change ${QTDIR}/lib/QtXml.framework/Versions/4/QtXml @executable_path/../Frameworks/QtXml ./QtWebKit
install_name_tool -change ${QTDIR}/lib/QtCore.framework/Versions/4/QtCore @executable_path/../Frameworks/QtCore ./phonon
install_name_tool -change ${QTDIR}/lib/QtGui.framework/Versions/4/QtGui @executable_path/../Frameworks/QtGui ./phonon
install_name_tool -change ${QTDIR}/lib/QtNetwork.framework/Versions/4/QtNetwork @executable_path/../Frameworks/QtNetwork ./QtXmlPatterns
install_name_tool -change ${QTDIR}/lib/QtCore.framework/Versions/4/QtCore @executable_path/../Frameworks/QtCore ./QtXmlPatterns

install_name_tool -change ${OPENSSLPATH}/lib/libssl.${SSLVER}.dylib @executable_path/../Frameworks/libssl.${SSLVER}.dylib ./libgadu.3.dylib
install_name_tool -change ${OPENSSLPATH}/lib/libcrypto.${SSLVER}.dylib @executable_path/../Frameworks/libcrypto.${SSLVER}.dylib ./libgadu.3.dylib

cd  ${CNT_DIR}/plugins/imageformats/
install_name_tool -change ${QTDIR}/lib/QtCore.framework/Versions/4/QtCore @executable_path/../Frameworks/QtCore ./libqjpeg.dylib
install_name_tool -change ${QTDIR}/lib/QtGui.framework/Versions/4/QtGui   @executable_path/../Frameworks/QtGui  ./libqjpeg.dylib
install_name_tool -change ${QTDIR}/lib/QtCore.framework/Versions/4/QtCore @executable_path/../Frameworks/QtCore ./libqgif.dylib
install_name_tool -change ${QTDIR}/lib/QtGui.framework/Versions/4/QtGui   @executable_path/../Frameworks/QtGui  ./libqgif.dylib
install_name_tool -change ${QTDIR}/lib/QtCore.framework/Versions/4/QtCore @executable_path/../Frameworks/QtCore ./libqico.dylib
install_name_tool -change ${QTDIR}/lib/QtGui.framework/Versions/4/QtGui   @executable_path/../Frameworks/QtGui  ./libqico.dylib
install_name_tool -change ${QTDIR}/lib/QtCore.framework/Versions/4/QtCore @executable_path/../Frameworks/QtCore ./libqmng.dylib
install_name_tool -change ${QTDIR}/lib/QtGui.framework/Versions/4/QtGui   @executable_path/../Frameworks/QtGui  ./libqmng.dylib
install_name_tool -change ${QTDIR}/lib/QtCore.framework/Versions/4/QtCore @executable_path/../Frameworks/QtCore ./libqsvg.dylib
install_name_tool -change ${QTDIR}/lib/QtGui.framework/Versions/4/QtGui   @executable_path/../Frameworks/QtGui  ./libqsvg.dylib
install_name_tool -change ${QTDIR}/lib/QtSvg.framework/Versions/4/QtSvg   @executable_path/../Frameworks/QtSvg  ./libqsvg.dylib
install_name_tool -change ${QTDIR}/lib/QtXml.framework/Versions/4/QtXml   @executable_path/../Frameworks/QtXml  ./libqsvg.dylib
install_name_tool -change ${QTDIR}/lib/QtCore.framework/Versions/4/QtCore @executable_path/../Frameworks/QtCore ./libqtiff.dylib
install_name_tool -change ${QTDIR}/lib/QtGui.framework/Versions/4/QtGui   @executable_path/../Frameworks/QtGui  ./libqtiff.dylib


cd ${MACOS_DIR}
echo "log: changing library bindings"
install_name_tool -change ${QTDIR}/lib/QtSql.framework/Versions/4/QtSql @executable_path/../Frameworks/QtSql ./kadu
install_name_tool -change ${QTDIR}/lib/QtSql.framework/Versions/4/QtSql @executable_path/../Frameworks/QtSql ./kadu
install_name_tool -change ${QTDIR}/lib/QtNetwork.framework/Versions/4/QtNetwork @executable_path/../Frameworks/QtNetwork ./kadu
install_name_tool -change ${QTDIR}/lib/QtGui.framework/Versions/4/QtGui @executable_path/../Frameworks/QtGui ./kadu
install_name_tool -change ${QTDIR}/lib/QtXml.framework/Versions/4/QtXml @executable_path/../Frameworks/QtXml ./kadu
install_name_tool -change ${QTDIR}/lib/Qt3Support.framework/Versions/4/Qt3Support @executable_path/../Frameworks/Qt3Support ./kadu
install_name_tool -change ${QTDIR}/lib/Qt3Support.framework/Versions/4/Qt3Support @executable_path/../Frameworks/Qt3Support ./kadu
install_name_tool -change ${QTDIR}/lib/QtWebKit.framework/Versions/4/QtWebKit @executable_path/../Frameworks/QtWebKit ./kadu
install_name_tool -change ${QTDIR}/lib/QtCore.framework/Versions/4/QtCore @executable_path/../Frameworks/QtCore ./kadu
install_name_tool -change ${QTDIR}/lib/phonon.framework/Versions/4/phonon @executable_path/../Frameworks/phonon ./kadu
install_name_tool -change ${QTDIR}/lib/QtDBus.framework/Versions/4/QtDBus @executable_path/../Frameworks/QtDBus ./kadu

install_name_tool -change ${LIBGADU}/lib/libgadu.3.dylib @executable_path/../Frameworks/libgadu.3.dylib ./kadu
install_name_tool -change ${SNDFILEPATH}/lib/libsndfile.1.dylib @executable_path/../Frameworks/libsndfile.1.dylib ./kadu
install_name_tool -change ${OPENSSLPATH}/lib/libssl.${SSLVER}.dylib @executable_path/../Frameworks/libssl.${SSLVER}.dylib ./kadu
install_name_tool -change ${OPENSSLPATH}/lib/libcrypto.${SSLVER}.dylib @executable_path/../Frameworks/libcrypto.${SSLVER}.dylib ./kadu

if [ -f ${DEST}/Kadu.app/kadu/modules/libsound.so ]; then
	install_name_tool -change ${SNDFILEPATH}/lib/libsndfile.1.dylib @executable_path/../Frameworks/libsndfile.1.dylib ${DEST}/Kadu.app/kadu/modules/libsound.so 
	install_name_tool -change ${SNDFILEPATH}//lib/libsndfile.1.dylib @executable_path/../Frameworks/libsndfile.1.dylib ${DEST}/Kadu.app/kadu/modules/libsound.so 
fi

if [ -f ${DEST}//Kadu.app/kadu/modules/libencryption_old.so ]; then
	install_name_tool -change ${OPENSSLPATH}/lib/libssl.${SSLVER}.dylib @executable_path/../Frameworks/libssl.${SSLVER}.dylib ${DEST}/Kadu.app/kadu/modules/libencryption_old.so
	install_name_tool -change ${OPENSSLPATH}/lib/libcrypto.${SSLVER}.dylib @executable_path/../Frameworks/libcrypto.${SSLVER}.dylib ${DEST}/Kadu.app/kadu/modules/libencryption_old.so
fi

if [ -f ${DEST}//Kadu.app/kadu/modules/libgrowl_notify.so ]; then
	install_name_tool -change @executable_path/../Frameworks/Growl.framework/Versions/A/Growl @executable_path/../Frameworks/Growl ${DEST}//Kadu.app/kadu/modules/libgrowl_notify.so
fi

echo "LoadByDefault=true" >> ${DEST}/Kadu.app/kadu/modules/qt4_sound.desc

echo "log: stripping binaries"
strip -x ${FM_DIR}/Qt*  ${CNT_DIR}/plugins/imageformats/* ${MACOS_DIR}/kadu
#Qt3Support QtCore QtGui QtNetwork QtSql QtWebKit QtXml phonon


echo "log: everything done"
