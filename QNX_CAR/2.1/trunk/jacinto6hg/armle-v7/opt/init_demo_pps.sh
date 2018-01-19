#!/bin/sh

echo fl::0 >> /pps/ui
echo fr::0 >> /pps/ui
echo rl::0 >> /pps/ui
echo rr::0 >> /pps/ui
echo backdoor::0 >> /pps/ui
echo set_ui:: 3 >> /pps/screen

echo clockwise::0 >> /pps/hinge-tech/gesture_control
echo gesture::none >> /pps/hinge-tech/gesture_control
echo gesture_tip::hide >> /pps/hinge-tech/gesture_control

echo ambience:n:1 >> /pps/hinge-tech/lampstatus
echo brightness:n:3 >> /pps/hinge-tech/lampstatus
echo intersection:b:false >> /pps/hinge-tech/lampstatus
echo turn_left:b:false >> /pps/hinge-tech/lampstatus
echo turn_right:b:false >> /pps/hinge-tech/lampstatus
echo steerwheelshock:n:0 >> /pps/hinge-tech/lampstatus

echo copilot_heatingstatus:n:0 >> /pps/hinge-tech/seatcontrol
echo copilot_height_movement:n:0 >> /pps/hinge-tech/seatcontrol
echo copilot_lumbar_adjustment:n:0 >> /pps/hinge-tech/seatcontrol
echo copilot_recliner_movement:n:0 >> /pps/hinge-tech/seatcontrol
echo copilot_slide_movement:n:0 >> /pps/hinge-tech/seatcontrol
echo copilot_tilt_movement:n:0 >> /pps/hinge-tech/seatcontrol
echo main_heatingstatus:n:0 >> /pps/hinge-tech/seatcontrol
echo main_height_movement:n:0 >> /pps/hinge-tech/seatcontrol
echo main_lumbar_adjustment:n:0 >> /pps/hinge-tech/seatcontrol
echo main_recliner_movement:n:0 >> /pps/hinge-tech/seatcontrol
echo main_slide_movement:n:0 >> /pps/hinge-tech/seatcontrol
echo main_tilt_movement:n:0 >> /pps/hinge-tech/seatcontrol
echo watch_kind::none >> /pps/hinge-tech/seatcontrol

echo app::stop >> /pps/hinge-tech/application_status
echo carplay::stop >> /pps/hinge-tech/application_status
echo mlink::stop >> /pps/hinge-tech/application_status

echo NavTo:: >> /pps/services/geolocation/vr_control
echo TrafficQuery:: >> /pps/services/geolocation/vr_control
echo cmd:n:0 >> /pps/services/geolocation/vr_control
echo feedback:n:0 >> /pps/services/geolocation/vr_status
echo navi::none >> /pps/services/geolocation/navinfo

echo speed:n:0 >> /pps/qnxcar/sensors
echo laneDeparture:n:0 >> /pps/qnxcar/sensors

echo theme::technology >> /pps/hinge-tech/theme_control
echo multimedia::none >> /pps/hinge-tech/music_control
echo ui_style::2 >> /pps/ui
echo chair_goback::5 >> /pps/ui
echo chair_rotate::5 >> /pps/ui
echo chair_updown::-5 >> /pps/ui


echo AppSection:json:{"action": "pause"} >> /pps/system/navigator/command
echo Communication:json:{"action": "pause"} >> /pps/system/navigator/command
echo MediaPlayer:json:{"action": "pause"} >> /pps/system/navigator/command
echo Settings:json:{"action": "pause"} >> /pps/system/navigator/command
echo navigation:json:{"action": "pause"} >> /pps/system/navigator/command

echo source::none >> /pps/hinge-tech/source_manger

echo frontwindow_heating:b:false >> /pps/qnxcar/hvac
echo heatedSeat_row1left:n:0 >> /pps/qnxcar/hvac
echo backwindow_heating:b:false >> /pps/qnxcar/hvac
echo airRecirculation_all:n:0 >> /pps/qnxcar/hvac
echo airConditioning_all:b:false >> /pps/qnxcar/hvac

echo pm2_5_state::1 >> /pps/ui

echo pluse:n:0 >> /pps/hinge-tech/health_info

echo seek::stop >> /pps/radio/status 
