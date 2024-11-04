//Copyright 2025 Kaya Sertel. All Rights Reserved.
var is_mobile_phone = ( /Android|webOS|iPhone|iPad|iPod|BlackBerry|IEMobile|Opera Mini/i.test(navigator.userAgent) ) ? true : false;


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

$( document ).ready(function() {


});


function sendText() {
  const inputText = document.getElementById("inputText").value;
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

  $( function() {

    $( ".location_image_div" ).draggable({ containment: ".main_right_div", scroll: false });
  } );
  
  
  

  $( window ).resize(function() {
	beReadyPage();
	setTimeout(function() { beReadyPage();}, 100);
});
  
  function beReadyPage() {
	window_height = parseInt($( window ).height());
	window_width = parseInt($( window ).width());
	
  }
  
  setTimeout(function() { beReadyPage();}, 200);
setTimeout(function() { beReadyPage();}, 500);
