function turnIcon(command) {
    switch (command) {
    case ManeuversModel.ArrivedAtDestination:
        return "dt";
    case ManeuversModel.DestinationIsOnTheLeft:
        return "dt-l";
    case ManeuversModel.DestinationIsOnTheRight:
        return "dt-r";
    case ManeuversModel.FollowRoundaboutOnTheLeft:
        return "lht-rt";
    case ManeuversModel.UTurnOnTheLeft:
        return "lht-ut";
    case ManeuversModel.NoChange:
        return "nc";
    case ManeuversModel.FollowRoundaboutOnTheRight:
        return "rt";
    case ManeuversModel.TurnLeft:
        return "tr-l";
    case ManeuversModel.TurnRight:
        return "tr-r";
    case ManeuversModel.UTurnOnTheRight:
        return "ut";
    }

    return "nc";
}

function formatDistance(meters) {
    if (meters > 1000)
        return qsTr("%1 km").arg(Math.floor(meters / 100) / 10);
    else
        return qsTr("%1 m").arg(meters);
}
