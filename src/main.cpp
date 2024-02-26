#include <Arduino.h>
#include <WiFi.h>

WiFiServer server(80);

String getSecurityType(int encryptionType)
{
  switch (encryptionType)
  {
  case (WIFI_AUTH_OPEN):
    return "Open";
  case (WIFI_AUTH_WEP):
    return "WEP";
  case (WIFI_AUTH_WPA_PSK):
    return "WPA";
  case (WIFI_AUTH_WPA2_PSK):
    return "WPA2";
  case (WIFI_AUTH_WPA_WPA2_PSK):
    return "WPA/WPA2";
  default:
    return "Unknown";
  }
}

void setup()
{
  Serial.begin(115200);
  WiFi.softAP("ESP32", "1234567890");
  Serial.println("Access Point started!");
  Serial.print("IP Address: ");
  Serial.print(WiFi.localIP());
  server.begin();
}

void loop()
{
  WiFiClient client = server.available();
  if (!client)
  {
    return;
  }

  Serial.println("New client");

  while (!client.available())
  {
    delay(1);
  }

  String request = client.readStringUntil('\r');
  Serial.println(request);
  client.flush();

  if (request.indexOf("/connect") != -1)
  {
    String ssid;
    String password;
    if (request.indexOf("ssid=") != -1 && request.indexOf("password=") != -1)
    {
      ssid = request.substring(request.indexOf("ssid=") + 5);
      ssid = ssid.substring(0, ssid.indexOf("&"));
      password = request.substring(request.indexOf("password=") + 9);
      password = password.substring(0, password.indexOf(" "));
    }

    WiFi.begin(ssid.c_str(), password.c_str());
    Serial.print("Connecting to ");
    Serial.println(ssid);

    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20)
    {
      delay(500);
      Serial.print(".");
      attempts++;
    }

    if (WiFi.status() == WL_CONNECTED)
    {
      Serial.println("");
      Serial.println("WiFi connected");
      Serial.print("IP address: ");
      Serial.println(WiFi.localIP());
    }
    else
    {
      Serial.println("");
      Serial.println("Connection failed.");
    }

    client.println("HTTP/1.1 301 Moved Permanently");
    client.println("Location: /");
    client.println();

    client.stop();
    return;
  }

  client.println("HTTP/1.1 200 OK");
  client.println("Content-type:text/html");
  client.println();

  client.println("<!DOCTYPE html>");
  client.println("<html lang=\"pt-BR\">");
  client.println("<head>");
  client.println("<meta charset=\"UTF-8\" />");
  client.println("<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\" />");
  client.println("<title>Redes Wi-Fi</title>");
  client.println("<style>");
  client.println("body {");
  client.println("  background-color: black;");
  client.println("  color: #00ff00;");
  client.println("  font-family: 'Courier New', monospace;");
  client.println("  width: 100%;");
  client.println("  height: 100%;");
  client.println("  margin: 0;");
  client.println("  padding: 0;");
  client.println("}");
  client.println("h2 {");
  client.println("  text-align: center;");
  client.println("  gap: 10px 10px;");
  client.println("}");
  client.println("form {");
  client.println("  display: flex;");
  client.println("  align-items: center;");
  client.println("  justify-content: center;");
  client.println("  flex-direction: column;");
  client.println("}");
  client.println("form > section {");
  client.println("  display: flex;");
  client.println("  align-items: center;");
  client.println("  justify-content: center;");
  client.println("  flex-direction: row;");
  client.println("  width: 100%;");
  client.println("}");
  client.println("input[type='radio'] {");
  client.println("display: flex;");
  client.println("align-items: center;");
  client.println("justify-content: center;");
  client.println("flex-direction: row;");
  client.println("text-align: center;");
  client.println("}");
  client.println("input[type='password'] {");
  client.println("  width: 60%;");
  client.println("  padding: 8px;");
  client.println("  margin-top: 30px;");
  client.println("  margin-bottom: 15px;");
  client.println("  border: 2px solid #00ff00;");
  client.println("  border-radius: 4px;");
  client.println("  background-color: black;");
  client.println("  color: #00ff00;");
  client.println("  font-family: 'Courier New', monospace;");
  client.println("}");
  client.println("button {");
  client.println("  padding: 10px 20px;");
  client.println("  background-color: #00ff00;");
  client.println("  color: black;");
  client.println("  border: none;");
  client.println("  border-radius: 4px;");
  client.println("  cursor: pointer;");
  client.println("}");
  client.println("button:hover {");
  client.println("  background-color: #00cc00;");
  client.println("}");
  client.println("</style>");
  client.println("</head>");
  client.println("<body>");
  client.println("<h2>Redes disponíveis</h2>");
  client.println("<form action='/connect' method='GET'>");
  client.println("<section>");
  int numNetworks = WiFi.scanNetworks();
  client.println("<div style='display: flex; align-items: center; justify-content: center; flex-direction: column; text-align: center; width: 50%;'>");
  client.println("<h3>Nome da Rede</h3>");
  for (int i = 0; i < numNetworks; i++)
  {
    client.println("<div style='display: flex; align-items: center; justify-content: center; flex-direction: row; text-align: center; width: 100%; gap: 5px;'>");
    client.println("<input type='radio' name='ssid' value='" + WiFi.SSID(i) + "'>");
    client.println(WiFi.SSID(i) + "<br>");
    client.println("</div>");
  }
  client.println("</div>");

  client.println("<div style='display: flex; align-items: center; justify-content: center; flex-direction: column; text-align: center; width: 25%; gap: 5px;'>");
  client.println("<h3>Segurança</h3>");
  for (int i = 0; i < numNetworks; i++)
  {
    client.println(getSecurityType(WiFi.encryptionType(i)) + "<br>");
  }
  client.println("</div>");

  client.println("<div style='display: flex; align-items: center; justify-content: center; flex-direction: column; text-align: center; width: 25%; gap: 5px;'>");
  client.println("<h3>Sinal</h3>");
  for (int i = 0; i < numNetworks; i++)
  {
    if (WiFi.RSSI(i) >= -25)
    {
      client.println("****<br>");
    }
    else if (WiFi.RSSI(i) >= -50)
    {
      client.println("***<br>");
    }
    else if (WiFi.RSSI(i) >= -75)
    {
      client.println("**<br>");
    }
    else if (WiFi.RSSI(i) >= -100)
    {
      client.println("*<br>");
    }
  }
  client.println("</div>");
  client.println("</section>");

  client.println("<input type='password' name='password' placeholder='Senha' required>");
  client.println("<button type='submit'>Conectar</button>");
  client.println("</form>");

  client.println("</body>");
  client.println("</html>");

  delay(1000);

  client.stop();
}
