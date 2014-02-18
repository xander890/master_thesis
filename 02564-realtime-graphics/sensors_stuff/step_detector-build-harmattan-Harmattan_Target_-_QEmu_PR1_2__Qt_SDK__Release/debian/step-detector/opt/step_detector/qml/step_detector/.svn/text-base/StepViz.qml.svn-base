// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1

Rectangle {
    id: stepViz
    width: value
    height: value
    property int value: 0
    color: "black"
    function step(value_)
    {
	stepViz.value = value_;
    }

    Timer
    {
	running: true
	interval: 10
	repeat: Animation.Infinite
	onTriggered:
	{
	    if (stepViz.value > 0) stepViz.value -= 1;
	}
    }
}
