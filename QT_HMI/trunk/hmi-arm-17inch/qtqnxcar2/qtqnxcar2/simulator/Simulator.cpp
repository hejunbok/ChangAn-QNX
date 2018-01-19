#include "Simulator.h"

#include <qpps/simulator.h>

void QnxCar2Simulator::initDemoData()
{
    QPps::Simulator * const s = QPps::Simulator::self();

    s->insertAttribute(QStringLiteral("/pps/qnxcar/profile/user"), QStringLiteral("id"), QByteArrayLiteral("1"), QByteArrayLiteral("n"));
    s->insertAttribute(QStringLiteral("/pps/qnxcar/profile/user"), QStringLiteral("fullName"), QByteArrayLiteral("John Doe"), QByteArrayLiteral("s"));
    s->insertAttribute(QStringLiteral("/pps/qnxcar/profile/user"), QStringLiteral("avatar"), QByteArrayLiteral("male1"), QByteArrayLiteral("s"));

    s->insertAttribute(QStringLiteral("/pps/qnxcar/profile/theme"), QStringLiteral("nightMode"), QByteArrayLiteral("false"), QByteArrayLiteral("b"));
    s->insertAttribute(QStringLiteral("/pps/qnxcar/profile/theme"), QStringLiteral("theme"), QByteArrayLiteral("default"), QByteArrayLiteral("s"));

    QPps::Simulator::self()->insertAttribute(QStringLiteral("/pps/qnxcar/themes"), QStringLiteral("default"), QByteArrayLiteral("{\"ppsName\":\"default\",\"title\":\"Midnight blue\",\"themePackageName\":\"default\",\"packageDate\":\"2012-04-23\"}"), QByteArrayLiteral("json"));
    QPps::Simulator::self()->insertAttribute(QStringLiteral("/pps/qnxcar/themes"), QStringLiteral("jeep"), QByteArrayLiteral("{\"ppsName\":\"jeep\",\"title\":\"Titanium\",\"themePackageName\":\"jeep\",\"packageDate\":\"2012-04-13\"}"), QByteArrayLiteral("json"));
    QPps::Simulator::self()->insertAttribute(QStringLiteral("/pps/qnxcar/themes"), QStringLiteral("bw"), QByteArrayLiteral("{\"ppsName\":\"bw\",\"title\":\"BW Theme\",\"themePackageName\":\"bw\",\"packageDate\":\"2013-05-21\"}"), QByteArrayLiteral("json"));

    s->insertAttribute(QStringLiteral("/pps/qnxcar/hvac"), QStringLiteral("airRecirculation_all"), QByteArrayLiteral("false"), QByteArrayLiteral("b"));
    s->insertAttribute(QStringLiteral("/pps/qnxcar/hvac"), QStringLiteral("airConditioning_all"), QByteArrayLiteral("false"), QByteArrayLiteral("b"));
    s->insertAttribute(QStringLiteral("/pps/qnxcar/hvac"), QStringLiteral("fanDirection_row1left"), QByteArrayLiteral("faceAndFeet"), QByteArrayLiteral(""));
    s->insertAttribute(QStringLiteral("/pps/qnxcar/hvac"), QStringLiteral("fanDirection_row1right"), QByteArrayLiteral("defrost"), QByteArrayLiteral(""));
    s->insertAttribute(QStringLiteral("/pps/qnxcar/hvac"), QStringLiteral("fanSpeed_row1left"), QByteArrayLiteral("1"), QByteArrayLiteral("n"));
    s->insertAttribute(QStringLiteral("/pps/qnxcar/hvac"), QStringLiteral("fanSpeed_row1right"), QByteArrayLiteral("3"), QByteArrayLiteral("n"));
    s->insertAttribute(QStringLiteral("/pps/qnxcar/hvac"), QStringLiteral("temperature_row1left"), QByteArrayLiteral("21"), QByteArrayLiteral("n"));
    s->insertAttribute(QStringLiteral("/pps/qnxcar/hvac"), QStringLiteral("temperature_row1right"), QByteArrayLiteral("19"), QByteArrayLiteral("n"));
    s->insertAttribute(QStringLiteral("/pps/qnxcar/hvac"), QStringLiteral("heatedSeat_row1left"), QByteArrayLiteral("2"), QByteArrayLiteral("n"));
    s->insertAttribute(QStringLiteral("/pps/qnxcar/hvac"), QStringLiteral("heatedSeat_row1right"), QByteArrayLiteral("0"), QByteArrayLiteral("n"));
    s->insertAttribute(QStringLiteral("/pps/qnxcar/hvac"), QStringLiteral("defrost_all"), QByteArrayLiteral("true"), QByteArrayLiteral("b"));
    s->insertAttribute(QStringLiteral("/pps/qnxcar/hvac"), QStringLiteral("zoneLink_all"), QByteArrayLiteral("false"), QByteArrayLiteral("b"));

    s->insertObject(QStringLiteral("/pps/qnxcar/navigation/control"));

    s->insertAttribute(QStringLiteral("/pps/system/navigator/applications/applications"), QStringLiteral("AppSection.testDev_AppSection_2fc201a4"), QByteArrayLiteral("{86x86}native/default-icon.png,AppSection,system,,auto,,"), QByteArray());
    s->insertAttribute(QStringLiteral("/pps/system/navigator/applications/applications"), QStringLiteral("BestParking.testDev_BestParkinga4a73514"), QByteArrayLiteral("{48x48}native/appicon.png,BestParking,internet,,auto,,"), QByteArray());
    s->insertAttribute(QStringLiteral("/pps/system/navigator/applications/applications"), QStringLiteral("Calendar.testDev_Calendar___f9395f5e"), QByteArrayLiteral("{86x86}native/icon.png,Calendar,bridge,,auto,,"), QByteArray());
    s->insertAttribute(QStringLiteral("/pps/system/navigator/applications/applications"), QStringLiteral("Communication.testDev_mmunicationf1e9ffb6"), QByteArrayLiteral("{60x60}native/icon.png,Communication,system,,auto,,"), QByteArray());
    s->insertAttribute(QStringLiteral("/pps/system/navigator/applications/applications"), QStringLiteral("Contacts.testDev_Contacts___e207c473"), QByteArrayLiteral("{86x86}native/icon.png,Contacts,bridge,,auto,,"), QByteArray());
    s->insertAttribute(QStringLiteral("/pps/system/navigator/applications/applications"), QStringLiteral("HelloWebWorks.testDev_lloWebWorks1fa80f60"), QByteArrayLiteral("{60x60}native/icon.png,HelloWebWorks,,,auto,,"), QByteArray());
    s->insertAttribute(QStringLiteral("/pps/system/navigator/applications/applications"), QStringLiteral("Home.testDev_Home_______2268ff__"), QByteArrayLiteral("{60x60}native/icon.png,Home,system,,auto,,"), QByteArray());
    s->insertAttribute(QStringLiteral("/pps/system/navigator/applications/applications"), QStringLiteral("Map.testDev_Map________12d3c___"), QByteArrayLiteral("{86x86}native/icon.png,Map,internet,,auto,,"), QByteArray());
    s->insertAttribute(QStringLiteral("/pps/system/navigator/applications/applications"), QStringLiteral("MediaPlayer.testDev_MediaPlayer49ba23c5"), QByteArrayLiteral("{60x60}native/icon.png,MediaPlayer,system,,auto,,"), QByteArray());
    s->insertAttribute(QStringLiteral("/pps/system/navigator/applications/applications"), QStringLiteral("Memopad.testDev_Memopad____9bcd70f9"), QByteArrayLiteral("{86x86}native/icon.png,Memopad,bridge,,auto,,"), QByteArray());
    s->insertAttribute(QStringLiteral("/pps/system/navigator/applications/applications"), QStringLiteral("Messages.testDev_Messages___e84f656c"), QByteArrayLiteral("{86x86}native/icon.png,Messages,bridge,,auto,,"), QByteArray());
    s->insertAttribute(QStringLiteral("/pps/system/navigator/applications/applications"), QStringLiteral("Navigator.testDev_Navigator__a4514a37"), QByteArrayLiteral("{60x60}native/icon.png,Navigator,system,,auto,,"), QByteArray());
    s->insertAttribute(QStringLiteral("/pps/system/navigator/applications/applications"), QStringLiteral("Pandora.testDev_Pandora____33b5d5f7"), QByteArrayLiteral("{512x512}native/icon.png,Pandora,media,,auto,,"), QByteArray());
    s->insertAttribute(QStringLiteral("/pps/system/navigator/applications/applications"), QStringLiteral("PeaksAndValleys.testDev_sAndValleys6bb91d91"), QByteArrayLiteral("{86x86}native/icon.png,PeaksAndValleys,games,,auto,,"), QByteArray());
    s->insertAttribute(QStringLiteral("/pps/system/navigator/applications/applications"), QStringLiteral("Settings.testDev_Settings___595d2043"), QByteArrayLiteral("{86x86}native/icon.png,Settings,vehicle,,auto,,"), QByteArray());
    s->insertAttribute(QStringLiteral("/pps/system/navigator/applications/applications"), QStringLiteral("Shutdown.testDev_Shutdown___f9c26a76"), QByteArrayLiteral("{96x96}native/icon.png,Shutdown,vehicle,,auto,,"), QByteArray());
    s->insertAttribute(QStringLiteral("/pps/system/navigator/applications/applications"), QStringLiteral("Slacker.testDev_Slacker____e476201d"), QByteArrayLiteral("{90x90}native/icon.png,Slacker Radio,media,,auto,,"), QByteArray());
    s->insertAttribute(QStringLiteral("/pps/system/navigator/applications/applications"), QStringLiteral("Status.testDev_Status_____9432bc12"), QByteArrayLiteral("{60x60}native/icon.png,Status,system,,auto,,"), QByteArray());
    s->insertAttribute(QStringLiteral("/pps/system/navigator/applications/applications"), QStringLiteral("Weather.testDev_Weather____ac24cfd4"), QByteArrayLiteral("{86x86}native/icon.png,Weather,internet,,auto,,"), QByteArray());
    s->insertAttribute(QStringLiteral("/pps/system/navigator/applications/applications"), QStringLiteral("WeatherNetwork.testDev_therNetworke376bba_"), QByteArrayLiteral("{84x84}native/icon.png,WeatherNetwork,internet,,auto,,"), QByteArray());
    s->insertAttribute(QStringLiteral("/pps/system/navigator/applications/applications"), QStringLiteral("carcontrol.testDev_carcontrol_21522f09"), QByteArrayLiteral("{86x86}native/default-icon.png,CarControl,system,,auto,,"), QByteArray());
    s->insertAttribute(QStringLiteral("/pps/system/navigator/applications/applications"), QStringLiteral("com.Scout.testRel_com_Scout__3a5fd831"), QByteArrayLiteral("native/icon.png,Scout,core.games,native/splash.png,landscape,,"), QByteArray());
    s->insertAttribute(QStringLiteral("/pps/system/navigator/applications/applications"), QStringLiteral("htmlgears.testDev_htmlgears__9202e3f9"), QByteArrayLiteral("{60x60}native/icon.png,HTMLGears,system,,auto,,"), QByteArray());
    s->insertAttribute(QStringLiteral("/pps/system/navigator/applications/applications"), QStringLiteral("navigation.testDev_navigation_6f060a14"), QByteArrayLiteral("{86x86}native/default-icon.png,Navigation,system,,auto,,"), QByteArray());
    s->insertAttribute(QStringLiteral("/pps/system/navigator/applications/applications"), QStringLiteral("sys.browser.new.testRel_browser_new77e89439"), QByteArrayLiteral("native/browserIcon.png,Browser,internet,,auto,,"), QByteArray());
    s->insertAttribute(QStringLiteral("/pps/system/navigator/applications/applications"), QStringLiteral("tunneltilt.testDev_tunneltilt_84be6f25"), QByteArrayLiteral("{86x86}native/data/images/tunnelTiltIcon.png,TunnelTilt,games,,auto,,"), QByteArray());

    s->insertAttribute(QStringLiteral("/pps/system/navigator/command"), QStringLiteral("navigation"), QByteArrayLiteral("{\"action\":\"pause\""), QByteArrayLiteral("json"));
    s->insertAttribute(QStringLiteral("/pps/system/navigator/command"), QStringLiteral("MediaPlayer"), QByteArrayLiteral("{\"action\":\"pause\""), QByteArrayLiteral("json"));
    s->insertAttribute(QStringLiteral("/pps/system/navigator/command"), QStringLiteral("AppSection"), QByteArrayLiteral("{\"action\":\"pause\""), QByteArrayLiteral("json"));
    s->insertAttribute(QStringLiteral("/pps/system/navigator/command"), QStringLiteral("Home"), QByteArrayLiteral("{\"action\":\"resume\""), QByteArrayLiteral("json"));

    s->insertAttribute(QStringLiteral("/pps/qnxcar/sensors"), QStringLiteral("brakeFluidLevel"), QByteArrayLiteral("90"), QByteArrayLiteral("n"));
    s->insertAttribute(QStringLiteral("/pps/qnxcar/sensors"), QStringLiteral("brakeAbsEnabled"), QByteArrayLiteral("true"), QByteArrayLiteral("b"));
    s->insertAttribute(QStringLiteral("/pps/qnxcar/sensors"), QStringLiteral("brakeAbsFrontLeft"), QByteArrayLiteral("true"), QByteArrayLiteral("b"));
    s->insertAttribute(QStringLiteral("/pps/qnxcar/sensors"), QStringLiteral("brakeAbsFrontRight"), QByteArrayLiteral("true"), QByteArrayLiteral("b"));
    s->insertAttribute(QStringLiteral("/pps/qnxcar/sensors"), QStringLiteral("brakeAbsRearLeft"), QByteArrayLiteral("true"), QByteArrayLiteral("b"));
    s->insertAttribute(QStringLiteral("/pps/qnxcar/sensors"), QStringLiteral("brakeAbsRearRight"), QByteArrayLiteral("true"), QByteArrayLiteral("b"));
    s->insertAttribute(QStringLiteral("/pps/qnxcar/sensors"), QStringLiteral("brakePadWearFrontLeft"), QByteArrayLiteral("100"), QByteArrayLiteral("n"));
    s->insertAttribute(QStringLiteral("/pps/qnxcar/sensors"), QStringLiteral("brakePadWearFrontRight"), QByteArrayLiteral("100"), QByteArrayLiteral("n"));
    s->insertAttribute(QStringLiteral("/pps/qnxcar/sensors"), QStringLiteral("brakePadWearRearLeft"), QByteArrayLiteral("50"), QByteArrayLiteral("n"));
    s->insertAttribute(QStringLiteral("/pps/qnxcar/sensors"), QStringLiteral("brakePadWearRearRight"), QByteArrayLiteral("65"), QByteArrayLiteral("n"));
    s->insertAttribute(QStringLiteral("/pps/qnxcar/sensors"), QStringLiteral("coolantLevel"), QByteArrayLiteral("100"), QByteArrayLiteral("n"));
    s->insertAttribute(QStringLiteral("/pps/qnxcar/sensors"), QStringLiteral("engineOilLevel"), QByteArrayLiteral("95"), QByteArrayLiteral("n"));
    s->insertAttribute(QStringLiteral("/pps/qnxcar/sensors"), QStringLiteral("engineOilPressure"), QByteArrayLiteral("100"), QByteArrayLiteral("n"));
    s->insertAttribute(QStringLiteral("/pps/qnxcar/sensors"), QStringLiteral("rpm"), QByteArrayLiteral("2800"), QByteArrayLiteral("n"));
    s->insertAttribute(QStringLiteral("/pps/qnxcar/sensors"), QStringLiteral("fuelLevel"), QByteArrayLiteral("75"), QByteArrayLiteral("n"));
    s->insertAttribute(QStringLiteral("/pps/qnxcar/sensors"), QStringLiteral("lightHeadLeft"), QByteArrayLiteral("true"), QByteArrayLiteral("b"));
    s->insertAttribute(QStringLiteral("/pps/qnxcar/sensors"), QStringLiteral("lightHeadRight"), QByteArrayLiteral("true"), QByteArrayLiteral("b"));
    s->insertAttribute(QStringLiteral("/pps/qnxcar/sensors"), QStringLiteral("lightTailLeft"), QByteArrayLiteral("false"), QByteArrayLiteral("b"));
    s->insertAttribute(QStringLiteral("/pps/qnxcar/sensors"), QStringLiteral("lightTailRight"), QByteArrayLiteral("true"), QByteArrayLiteral("b"));
    s->insertAttribute(QStringLiteral("/pps/qnxcar/sensors"), QStringLiteral("speed"), QByteArrayLiteral("0"), QByteArrayLiteral("n"));
    s->insertAttribute(QStringLiteral("/pps/qnxcar/sensors"), QStringLiteral("tirePressureFrontLeft"), QByteArrayLiteral("31"), QByteArrayLiteral("n"));
    s->insertAttribute(QStringLiteral("/pps/qnxcar/sensors"), QStringLiteral("tirePressureFrontRight"), QByteArrayLiteral("31"), QByteArrayLiteral("n"));
    s->insertAttribute(QStringLiteral("/pps/qnxcar/sensors"), QStringLiteral("tirePressureRearLeft"), QByteArrayLiteral("25"), QByteArrayLiteral("n"));
    s->insertAttribute(QStringLiteral("/pps/qnxcar/sensors"), QStringLiteral("tirePressureRearRight"), QByteArrayLiteral("32"), QByteArrayLiteral("n"));
    s->insertAttribute(QStringLiteral("/pps/qnxcar/sensors"), QStringLiteral("tireWearFrontLeft"), QByteArrayLiteral("90"), QByteArrayLiteral("n"));
    s->insertAttribute(QStringLiteral("/pps/qnxcar/sensors"), QStringLiteral("tireWearFrontRight"), QByteArrayLiteral("90"), QByteArrayLiteral("n"));
    s->insertAttribute(QStringLiteral("/pps/qnxcar/sensors"), QStringLiteral("tireWearRearLeft"), QByteArrayLiteral("70"), QByteArrayLiteral("n"));
    s->insertAttribute(QStringLiteral("/pps/qnxcar/sensors"), QStringLiteral("tireWearRearRight"), QByteArrayLiteral("70"), QByteArrayLiteral("n"));
    s->insertAttribute(QStringLiteral("/pps/qnxcar/sensors"), QStringLiteral("transmissionFluidLevel"), QByteArrayLiteral("85"), QByteArrayLiteral("n"));
    s->insertAttribute(QStringLiteral("/pps/qnxcar/sensors"), QStringLiteral("transmissionClutchWear"), QByteArrayLiteral("70"), QByteArrayLiteral("n"));
    s->insertAttribute(QStringLiteral("/pps/qnxcar/sensors"), QStringLiteral("transmissionGear"), QByteArrayLiteral("n"), QByteArray());
    s->insertAttribute(QStringLiteral("/pps/qnxcar/sensors"), QStringLiteral("transmissionFluidTemperature"), QByteArrayLiteral("185"), QByteArrayLiteral("n"));
    s->insertAttribute(QStringLiteral("/pps/qnxcar/sensors"), QStringLiteral("washerFluidLevel"), QByteArrayLiteral("20"), QByteArrayLiteral("n"));

    s->insertAttribute(QStringLiteral("/pps/services/app-launcher"), QStringLiteral("app_list"), QByteArrayLiteral("[\"AppSection\",\"BestParking\",\"Calendar\",\"Communication\",\"Contacts\",\"HelloWebWorks\",\"Home\",\"Map\",\"MediaPlayer\",\"Memopad\",\"Messages\",\"Navigator\",\"Pandora\",\"PeaksAndValleys\",\"Settings\",\"Shutdown\",\"Slacker Radio\",\"Status\",\"Weather\",\"WeatherNetwork\",\"CarControl\",\"Scout\",\"HTMLGears\",\"Navigation\",\"Browser\",\"TunnelTilt\"]"), QByteArrayLiteral("json"));

    s->insertAttribute(QStringLiteral("/pps/services/launcher/control"), QStringLiteral("dummy"), QByteArrayLiteral("<empty>"), QByteArray());

    s->insertAttribute(QStringLiteral("/pps/services/audio/control"), QStringLiteral("dummy"), QByteArrayLiteral("<empty>"), QByteArray());
    s->insertAttribute(QStringLiteral("/pps/services/audio/status"), QStringLiteral("output.speaker.volume"), QByteArrayLiteral("60.000000"), QByteArray("n"));
    s->insertAttribute(QStringLiteral("/pps/services/audio/status"), QStringLiteral("output.handset.volume"), QByteArrayLiteral("60.000000"), QByteArray("n"));
    s->insertAttribute(QStringLiteral("/pps/services/audio/status"), QStringLiteral("output.available"), QByteArrayLiteral("speaker"), QByteArray());

    s->insertAttribute(QStringLiteral("/pps/services/audio/mixer"), QStringLiteral("balance"), QByteArrayLiteral("50"), QByteArray("n"));
    s->insertAttribute(QStringLiteral("/pps/services/audio/mixer"), QStringLiteral("bass"), QByteArrayLiteral("74.02597402597402"), QByteArray("n"));
    s->insertAttribute(QStringLiteral("/pps/services/audio/mixer"), QStringLiteral("fade"), QByteArrayLiteral("42.2077922077922"), QByteArray("n"));
    s->insertAttribute(QStringLiteral("/pps/services/audio/mixer"), QStringLiteral("mid"), QByteArrayLiteral("92.20779220779221"), QByteArray("n"));
    s->insertAttribute(QStringLiteral("/pps/services/audio/mixer"), QStringLiteral("treble"), QByteArrayLiteral("78.57142857142857"), QByteArray("n"));

    s->insertObject(QStringLiteral("/pps/services/mm-control/control"));

    s->insertAttribute(QStringLiteral("/pps/services/mm-control/mpaudio/status"), QStringLiteral("db_timestamp"), QByteArrayLiteral("5980619000000"), QByteArray("n"));
    s->insertAttribute(QStringLiteral("/pps/services/mm-control/mpaudio/status"), QStringLiteral("dbpath"), QByteArrayLiteral("/dev/qdb/mme"), QByteArray());
    s->insertAttribute(QStringLiteral("/pps/services/mm-control/mpaudio/status"), QStringLiteral("fid"), QByteArrayLiteral("17"), QByteArray("n"));
    s->insertAttribute(QStringLiteral("/pps/services/mm-control/mpaudio/status"), QStringLiteral("media_source"), QByteArrayLiteral("dbmme"), QByteArray());
    s->insertAttribute(QStringLiteral("/pps/services/mm-control/mpaudio/status"), QStringLiteral("position"), QByteArrayLiteral("239246"), QByteArray("n"));
    s->insertAttribute(QStringLiteral("/pps/services/mm-control/mpaudio/status"), QStringLiteral("speed"), QByteArrayLiteral("1000"), QByteArray());
    s->insertAttribute(QStringLiteral("/pps/services/mm-control/mpaudio/status"), QStringLiteral("state"), QByteArrayLiteral("STOPPED"), QByteArray());
    s->insertAttribute(QStringLiteral("/pps/services/mm-control/mpaudio/status"), QStringLiteral("trkid"), QByteArrayLiteral("0"), QByteArray("n"));
    s->insertAttribute(QStringLiteral("/pps/services/mm-control/mpaudio/status"), QStringLiteral("trksession"), QByteArrayLiteral("7"), QByteArray("n"));

    s->insertObject(QStringLiteral("/pps/services/multimedia/renderer/control"));

    s->insertAttribute(QStringLiteral("/pps/qnxcar/system/settings"), QStringLiteral("appSection_profile"), QByteArrayLiteral("high"), QByteArray());
    s->insertAttribute(QStringLiteral("/pps/qnxcar/system/settings"), QStringLiteral("carControl_profile"), QByteArrayLiteral("high"), QByteArray());
    s->insertAttribute(QStringLiteral("/pps/qnxcar/system/settings"), QStringLiteral("communication_profile"), QByteArrayLiteral("high"), QByteArray());
    s->insertAttribute(QStringLiteral("/pps/qnxcar/system/settings"), QStringLiteral("mediaPlayer_profile"), QByteArrayLiteral("high"), QByteArray());

    s->insertAttribute(QStringLiteral("/pps/services/asr/control"), QStringLiteral("state"), QByteArrayLiteral("idle"), QByteArray());

    s->insertObject(QStringLiteral("/pps/services/bluetooth/messages/control"));

    s->insertAttribute(QStringLiteral("/pps/services/bluetooth/settings"), QStringLiteral("accessibility"), QByteArrayLiteral("3"), QByteArrayLiteral("n"));
    s->insertAttribute(QStringLiteral("/pps/services/bluetooth/settings"), QStringLiteral("active_connections"), QByteArrayLiteral("false"), QByteArrayLiteral("b"));
    s->insertAttribute(QStringLiteral("/pps/services/bluetooth/settings"), QStringLiteral("btaddr"), QByteArrayLiteral("C8:3C:12:00:14:57"), QByteArray());
    s->insertAttribute(QStringLiteral("/pps/services/bluetooth/settings"), QStringLiteral("enabled"), QByteArrayLiteral("true"), QByteArrayLiteral("b"));
    s->insertAttribute(QStringLiteral("/pps/services/bluetooth/settings"), QStringLiteral("name"), QByteArrayLiteral("panda"), QByteArray());

    s->insertAttribute(QStringLiteral("/pps/services/bluetooth/services"), QStringLiteral("hfp"), QByteArray(), QByteArray());
    s->insertAttribute(QStringLiteral("/pps/services/bluetooth/services"), QStringLiteral("map"), QByteArray(), QByteArray());
    s->insertAttribute(QStringLiteral("/pps/services/bluetooth/services"), QStringLiteral("pbap"), QByteArray(), QByteArray());
    s->insertAttribute(QStringLiteral("/pps/services/bluetooth/services"), QStringLiteral("spp"), QByteArray(), QByteArray());

    s->insertAttribute(QStringLiteral("/pps/services/bluetooth/phonebook/status"), QStringLiteral("[n]device"), QByteArray("AC:FC:14:CE:E5:5D"), QByteArray());
    s->insertAttribute(QStringLiteral("/pps/services/bluetooth/phonebook/status"), QStringLiteral("[n]state"), QByteArray("CONNECTED"), QByteArray());
    s->insertAttribute(QStringLiteral("/pps/services/bluetooth/phonebook/status"), QStringLiteral("[n]status"), QByteArrayLiteral("COMPLETE"), QByteArray());

    s->insertAttribute(QStringLiteral("/pps/services/bluetooth/messages/status"), QStringLiteral("[n]device"), QByteArray("AC:FC:14:CE:E5:5D"), QByteArray());
    s->insertAttribute(QStringLiteral("/pps/services/bluetooth/messages/status"), QStringLiteral("[n]state"), QByteArray("CONNECTED"), QByteArray());
    s->insertAttribute(QStringLiteral("/pps/services/bluetooth/messages/status"), QStringLiteral("[n]status"), QByteArrayLiteral("COMPLETE"), QByteArray());

    s->insertAttribute(QStringLiteral("/pps/services/bluetooth/remote_devices/AC:FC:14:CE:E5:5D"), QStringLiteral("cod"), QByteArrayLiteral("0x005a020c"), QByteArray());
    s->insertAttribute(QStringLiteral("/pps/services/bluetooth/remote_devices/AC:FC:14:CE:E5:5D"), QStringLiteral("in_range"), QByteArrayLiteral("false"), QByteArrayLiteral("b"));
    s->insertAttribute(QStringLiteral("/pps/services/bluetooth/remote_devices/AC:FC:14:CE:E5:5D"), QStringLiteral("name"), QByteArrayLiteral("Device1"), QByteArray());
    s->insertAttribute(QStringLiteral("/pps/services/bluetooth/remote_devices/AC:FC:14:CE:E5:5D"), QStringLiteral("rssi"), QByteArrayLiteral("0x00"), QByteArray());

    s->insertAttribute(QStringLiteral("/pps/services/bluetooth/paired_devices/AC:FC:14:CE:E5:5D"), QStringLiteral("cod"), QByteArrayLiteral("0x005a020c"), QByteArray());
    s->insertAttribute(QStringLiteral("/pps/services/bluetooth/paired_devices/AC:FC:14:CE:E5:5D"), QStringLiteral("in_range"), QByteArrayLiteral("false"), QByteArrayLiteral("b"));
    s->insertAttribute(QStringLiteral("/pps/services/bluetooth/paired_devices/AC:FC:14:CE:E5:5D"), QStringLiteral("name"), QByteArrayLiteral("Device1"), QByteArray());
    s->insertAttribute(QStringLiteral("/pps/services/bluetooth/paired_devices/AC:FC:14:CE:E5:5D"), QStringLiteral("paired"), QByteArrayLiteral("true"), QByteArrayLiteral("b"));
    s->insertAttribute(QStringLiteral("/pps/services/bluetooth/paired_devices/AC:FC:14:CE:E5:5D"), QStringLiteral("rssi"), QByteArrayLiteral("0x00"), QByteArray());
    s->insertAttribute(QStringLiteral("/pps/services/bluetooth/paired_devices/AC:FC:14:CE:E5:5E"), QStringLiteral("cod"), QByteArrayLiteral("0x005a020d"), QByteArray());
    s->insertAttribute(QStringLiteral("/pps/services/bluetooth/paired_devices/AC:FC:14:CE:E5:5E"), QStringLiteral("in_range"), QByteArrayLiteral("true"), QByteArrayLiteral("b"));
    s->insertAttribute(QStringLiteral("/pps/services/bluetooth/paired_devices/AC:FC:14:CE:E5:5E"), QStringLiteral("name"), QByteArrayLiteral("Device2"), QByteArray());
    s->insertAttribute(QStringLiteral("/pps/services/bluetooth/paired_devices/AC:FC:14:CE:E5:5E"), QStringLiteral("paired"), QByteArrayLiteral("true"), QByteArrayLiteral("b"));
    s->insertAttribute(QStringLiteral("/pps/services/bluetooth/paired_devices/AC:FC:14:CE:E5:5E"), QStringLiteral("rssi"), QByteArrayLiteral("0x01"), QByteArray());

    s->insertAttribute(QStringLiteral("/pps/radio/control"), QStringLiteral("dummy"), QByteArray(), QByteArray());

    s->insertAttribute(QStringLiteral("/pps/radio/status"), QStringLiteral("am"), QByteArrayLiteral("{\"presets\":[880,910,950,1020,1220,1430],\"station\":1680}"), QByteArrayLiteral("json"));
    s->insertAttribute(QStringLiteral("/pps/radio/status"), QStringLiteral("fm"), QByteArrayLiteral("{\"presets\":[87.5,88.5,99.9,105.3,106.9,107.1],\"station\":96.5}"), QByteArrayLiteral("json"));
    s->insertAttribute(QStringLiteral("/pps/radio/status"), QStringLiteral("artist"), QByteArrayLiteral("Bjork"), QByteArray());
    s->insertAttribute(QStringLiteral("/pps/radio/status"), QStringLiteral("genre"), QByteArrayLiteral("News & Entertainment"), QByteArray());
    s->insertAttribute(QStringLiteral("/pps/radio/status"), QStringLiteral("song"), QByteArrayLiteral("All is Full of Love"), QByteArray());
    s->insertAttribute(QStringLiteral("/pps/radio/status"), QStringLiteral("station"), QByteArrayLiteral("CBC Radio 2"), QByteArray());
    s->insertAttribute(QStringLiteral("/pps/radio/status"), QStringLiteral("hd"), QByteArrayLiteral("true"), QByteArrayLiteral("b"));
    s->insertAttribute(QStringLiteral("/pps/radio/status"), QStringLiteral("tuner"), QByteArrayLiteral("fm"), QByteArray());

    s->insertAttribute(QStringLiteral("/pps/radio/tuners"), QStringLiteral("am"), QByteArrayLiteral("{\"type\":\"analog\",\"rangeMin\":880,\"rangeMax\":1710,\"rangeStep\":10}"), QByteArrayLiteral("json"));
    s->insertAttribute(QStringLiteral("/pps/radio/tuners"), QStringLiteral("fm"), QByteArrayLiteral("{\"type\":\"analog\",\"rangeMin\":87.5,\"rangeMax\":107.1,\"rangeStep\":0.2}"), QByteArrayLiteral("json"));

    s->insertAttribute(QStringLiteral("/pps/services/handsfree/control"), QStringLiteral("[n]cmd_data"), QByteArray(), QByteArray());
    s->insertAttribute(QStringLiteral("/pps/services/handsfree/control"), QStringLiteral("[n]command"), QByteArray(), QByteArray());
    s->insertAttribute(QStringLiteral("/pps/services/handsfree/status"), QStringLiteral("[n]cmd_status"), QByteArrayLiteral("NA"), QByteArray());
    s->insertAttribute(QStringLiteral("/pps/services/handsfree/status"), QStringLiteral("[n]state"), QByteArrayLiteral("HFP_INITIALIZED"), QByteArray());
    s->insertAttribute(QStringLiteral("/pps/services/handsfree/status"), QStringLiteral("[n]state_param"), QByteArrayLiteral("NA"), QByteArray());

    s->insertAttribute(QStringLiteral("/pps/services/mm-detect/status"), QStringLiteral("dbmme"), QByteArrayLiteral("{\"dbpath\":\"/dev/qdb/mme\",\"mount\":\"/accounts/1000/shared/\",\"name\":\"Juke Box\",\"fs_type\":\"qnx\",\"device_type\":\"hdd\",\"image_path\":\"/apps/mediasources/imagecache/dbmme\",\"synched\":true}"), QByteArrayLiteral("json"));
    s->insertAttribute(QStringLiteral("/pps/services/mm-detect/status"), QStringLiteral("myusb"), QByteArrayLiteral("{\"dbpath\":\"/dev/qdb/myusb\",\"mount\":\"/fs/usb0\",\"name\":\"My USB\",\"fs_type\":\"dos (fat32)\",\"device_type\":\"usb\",\"image_path\":\"/apps/mediasources/imagecache/myusb\",\"synched\":true}"), QByteArrayLiteral("json"));
    s->insertAttribute(QStringLiteral("/pps/services/mm-detect/status"), QStringLiteral("myotherusb"), QByteArrayLiteral("{\"dbpath\":\"/dev/qdb/myotherusb\",\"mount\":\"/fs/usb1\",\"name\":\"My other USB\",\"fs_type\":\"dos (fat32)\",\"device_type\":\"usb\",\"image_path\":\"/apps/mediasources/imagecache/myotherusb\",\"synched\":true}"), QByteArrayLiteral("json"));
    s->insertAttribute(QStringLiteral("/pps/services/mm-detect/status"), QStringLiteral("myotherusb2"), QByteArrayLiteral("{\"dbpath\":\"/dev/qdb/myotherusb2\",\"mount\":\"/fs/usb1\",\"name\":\"My other USB2\",\"fs_type\":\"dos (fat32)\",\"device_type\":\"usb\",\"image_path\":\"/apps/mediasources/imagecache/myotherusb\",\"synched\":true}"), QByteArrayLiteral("json"));
    s->insertAttribute(QStringLiteral("/pps/services/mm-detect/status"), QStringLiteral("myotherusb3"), QByteArrayLiteral("{\"dbpath\":\"/dev/qdb/myotherusb3\",\"mount\":\"/fs/usb1\",\"name\":\"My other USB3\",\"fs_type\":\"dos (fat32)\",\"device_type\":\"usb\",\"image_path\":\"/apps/mediasources/imagecache/myotherusb\",\"synched\":true}"), QByteArrayLiteral("json"));
    s->insertAttribute(QStringLiteral("/pps/services/mm-detect/status"), QStringLiteral("myotherusb4"), QByteArrayLiteral("{\"dbpath\":\"/dev/qdb/myotherusb3\",\"mount\":\"/fs/usb1\",\"name\":\"My other USB4\",\"fs_type\":\"dos (fat32)\",\"device_type\":\"usb\",\"image_path\":\"/apps/mediasources/imagecache/myotherusb\",\"synched\":true}"), QByteArrayLiteral("json"));
    s->insertAttribute(QStringLiteral("/pps/services/mm-detect/status"), QStringLiteral("myotherusb5"), QByteArrayLiteral("{\"dbpath\":\"/dev/qdb/myotherusb3\",\"mount\":\"/fs/usb1\",\"name\":\"My other USB5\",\"fs_type\":\"dos (fat32)\",\"device_type\":\"usb\",\"image_path\":\"/apps/mediasources/imagecache/myotherusb\",\"synched\":true}"), QByteArrayLiteral("json"));
    s->insertAttribute(QStringLiteral("/pps/services/mm-detect/status"), QStringLiteral("myotherusb6"), QByteArrayLiteral("{\"dbpath\":\"/dev/qdb/myotherusb3\",\"mount\":\"/fs/usb1\",\"name\":\"My other USB6\",\"fs_type\":\"dos (fat32)\",\"device_type\":\"usb\",\"image_path\":\"/apps/mediasources/imagecache/myotherusb\",\"synched\":true}"), QByteArrayLiteral("json"));
    s->insertAttribute(QStringLiteral("/pps/services/mm-detect/status"), QStringLiteral("myotherusb7"), QByteArrayLiteral("{\"dbpath\":\"/dev/qdb/myotherusb3\",\"mount\":\"/fs/usb1\",\"name\":\"My other USB7\",\"fs_type\":\"dos (fat32)\",\"device_type\":\"usb\",\"image_path\":\"/apps/mediasources/imagecache/myotherusb\",\"synched\":true}"), QByteArrayLiteral("json"));

    s->insertAttribute(QStringLiteral("/pps/qnxcar/system/info"), QStringLiteral("buildNum"), QByteArrayLiteral("6467"), QByteArray());
    s->insertAttribute(QStringLiteral("/pps/qnxcar/system/info"), QStringLiteral("car2Rev"), QByteArrayLiteral("6106"), QByteArray());
    s->insertAttribute(QStringLiteral("/pps/qnxcar/system/info"), QStringLiteral("date"), QByteArrayLiteral("2013.04.25-12:10:16"), QByteArray());

    s->insertAttribute(QStringLiteral("/pps/services/update/status"), QStringLiteral("updateAvailable"), QByteArrayLiteral("0"), QByteArray("n"));
    s->insertAttribute(QStringLiteral("/pps/services/update/control"), QStringLiteral("cmd"), QByteArray(), QByteArray());
}
