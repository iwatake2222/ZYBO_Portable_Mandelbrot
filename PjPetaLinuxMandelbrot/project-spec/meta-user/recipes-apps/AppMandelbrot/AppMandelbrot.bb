#
# This file is the AppMandelbrot recipe.
#

SUMMARY = "Simple AppMandelbrot application"
SECTION = "PETALINUX/apps"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

SRC_URI = "file://mandelbrotinit \
	file://mandelbrot_init_run.sh \
	file://fw_bm_mandelbrot.elf \
	file://MandelbrotServer \
	"

S = "${WORKDIR}"
INSANE_SKIP_${PN} = "arch"
inherit update-rc.d
INITSCRIPT_NAME = "mandelbrotinit"
INITSCRIPT_PARAMS = "start 99 S ."

do_install() {
    # mandelbrotinit is init daemon
    install -d ${D}${sysconfdir}/init.d
    install -m 0777 ${S}/mandelbrotinit ${D}${sysconfdir}/init.d/mandelbrotinit

    # I need to this for some reasons...
    install -d ${D}${sysconfdir}/rcS.d
    install -m 0755 ${S}/mandelbrotinit ${D}${sysconfdir}/rcS.d/S99mandelbrotinit

    # mandelbrot_init_run.sh is shell script called from mandelbrotinit
    install -d ${D}${bindir}
    install -m 0777 mandelbrot_init_run.sh ${D}${bindir}

    # baremetal firmware binary file
    install -d ${D}/lib/firmware
    install -m 0666 ${S}/fw_bm_mandelbrot.elf ${D}/lib/firmware/fw_bm_mandelbrot.elf

    # install python script and realated files in a directory
    install -d ${D}/home/root/www
    cp -r ${S}/MandelbrotServer ${D}/home/root/www
}

FILES_${PN} += "${sysconfdir}/*"
FILES_${PN} += "/lib/firmware/*.elf"
FILES_${PN} += "/home/root/www"
