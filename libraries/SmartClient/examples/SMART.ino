  // Set the LCD I2C address
//Smartapps Libs:

#include <SmartClient.h>


//Ethernet Libs:
#include <SPI.h>
#include <Ethernet.h>
#include <ArduinoJson.h>
//Definicao endereco MAC do Ethernet Shield:
//byte mac[] = {  0xDE, 0xAD, 0xBE, 0xEF, 0xFF, 0xED };
byte mac[] = {  0xDE, 0xAD, 0xBE, 0xEF, 0xFF, 0xED };
//Definicoes do usuário na Smartapps:
char server[] = "www.smartapps.com.br";              //Endereco Smartapps.
char login[] = "7a4e894ab12b917aeb0f33b7e61139b8";   //Login do usuario: Encontra-se na plataforma.
char password[] = "AG8BPFRlUmtfNARmD28=";            //Senha do usuário: Encontra-se na plataforma.
char app[] = "controls";                             //Nome do aplicativo utilizado.
char schema[] = "7a4e894ab12b917aeb0f33b7e61139b8";  //Schema do aplicativo: Normalmente o mesmo do usuário, só muda se for utilizado um aplicativo que foi compartilhado por outro usuário, neste caso é o login do outro usuário.
char caminhoPost[] = "variaveis_valores/insert";
String result;
String lastRegister;
SmartClient smart; 
//Variaveis do Exemplo 1;
char* PostData;
//Variaveis usadas pela funcao get_free_memory(). Muito util, principalmente com o Arduino Uno.
extern int __bss_end;                                
extern void *__brkval;                               
void setup() {
  Serial.begin(9600);
  config_rede();
  smart.connect(server, login, password);
  result = smart.getSchema(server,login,password);
  Serial.print("Schema: ");
  Serial.print(result);

  delay(2000);
  lastRegister = smart.getLastRegister(server,login,password);
  Serial.println(lastRegister);
  StaticJsonBuffer<70> jsonBuffer;

  JsonObject& root = jsonBuffer.parseObject(lastRegister);
  if (!root.success()) {
    Serial.println("parseObject() failed");
    return;
  }
  const char* valor = root["valor"];
  Serial.println("VALOR: ");
  Serial.println(valor);
}
void loop() {
     PostData = "variavel=10&valor=100";//+String(random(0,100));  //Gera valor randomico a cada loop e adiciona a variavel escolhida.
     smart.send(true, server, app, schema, caminhoPost, PostData);
     delay(2000);
}

//Funcao get_free_memory(): 
int get_free_memory()
{
  int free_memory;
  if((int)__brkval == 0)
    free_memory = ((int)&free_memory) - ((int)&__bss_end);
  else
    free_memory = ((int)&free_memory) - ((int)__brkval);
  return free_memory;
}



