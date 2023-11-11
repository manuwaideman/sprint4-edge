#include <WiFi.h> 

#include <PubSubClient.h> 

#include <Servo.h> 

  

const char* SSID = "Wolace"; 

const char* PASSWORD = "wolace2"; 

const char* BROKER_MQTT = "46.17.108.113"; 

int BROKER_PORT = 1883; 

const char* TOPICO_PUBLISH_LED = "/TEF/lamp108/attrs/led"; 

const char* TOPICO_PUBLISH_SERVO = "/TEF/lamp108/attrs/servo"; 

const char* TOPICO_PUBLISH_DISTANCE = "/TEF/lamp108/attrs/distance"; 

  

WiFiClient espClient; 

PubSubClient MQTT(espClient); 

char EstadoSaida = '0'; 

  

Servo servo_5; 

  

long readUltrasonicDistance(int triggerPin, int echoPin) { 

  pinMode(triggerPin, OUTPUT); 

  digitalWrite(triggerPin, LOW); 

  delayMicroseconds(2); 

   

  digitalWrite(triggerPin, HIGH); 

  delayMicroseconds(10); 

   

  digitalWrite(triggerPin, LOW); 

  pinMode(echoPin, INPUT); 

   

  return pulseIn(echoPin, HIGH); 

} 

  

void setup() { 

  Serial.begin(115200); 

  pinMode(13, OUTPUT); 

  pinMode(5, OUTPUT); 

  servo_5.attach(5); 

  delay(5000);  // Aguarda 5 segundos antes de iniciar 

  initWiFi(); 

  initMQTT(); 

} 

  

void initWiFi() { 

  delay(10); 

  Serial.println("------Conexao WI-FI------"); 

  Serial.print("Conectando-se na rede: "); 

  Serial.println(SSID); 

  Serial.println("Aguarde"); 

  reconectWiFi(); 

} 

  

void reconectWiFi() { 

  if (WiFi.status() == WL_CONNECTED) { 

    return; 

  } 

  

  WiFi.begin(SSID, PASSWORD); 

  

  while (WiFi.status() != WL_CONNECTED) { 

    delay(100); 

    Serial.print("."); 

  } 

  

  Serial.println(); 

  Serial.print("Conectado com sucesso na rede "); 

  Serial.print(SSID); 

  Serial.println("IP obtido: "); 

  Serial.println(WiFi.localIP()); 

} 

  

void initMQTT() { 

  MQTT.setServer(BROKER_MQTT, BROKER_PORT); 

  MQTT.setCallback(mqtt_callback); 

  reconnectMQTT(); 

} 

  

void reconnectMQTT() { 

  while (!MQTT.connected()) { 

    Serial.print("* Tentando se conectar ao Broker MQTT: "); 

    Serial.println(BROKER_MQTT); 

     

    if (MQTT.connect("fiware_108")) { 

      Serial.println("Conectado com sucesso ao broker MQTT!"); 

      MQTT.subscribe(TOPICO_PUBLISH_LED); 

      MQTT.subscribe(TOPICO_PUBLISH_SERVO); 

      MQTT.subscribe(TOPICO_PUBLISH_DISTANCE); 

    } else { 

      Serial.println("Falha ao reconectar no broker."); 

      Serial.println("Havera nova tentatica de conexao em 2s"); 

      delay(2000); 

    } 

  } 

} 

  

void mqtt_callback(char* topic, byte* payload, unsigned int length) { 

  String msg; 

   

  for (int i = 0; i < length; i++) { 

    char c = (char)payload[i]; 

    msg += c; 

  } 

  

  Serial.print("Mensagem recebida no tópico "); 

  Serial.print(topic); 

  Serial.print(": "); 

  Serial.println(msg); 

  

  if (strcmp(topic, TOPICO_PUBLISH_LED) == 0) { 

    if (msg.equals("on")) { 

      digitalWrite(13, HIGH); 

    } else if (msg.equals("off")) { 

      digitalWrite(13, LOW); 

    } 

  } else if (strcmp(topic, TOPICO_PUBLISH_SERVO) == 0) { 

    if (msg.equals("90")) { 

      servo_5.write(90); 

    } else if (msg.equals("0")) { 

      servo_5.write(0); 

    } 

  } else if (strcmp(topic, TOPICO_PUBLISH_DISTANCE) == 0) { 

    Serial.print("Distancia do sensor: "); 

    Serial.print(msg); 

    Serial.println(" cm"); 

  } 

} 

  

void loop() { 

  VerificaConexoesWiFIEMQTT(); 

  EnviaEstadoOutputMQTT(); 

  readAndPublishSensorData(); 

  MQTT.loop(); 

  delay(2000);  // Intervalo de 2 segundos entre as leituras do sensor 

} 

  

void EnviaEstadoOutputMQTT() { 

  if (EstadoSaida == '1') { 

    MQTT.publish(TOPICO_PUBLISH_LED, "on"); 

    Serial.println("- LED Ligado - Mensagem MQTT Enviada"); 

  } 

  if (EstadoSaida == '0') { 

    MQTT.publish(TOPICO_PUBLISH_LED, "off"); 

    Serial.println("- LED Desligado - Mensagem MQTT Enviada"); 

  } 

} 

  

void readAndPublishSensor 
