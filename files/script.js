//Copyright 2025 Kaya Sertel. All Rights Reserved.
var is_mobile_phone = ( /Android|webOS|iPhone|iPad|iPod|BlackBerry|IEMobile|Opera Mini/i.test(navigator.userAgent) ) ? true : false;
var key_pressed = [false, false, false, false, false, false];
var new_X_coor = 0, new_Y_coor = 0;
var encryption_chars = "!\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`{}~";

fetch('https://raw.githubusercontent.com/eylulberil/encoded_key/main/keys.json')
  .then(response => response.json())
  .then(myObj => {
	encrypted_key = myObj[0];
	console.log(encrypted_key);
	
  })
  .catch(error => {
    // Handle any errors that occur during the fetch request
    console.log('Error:', error);
  });

$(document).ready(function() {
	$(document).keydown(function(event) {
		// When a key is pressed
		var key = event.key; // Get the key that was pressed
		buttonStateChanged(key, true);
	});

	$(document).keyup(function(event) {
		// When a key is released
		var key = event.key; // Get the key that was released
		buttonStateChanged(key, false);
	});
});

  $( function() {

    $( ".location_image_div" ).draggable({ drag: function() {refCoordinates();}, containment: ".main_right_div", scroll: false }); //drag, stop, start
  } );
  
function refCoordinates() {
	new_X_coor = Math.round(($( ".location_image_div" ).position().left - $( ".main_left_div" ).width())/10)*10;
	new_Y_coor = Math.round($( ".location_image_div" ).position().top/10)*10;
	$( ".new_coord_text" ).text("X: " + new_X_coor + " Y: " + new_Y_coor);
	
}


function buttonStateChanged(key, bool_btn_state) {
	if(bool_btn_state) {
		if(key == 'q' && !key_pressed[0]) {
			//console.log("turning right");
			$(".manuel_control_turn_btn:not(:eq(0))").css("background", "black");
			$(".manuel_control_turn_btn:eq(0)").css("background", "#3d3d3d");
			moveRobot('q');
			key_pressed[0] = true;
		} else if(key == 'e' && !key_pressed[1]) {
			//console.log("turning left");
			$(".manuel_control_turn_btn:not(:eq(1))").css("background", "black");
			$(".manuel_control_turn_btn:eq(1)").css("background", "#3d3d3d");
			moveRobot('e');
			key_pressed[1] = true;
		} else	if(key == 'w' && !key_pressed[2]) {
			//console.log("going forward");
			$(".manuel_control_turn_btn:not(:eq(2))").css("background", "black");
			$(".manuel_control_turn_btn:eq(2)").css("background", "#3d3d3d");
			moveRobot('f');
			key_pressed[2] = true;
		} else	if(key == 'a' && !key_pressed[3]) {
			//console.log("going left");
			$(".manuel_control_turn_btn:not(:eq(3))").css("background", "black");
			$(".manuel_control_turn_btn:eq(3)").css("background", "#3d3d3d");
			moveRobot('l');
			key_pressed[3] = true;
		} else	if(key == 'd' && !key_pressed[4]) {
			//console.log("going backward");
			$(".manuel_control_turn_btn:not(:eq(4))").css("background", "black");
			$(".manuel_control_turn_btn:eq(4)").css("background", "#3d3d3d");
			moveRobot('r');
			key_pressed[4] = true;
		}else if(key == 's' && !key_pressed[5]) {
			//console.log("going right");
			$(".manuel_control_turn_btn:not(:eq(5))").css("background", "black");
			$(".manuel_control_turn_btn:eq(5)").css("background", "#3d3d3d");
			moveRobot('b');
			key_pressed[5] = true;
		}
	} else {
		let is_related = true;
		if(key == 'q') {
			key_pressed[0] = false;
			$(".manuel_control_turn_btn:eq(0)").css("background", "black");
		} else if(key == 'e') {
			key_pressed[1] = false;
			$(".manuel_control_turn_btn:eq(1)").css("background", "black");
		} else	if(key == 'w') {
			key_pressed[2] = false;
			$(".manuel_control_turn_btn:eq(2)").css("background", "black");
		} else	if(key == 'a') {
			key_pressed[3] = false;
			$(".manuel_control_turn_btn:eq(3)").css("background", "black");
		} else	if(key == 'd') {
			key_pressed[4] = false;
			$(".manuel_control_turn_btn:eq(4)").css("background", "black");
		} else if(key == 's') {
			key_pressed[5] = false;
			$(".manuel_control_turn_btn:eq(5)").css("background", "black");
		} else {
			is_related = false;
			$(".manuel_control_turn_btn").css("background", "black");
		}
		if(is_related) {
			let i = key_pressed.length;
			while(i--) {
				if(key_pressed[i]){
					break;
				} else if(i == 0) {
					moveRobot('p'); //stop
				}
			}
		}
	}
}

function mapping() {
	sendText("ksm|");
}

function stopMission() {
	sendText("kss|");
}

function startMission() {
	sendText("sk" + Math.round(new_X_coor/20) + "a" + Math.round(new_Y_coor/20) + "|");
}

  $( window ).resize(function() {
	beReadyPage();
	setTimeout(function() { beReadyPage();}, 100);
});
  
function beReadyPage() {
	window_height = parseInt($( window ).height());
	window_width = parseInt($( window ).width());
}
  
function moveRobot(inputText) {
	sendText("ks" + inputText + "|");
}
  
  function sendText(inputText) {
	  if(!is_esp) {
		  console.log("The " + inputText + " text will be sended however please Enable moveRobot function first.");
		  return; //delete this to Enable function
	  }
  fetch("/send", {
	method: "POST",
	headers: {
	  "Content-Type": "application/x-www-form-urlencoded"
	},
	body: "text=" + encodeURIComponent(inputText)
  }).then(response => response.text())
	.then(data => console.log(data))
	.catch(error => console.error("Error:", error));
}
  
  setTimeout(function() { beReadyPage();}, 200);
setTimeout(function() { beReadyPage();}, 500);
