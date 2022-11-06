
#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include "DEV_Config.h"
#include "EPD.h"
#include "GUI_Paint.h"
#include "ImageData.h"

// Provide the token generation process info.
#include <addons/TokenHelper.h>
void updateDisplay();
void updateEpaper(String ID, String Name, String Message);

#define WIFI_SSID "DJ"
#define WIFI_PASSWORD "0481355596"

#define API_KEY "AIzaSyD3qQ-lD8WUFVslyiK7AEjU_Dad3llnEhU"

#define FIREBASE_PROJECT_ID "esp32-epaper"

#define USER_EMAIL "test@gmail.com"
#define USER_PASSWORD "00000000"

// Define Firebase Data object
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

float vBat = 0;
bool taskcomplete = false;
unsigned long dataMillis = 0;

String getDevice()
{ // get ESP32 unique ID
    char name[23];
    uint64_t chipid = ESP.getEfuseMac(); // The chip ID is essentially its MAC address(length: 6 bytes).
    snprintf(name, 23, "%04X%08X", (uint16_t)(chipid >> 32), (uint32_t)chipid);
    return name;
}

String devicename = getDevice(); // accomodate ESP32 ID into a variable

String documentPath = "users/user01"; // making document path in the firestore database

String getData(String field)
{

    if (Firebase.Firestore.getDocument(&fbdo, FIREBASE_PROJECT_ID, "", documentPath.c_str(), field.c_str()))
        ;
    // Serial.printf("ok\n%s\n\n", fbdo.payload().c_str());
    else
        Serial.println(fbdo.errorReason());

    FirebaseJson payload;
    payload.setJsonData(fbdo.payload().c_str());

    // Get the data from FirebaseJson object
    FirebaseJsonData jsonData;
    payload.get(jsonData, "fields/" + field + "/stringValue", true);
    String value = jsonData.stringValue;
    return value;
}

void setup()
{
    
    Serial.begin(115200);

    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    // WiFi.setTxPower(WIFI_POWER_13dBm );
    esp_wifi_set_ps(WIFI_PS_MAX_MODEM);

    Serial.print("Connecting to Wi-Fi");
    while (WiFi.status() != WL_CONNECTED)
    {
        // Serial.print(".");
        // delay(300);
    }
    Serial.println();
    Serial.print("Connected with IP: ");
    Serial.println(WiFi.localIP());
    Serial.println();

    Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);

    /* Assign the api key (required) */
    config.api_key = API_KEY;

    /* Assign the user sign in credentials */
    auth.user.email = USER_EMAIL;
    auth.user.password = USER_PASSWORD;

    /* Assign the callback function for the long running token generation task */
    config.token_status_callback = tokenStatusCallback; // see addons/TokenHelper.h

    // Limit the size of response payload to be collected in FirebaseData
    fbdo.setResponseSize(2048);

    Firebase.begin(&config, &auth);

    Firebase.reconnectWiFi(true);

    updateDisplay();
}

void updateDisplay()
{
    // Firebase.ready() should be called repeatedly to handle authentication tasks.

    if (Firebase.ready())
    {
        dataMillis = millis();

        FirebaseJson content;

        String batteryVoltage = "";
        batteryVoltage.concat(vBat);
        batteryVoltage += "v";
        // aa is the collection id, bb is the document id.
        String documentPath = "users/user01";

        content.set("fields/vBat/stringValue", batteryVoltage);

        Serial.print("Update a document... ");

        if (Firebase.Firestore.patchDocument(&fbdo, FIREBASE_PROJECT_ID, "" /* databaseId can be (default) or empty */, documentPath.c_str(), content.raw(), "vBat" /* updateMask */))
            Serial.printf("ok\n%s\n\n", fbdo.payload().c_str());
        else
            Serial.println(fbdo.errorReason());

        String ID = getData("ID");
        String Name = getData("Name");
        String Message = getData("Message");

        // WiFi.disconnect(true);
        WiFi.mode(WIFI_OFF);
        // WiFi.setSleep(true);
         setCpuFrequencyMhz(10);
        updateEpaper(ID, Name, Message);

        // mencetak value dari variabel
        Serial.print("ID: ");
        Serial.println(ID);
        Serial.print("Name: ");
        Serial.println(Name);
        Serial.print("Message: ");
        Serial.println(Message);
    }
}

void updateEpaper(String ID, String Name, String Message)
{
    char userID[ID.length() + 1];
    char userName[Name.length() + 1];
    char userMessage[Message.length() + 1];
    ID.toCharArray(userID, ID.length() + 1);
    Name.toCharArray(userName, Name.length() + 1);
    Message.toCharArray(userMessage, Message.length() + 1);

    DEV_Module_Init();
    EPD_2IN9_V2_Init();
    EPD_2IN9_V2_Clear();

    //   DEV_Delay_ms(500);

    UBYTE *BlackImage; // Create a new image cache

    /* you have to edit the startup_stm32fxxx.s file and set a big enough heap size */
    UWORD Imagesize = ((EPD_2IN9_V2_WIDTH % 8 == 0) ? (EPD_2IN9_V2_WIDTH / 8) : (EPD_2IN9_V2_WIDTH / 8 + 1)) * EPD_2IN9_V2_HEIGHT;
    if ((BlackImage = (UBYTE *)malloc(Imagesize)) == NULL)
    {
        printf("Failed to apply for black memory...\r\n");
        while (1)
            ;
    }
    printf("Paint_NewImage\r\n");
    Paint_NewImage(BlackImage, EPD_2IN9_V2_WIDTH, EPD_2IN9_V2_HEIGHT, 270, WHITE);

    //#if 1 // show image for array
    //  Paint_NewImage(BlackImage, EPD_2IN9_V2_WIDTH, EPD_2IN9_V2_HEIGHT, 270, WHITE);
    // printf("show image for array\r\n");
    // Paint_SelectImage(BlackImage);
    // Paint_Clear(WHITE);
    // Paint_DrawBitMap(gImage_2in9);

    // EPD_2IN9_V2_Display(BlackImage);
    //  DEV_Delay_ms(2000);
    //#endif

    printf("Drawing\r\n");
    // 1.Select Image
    Paint_SelectImage(BlackImage);
    Paint_Clear(WHITE);

    // 2.Drawing on the image
    printf("Drawing:BlackImage\r\n");
    Paint_DrawString_EN(0, 0, "Name:", &Font20, WHITE, BLACK);
    Paint_DrawString_EN(0, 20, "ID:", &Font20, WHITE, BLACK);
    Paint_DrawString_EN(0, 40, "Message:", &Font20, WHITE, BLACK);

    Paint_DrawString_EN(70, 2, userName, &Font16, WHITE, BLACK);
    Paint_DrawString_EN(40, 22, userID, &Font16, WHITE, BLACK);
    Paint_DrawString_EN(110, 42, userMessage, &Font16, WHITE, BLACK);

    EPD_2IN9_V2_Display_Base(BlackImage);
    //   DEV_Delay_ms(2000);

    printf("Goto Sleep...\r\n");
    EPD_2IN9_V2_Sleep();
    free(BlackImage);
    BlackImage = NULL;

    // esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_OFF);
    // esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_SLOW_MEM, ESP_PD_OPTION_OFF);
    // esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_FAST_MEM, ESP_PD_OPTION_OFF);
    // esp_sleep_pd_config(ESP_PD_DOMAIN_XTAL, ESP_PD_OPTION_OFF);

  //  esp_sleep_enable_timer_wakeup(3000000);
  //  esp_deep_sleep_start();
}

void loop()
{
}