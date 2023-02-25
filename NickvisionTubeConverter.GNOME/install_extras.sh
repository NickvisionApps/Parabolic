#!/bin/sh

INSTALL_PREFIX="/usr"
if [ -n "${1}" ]
then
    INSTALL_PREFIX="${1}"
fi
echo Install prefix: "${INSTALL_PREFIX}"

if [ "$(basename "$(pwd)")" = "NickvisionTubeConverter.GNOME" ]
then
    cd ..
fi

echo "Installing icons..."
mkdir -p "${INSTALL_PREFIX}"/share/icons/hicolor/scalable/apps
for icon in org.nickvision.tubeconverter.svg org.nickvision.tubeconverter-devel.svg
do
    cp ./NickvisionTubeConverter.Shared/Resources/${icon}               \
       "${INSTALL_PREFIX}"/share/icons/hicolor/scalable/apps/
done
mkdir -p "${INSTALL_PREFIX}"/share/icons/hicolor/symbolic/apps
for icon in org.nickvision.tubeconverter-symbolic.svg  \
                moon-outline-symbolic.svg                    \
                sun-outline-symbolic.svg
do
    cp ./NickvisionTubeConverter.Shared/Resources/${icon}               \
       "${INSTALL_PREFIX}"/share/icons/hicolor/symbolic/apps/
done

echo "Installing GResource..."
mkdir -p "${INSTALL_PREFIX}"/share/org.nickvision.tubeconverter
glib-compile-resources ./NickvisionTubeConverter.GNOME/Resources/org.nickvision.tubeconverter.gresource.xml
mv ./NickvisionTubeConverter.GNOME/Resources/org.nickvision.tubeconverter.gresource \
   "${INSTALL_PREFIX}"/share/org.nickvision.tubeconverter/

echo "Installing desktop file..."
mkdir -p "${INSTALL_PREFIX}"/share/applications
cp ./NickvisionTubeConverter.GNOME/org.nickvision.tubeconverter.desktop \
   "${INSTALL_PREFIX}"/share/applications/

echo "Installing metainfo..."
mkdir -p "${INSTALL_PREFIX}"/share/metainfo
cp ./NickvisionTubeConverter.GNOME/org.nickvision.tubeconverter.metainfo.xml    \
   "${INSTALL_PREFIX}"/share/metainfo/

echo "Translating desktop file and metainfo..."
python3 ./NickvisionTubeConverter.GNOME/translate_meta.py ${INSTALL_PREFIX}

# echo "Installing mime types..."
# mkdir -p "${INSTALL_PREFIX}"/share/mime/packages
# cp ./NickvisionTubeConverter.GNOME/org.nickvision.tubeconverter.extension.xml   \
#    "${INSTALL_PREFIX}"/share/mime/packages/
# update-mime-database "${INSTALL_PREFIX}"/share/mime/

echo "Done!"
