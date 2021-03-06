void setup() {
//  wifiNotConnected();
  Serial.begin(115200);
  pinMode(TriggerPinFront, OUTPUT);
  pinMode(EchoPinFront, INPUT);
  pinMode(TriggerPinRight, OUTPUT);
  pinMode(EchoPinRight, INPUT);
  pinMode(TriggerPinLeft, OUTPUT);
  pinMode(EchoPinLeft, INPUT);
  pinMode(LED, OUTPUT);

//  WiFi.begin(ssid, password);
//  Serial.println("Connecting to WiFi...");
//  while (WiFi.status() != WL_CONNECTED) {
//    delay(1000);
//    Serial.print(".");
//
//  }
//
//  Serial.println("Wifi connected!!");
//  Serial.println(WiFi.localIP());
  wifiManager();
  wifiConnected();

  if (!SPIFFS.begin(true)) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    ESP.restart();
  }
  else {
    delay(500);
    Serial.println("SPIFFS mounted successfully");
  }

  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);

  // initialize OV2640 camera module
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;

  if (psramFound()) {
    config.frame_size = FRAMESIZE_UXGA;
    config.jpeg_quality = 10;
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }

  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    ESP.restart();
  }


  sensor_t * s = esp_camera_sensor_get();
  // initial sensors are flipped vertically and colors are a bit saturated
  if (s->id.PID == OV3660_PID) {
    s->set_vflip(s, 1); // flip it back
    s->set_brightness(s, 1); // up the brightness just a bit
    s->set_saturation(s, -2); // lower the saturation
  }
  // drop down frame size for higher initial frame rate
  s->set_framesize(s, FRAMESIZE_QVGA);
  startCameraServer();

  configuration.api_key = API_KEY;
  configuration.database_url  = DATABASE_URL ;
  authentication.user.email = USER_EMAIL;
  authentication.user.password = USER_PASSWORD;
  configuration.token_status_callback = tokenStatusCallback;

  Firebase.begin(&configuration, &authentication);
  Firebase.reconnectWiFi(true);
}
