#########################################
# This is the main environment script used
# by QNX CAR2
#########################################

# Some initial environment setup
: ${BASEFS=/base}
: ${IFS2=$BASEFS/etc/secondary.ifs};
: ${IFS2_LOC=/carifs2};
# IFS2_ENABLED must be set to true for secondary IFS to be mounted
: ${IFS2_ENABLED=true};
: ${CPUROOT=$BASEFS}
: ${ROOTFS=/}
export BASEFS IFS2 IFS2_LOC IFS2_ENABLED ROOTFS CPUROOT

########################################
# COMMON CONFIGURATION
########################################
export LOGDIR=/var/log
export TMPDIR=/var/tmp
export HOME=/var
export PERSISTENCE_DATA="/"
export WEBKIT_ROOT="${BASEFS?}/usr/lib/torch-webkit"
export TERM=ansi
export LOCFILE=/usr/share/locale/locale.file
export CALIBFILE=/var/etc/system/config/calib.localhost

# PATH/LD_LIBRARY_PATH ordering
LD_LIBRARY_PATH="/proc/boot:${IFS2_LOC}/lib:${IFS2_LOC}/lib/dll:${IFS2_LOC}/usr/lib:${BASEFS?}/usr/lib:${BASEFS?}/lib/dll:${BASEFS?}/lib:${BASEFS?}/lib/dll/font:/lib:/usr/lib:/lib/dll:/opt/lib:${BASEFS?}/usr/photon/lib:${BASEFS?}/qt5-5.5.1/lib:/qtcar/lib:${WEBKIT_ROOT?}"
PATH="/proc/boot:${IFS2_LOC}/bin:${IFS2_LOC}/usr/bin:${IFS2_LOC}/sbin:${IFS2_LOC}/usr/sbin:${BASEFS?}/bin:${BASEFS?}/usr/bin:${BASEFS?}/sbin:${BASEFS?}/usr/sbin:${BASEFS?}/usr/photon/bin:${BASEFS?}/scripts:/bin:/usr/bin:/sbin:/usr/sbin"
MM_INIT=${BASEFS?}/lib/dll/mmedia
export PATH LD_LIBRARY_PATH MM_INIT
	
# Flash Configuration
export LIBIMG_CFGFILE=/etc/system/config/img.conf
export FLASH_FONT_CFG_PATH=/etc/system/config/flash-font.cfg
export FONTCONFIG_FILE=/etc/fontconfig/fonts.conf
export PHKSCOPE=1
export FLASHTMP=/tmp
export PHFONT=/dev/phfont
export PHOTON=/dev/photon

#Webkit Configuration
# The next item also needs to be available to mm-renderer.
export CURLOPT_CAINFO=/opt/webkit/config/ssl/ca-bundle.crt
# Hardware scrolling on by default
export WK_USE_HW_SCROLL=2,3
# To get mm-renderer and libmd to use the same user agent as webkit
export MM_HTTP_USERAGENT="Mozilla/5.0 (PlayBook; U; RIM Tablet OS 1.0.0; en-US) AppleWebKit/534.8+ (KHTML, like Gecko) Version/0.0.1 Safari/534.8+"

#Run HMI_REMOTE to connect to mme remotely
export HMI_REMOTE="FALSE"
export HMI_VOLUME_SCALE=50
export PERSISTENCE_DATA=/

# The min dates are for our SSL Cert
export MIN_VALID_DATE=200905010000.00
export MIN_VALID_DATE_SEC=1241136000
export RTC_CLOCK_TYPE=""
export RTC_OPTS=""
export RTC_CLOCK_OPTS=""

# Set the time zone (see QNX "Setting the time zone" documentation)
#export TZ=EST5EDT
export TZ=CST-8

# Set the list of apps that should not be shutdown when running a software self update
# Note that the list of applications that are left running should be minimal to avoid some
# other process modifying the file system while the update is being applied.
export UPDATE_APPS_TO_KEEP="ksh sh devc-seromap pipe screen slogger io-usb devb-umass display_image swud"

DEFAULT_ACCOUNT="/accounts/1000"
MEDIA_DIR=$DEFAULT_ACCOUNT/shared

# Apple carplay stuff
export CARPLAY_IFACE_NAME=carplay
export CARPLAY_IFACE_NUM=0
export CARPLAY_IFACE=${CARPLAY_IFACE_NAME}${CARPLAY_IFACE_NUM}
export MDNS_DIRECTLINK_IFACE=${CARPLAY_IFACE}

alias dir='ls -altF'
alias which='command -v'

#Allow users to override any of the default settings in this file.
for f in \
	"${BASEFS?}/scripts/env.override" \
	"${BASEFS?}/scripts/env.override.board" \
	"${BASEFS?}/scripts/env.override.variant" \
	"${ROOTFS?}/env.user" \
	"/proc/boot/env.override.diskconf" \
	;
do
	if [ -e "$f" ]
	then
		. "$f"
	fi
done
unset f ||:
