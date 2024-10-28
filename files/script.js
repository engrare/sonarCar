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

