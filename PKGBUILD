# Maintainer: Nick Logozzo <nlogozzo225@gmail.com>
pkgname=nickvision-tube-converter
pkgver=2022.6.1
pkgrel=1
pkgdesc="An easy-to-use YouTube video downloader"
arch=(x86_64)
url="https://github.com/nlogozzo/NickvisionTubeConverter"
license=(GPL3)
depends=(gtk4 libadwaita jsoncpp libcurlpp yt-dlp)
makedepends=(git cmake)
source=("git+https://github.com/nlogozzo/NickvisionTubeConverter.git#tag=${pkgver}"
        "git+https://github.com/Makman2/GCR_CMake.git")
sha256sums=("SKIP"
            "SKIP")

prepare() {
	cd "$srcdir/NickvisionTubeConverter"
    git submodule init
    git config submodule.GCR_CMake.url "${srcdir}/GCR_CMake"
    git submodule update
}

build() {
	cmake -B build -S NickvisionTubeConverter \
        -DCMAKE_INSTALL_PREFIX="/usr" \
        -DCMAKE_BUILD_TYPE="Release"
    cmake --build build
}

package() {
	DESTDIR="$pkgdir" cmake --install build
    ln -s /usr/bin/org.nickvision.tubeconverter "${pkgdir}/usr/bin/${pkgname}"
}