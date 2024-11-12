//Copyright 2025 Kaya Sertel. All Rights Reserved.
var is_mobile_phone = ( /Android|webOS|iPhone|iPad|iPod|BlackBerry|IEMobile|Opera Mini/i.test(navigator.userAgent) ) ? true : false;
var key_pressed = [false, false, false, false, false, false];

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
		//console.log(key + ' key pressed');
		if(key == 'q' && !key_pressed[0]) {
			console.log("turning right");
			key_pressed[0] = true;
		} else if(key == 'e' && !key_pressed[1]) {
			console.log("turning left");
			key_pressed[1] = true;
		} else	if(key == 'w' && !key_pressed[2]) {
			console.log("going forward");
			key_pressed[2] = true;
		} else	if(key == 'a' && !key_pressed[3]) {
			console.log("going left");
			key_pressed[3] = true;
		} else	if(key == 's' && !key_pressed[4]) {
			console.log("going backward");
			key_pressed[4] = true;
		}else if(key == 'd' && !key_pressed[5]) {
			console.log("going right");
			key_pressed[5] = true;
		}
	});

	$(document).keyup(function(event) {
		// When a key is released
		var key = event.key; // Get the key that was released
		if(key == 'q') {
			key_pressed[0] = false;
		} else if(key == 'e') {
			key_pressed[1] = false;
		} else	if(key == 'w') {
			key_pressed[2] = false;
		} else	if(key == 'a') {
			key_pressed[3] = false;
		} else	if(key == 's') {
			key_pressed[4] = false;
		} else if(key == 'd') {
			key_pressed[5] = false;
		}
	});
});

  $( function() {

    $( ".location_image_div" ).draggable({ drag: function() {refCoordinates();}, containment: ".main_right_div", scroll: false }); //drag, stop, start
  } );
  
function refCoordinates() {
	$( ".new_coord_text" ).text("X: " + Math.round(($( ".location_image_div" ).position().left - $( ".main_left_div" ).width())/10)*10 + " Y: " + Math.round($( ".location_image_div" ).position().top/10)*10);
}
  

  $( window ).resize(function() {
	beReadyPage();
	setTimeout(function() { beReadyPage();}, 100);
});
  
  $("body").keydown(function(event) {
  if (!key_pressed) {
        var keyPressed = keys[event.keyCode];
        console.log(keyPressed);
    console.log(" key pressed");
    key_pressed = true;
  };
});

$("body").keyup(function(event) {
  key_pressed = false;
  console.log(' key released');
});
  
  
  function beReadyPage() {
	window_height = parseInt($( window ).height());
	window_width = parseInt($( window ).width());
	
  }
  
  function moveRobot(inputText) {
	  console.log("Please Enable moveRobot function");
  }
  
  
  
  function sendText(inputText) {
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

