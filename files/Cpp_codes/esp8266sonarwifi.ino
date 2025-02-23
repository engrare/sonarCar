//when coding select: NodeMCU 1.0 (ESP-12E Module)
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

const char* ssid = "PUE-WLAN";  // Your WiFi SSID with spaces
const char* password = "4001133866148221";  // Replace with your WiFi password

ESP8266WebServer server(80);  // Create a server on port 80

IPAddress local_IP(192, 168, 1, 184);
// Set your Gateway IP address
IPAddress gateway(192, 168, 1, 1);

IPAddress subnet(255, 255, 0, 0);
IPAddress primaryDNS(8, 8, 8, 8);   //optional
IPAddress secondaryDNS(8, 8, 4, 4); //optional

String htmlPage = R"rawliteral(
<!-- Copyright 2025 Kaya Sertel. All Rights Reserved.  -->
<!DOCTYPE html>
<html  lang="tr" dir="ltr" data-cast-api-enabled="true">
<head>
<title>Sonar Car</title>
<link rel="icon" type="image/png" href="https://www.engrare.com/files/photos/title_icon.png" sizes="48x48">
<link href='https://fonts.googleapis.com/css?family=Atkinson Hyperlegible|Bebas Neue|Montserrat' rel='stylesheet'>
<script src="https://ajax.googleapis.com/ajax/libs/jquery/3.5.1/jquery.min.js"></script>
<link rel="stylesheet" href="https://site-assets.fontawesome.com/releases/v6.4.2/css/all.css">
<script src="https://code.jquery.com/ui/1.14.0/jquery-ui.js"></script>
<script>
var is_esp = true;
</script>
<script src="https://sonarcar.engrare.com/files/script.js"></script>
<link rel="stylesheet" href="https://sonarcar.engrare.com/files/style.css">
<!--<script src="./files/script.js"></script>
<link rel="stylesheet" href="./files/style.css">-->


</head>

<body>
	<div class="main_div">
		<div class="main_menu_outer_div">
			<div class="main_left_div">
				<div class="button_part">
					<!--<div class="button_des1_outer mapping_btn"><div class="button_des1_inner">MAPPING</div></div>
					<div class="button_des1_outer str_msn_btn"><div class="button_des1_inner">START MISSION</div></div>
					<div class="button_des1_outer stop_msn_btn"><div class="button_des1_inner">STOP MISSION</div></div>-->
					<button class="button_des2_outer mapping_btn" role="button" onclick="mapping()">
						<div class="button_des2_shadow"></div>
						<div class="button_des2_edge"></div>
						<div class="button_des2_front"><i class="fa fa-map" style="font-size: 20px;"></i></div>
					</button>
					<button class="button_des2_outer str_msn_btn" role="button" onclick="startMission()">
						<div class="button_des2_shadow"></div>
						<div class="button_des2_edge"></div>
						<div class="button_des2_front"><i class="fa fa-play" style="font-size: 20px;"></i></div>
					</button>
					<button class="button_des2_outer stop_msn_btn" role="button" onclick="stopMission()">
						<div class="button_des2_shadow"></div>
						<div class="button_des2_edge"></div>
						<div class="button_des2_front"><i class="fa fa-stop" style="font-size: 20px;"></i></div>
					</button>
				</div>
				
				<div class="info_robot_part">25%
					<div class="div_charge_bar_outer"><div class="div_charge_bar_inner"></div></div>
					<div class="current_coord_text_outer">Current:<div class="current_coord_text">X: 20 Y: 35</div></div>
					<div class="new_coord_text_outer">New:<div class="new_coord_text">X: 20 Y: 35</div></div>
				</div>
				
				<div class="manuel_control_part_outer">
					<div class="manuel_control_turn_outer">
						<div class="manuel_control_turn_btn" onclick="buttonStateChanged('q', false)" onmousedown="buttonStateChanged('q', true)"><div class="manuel_control_turn_letter">Q</div></div>
						<div class="manuel_control_turn_btn" onclick="buttonStateChanged('e', false)" onmousedown="buttonStateChanged('e', true)"><div class="manuel_control_turn_letter">E</div></div>
					</div>
					<div class="manuel_control_directions">
						<div class="manuel_control_dir_letter">
							<div class="manuel_control_go_letter"><div class="manuel_control_turn_btn" onclick="buttonStateChanged('w', false)" onmousedown="buttonStateChanged('w', true)"><div class="manuel_control_turn_letter">W</div></div></div>
							<div class="manuel_control_center_letters"><div class="manuel_control_go_letter" style="width: unset;"><div class="manuel_control_turn_btn"  onclick="buttonStateChanged('a', false)" onmousedown="buttonStateChanged('a', true)"><div class="manuel_control_turn_letter">A</div></div></div>
							<div class="manuel_control_go_letter" style="width: unset;"><div class="manuel_control_turn_btn"  onclick="buttonStateChanged('d', false)" onmousedown="buttonStateChanged('d', true)"><div class="manuel_control_turn_letter">D</div></div></div></div>
							<div class="manuel_control_go_letter"><div class="manuel_control_turn_btn"  onclick="buttonStateChanged('s', false)" onmousedown="buttonStateChanged('s', true)"><div class="manuel_control_turn_letter">S</div></div></div>
						</div>
					</div>
				</div>
			</div>
			<div class="main_right_div">
				<div class="car_image_div">
					<image src="https://sonarcar.engrare.com/files/photos/sonar_car_image.png" height="100%" width="100%" class="sonar_car_img" />
				</div>
				<div class="location_image_div">
					<image src="https://sonarcar.engrare.com/files/photos/location_mark_icon.png" height="100%" width="100%" class="location_mark_img" />
				
				</div>
			</div>
		</div>
	</div>

</body>

</html>
)rawliteral";

void handleRoot() {
  server.send(200, "text/html", htmlPage);
}

void handleSendText() {
  if (server.method() == HTTP_POST) {
    String text = server.arg("text");  // Get the text from the POST request
    Serial.print(text);
    //Serial.println("Received text: " + text);
    server.send(200, "text/plain", "Text received");
  } else {
    server.send(405, "text/plain", "Method Not Allowed");
  }
}

void setup() {
  Serial.begin(19200);
  WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS);
  WiFi.begin(ssid, password);

  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConnected to WiFi");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  server.on("/", HTTP_GET, handleRoot);
  server.on("/send", HTTP_POST, handleSendText);
  server.begin();
}

void loop() {
  server.handleClient();
  
}
