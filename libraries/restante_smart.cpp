
bool SmartClient::readHandshake() {
    bool result = false;
    int maxAttempts = 300, attempts = 0;
    int time = millis();
    int n;
    int i;
    while(_client.available() == 0 && attempts < maxAttempts) {
        delay(100);
        attempts++;
    }

   //procura se a resposta HTTP eh 200
    while(_client.available()) {
        readLine();
        n = -1;
        //-8 do HTTP/1.1 e -3 do 200
        for(i=0;i<posBuffer-11;i++){
            //procura o header
            if( (readBuffer[i] == 'H') &&
                (readBuffer[i+1] == 'T') &&
                (readBuffer[i+2] == 'T') &&
                (readBuffer[i+3] == 'P') &&
                (readBuffer[i+4] == '/') &&
                (readBuffer[i+5] == '1') &&
                (readBuffer[i+6] == '.') &&
                (readBuffer[i+7] == '1') &&
                (readBuffer[i+8] == ' ')
              ){
                //verifica se a resposta eh 200
                if( (readBuffer[i+9] == '2') &&
                   (readBuffer[i+10] == '0') &&
                   (readBuffer[i+11] == '0')
                 ){
                   //encontrou
                   n = i;
                    #ifdef DEBUGPRINT
                    Serial.println(F("Resposta HTTP OK"));
                    #endif
                   break;
                }else{
                  //deu erro na msg
                  _client.flush();
                  _client.stop();
                  return false;
                }
            }
        }
        if(n != -1){
                break;
        }
       

        #ifndef NEW
        n = buff.indexOf("HTTP/1.1");
        if(n != -1){
            if ((buff.charAt(n+9)== '2')&&
                (buff.charAt(n+10)== '0')&&
                (buff.charAt(n+11)== '0')){
                break;
            }else{
                _client.flush();
                _client.stop();
                return false;
            }
        }
        #endif
    }
    #ifdef DEBUGPRINT
    Serial.println(F("Procurando PHPSESSID"));
    #endif
    //procura PHP session ID
    while(_client.available()) {
        readLine();

        #ifdef NEW
        n = -1;
        //-9 do PHPSSID e -20 do pass
        for(i=0;i<posBuffer-(9);i++){
            //procura o header
            if( (readBuffer[i+0] == 'P') &&
                (readBuffer[i+1] == 'H') &&
                (readBuffer[i+2] == 'P') &&
                (readBuffer[i+3] == 'S') &&
                (readBuffer[i+4] == 'E') &&
                (readBuffer[i+5] == 'S') &&
                (readBuffer[i+6] == 'S') &&
                (readBuffer[i+7] == 'I') &&
                (readBuffer[i+8] == 'D') &&
                (readBuffer[i+9] == '=')
              ){
                //encontrou, agora eh copiar o cookie
                //for (n=0; n<32; n++){
                for (n=i+10; readBuffer[n] != ';'; n++){
                  PHPSESSID[n-i-10]= readBuffer[n];
                }
                #ifdef DEBUGPRINT
                Serial.print(F("PHPSESSID encontrado: "));
                Serial.println(PHPSESSID);

                #endif
                break;
            }
        }
        if(n != -1){
            break;
        }
        #endif

        #ifndef NEW
        n = buff.indexOf("PHPSESSID=");
        if(n != -1){
            //OK. Le caracter por caracter ate encontrar  o ponto-virgula.
            for (i=n+10; buff.charAt(i) !=';'; i++){
                PHPSESSID[i-n-10]= buff.charAt(i);
            }
            break;
        }
        #endif
    }
    #ifdef DEBUGPRINT
    Serial.println(F("Verificando Sucessfull"));
    #endif

    //procura se conexao foi "sucessfull"
    while(_client.available()) {
        readLine();


        #ifdef NEW
        result = -1;
        //-10 do Successfull
        for(i=0;i<posBuffer-10;i++){
            //procura o header
            if( (readBuffer[i] == 'S') &&
                (readBuffer[i+1] == 'u') &&
                (readBuffer[i+2] == 'c') &&
                (readBuffer[i+3] == 'c') &&
                (readBuffer[i+4] == 'e') &&
                (readBuffer[i+5] == 's') &&
                (readBuffer[i+6] == 's') &&
                (readBuffer[i+7] == 'f') &&
                (readBuffer[i+8] == 'u') &&
                (readBuffer[i+9] == 'l') &&
                (readBuffer[i+10] == 'l')
              ){
                //encontrou, sai do for
                result = true;
                break;
            }
        }
        #endif

        #ifndef NEW
        result = buff.indexOf("Successfull") != -1;
        #endif

        if(result){
          break;
        }
    }

    _client.flush();
    _client.stop();

    #ifdef DEBUGPRINT
        Serial.print(F("Dados recebidos em [ms]: "));
        Serial.println(millis() - time);
        Serial.println(result);

        if(!result) {
           Serial.println(F("Authentication Failed!"));
        }
        Serial.println(PHPSESSID);
    #endif
    return result;
}


String SmartClient::readResponse() {
    //bool result = false;
    int maxAttempts = 300, attempts = 0;
    int time = millis();
    //TODO remove line
    String handshake = "", line;
#ifdef NEW
    char character;
#endif
    #ifdef DEBUGPRINT
        Serial.println(F("Smart readResponse 2"));
    #endif

    while(_client.available() == 0 && attempts < maxAttempts){
        delay(100);
        attempts++;
    }

    while(_client.available()) {
        readLine();
        handshake += readBuffer + '\n';
    }
    _client.stop();

    #ifdef DEBUGPRINT
        Serial.print(F("Dados recebidos em [ms]: "));
        Serial.println(millis() - time);
        Serial.println(handshake);
    #endif
    return handshake;
}

void SmartClient::readLine() {
    char character;
    buff = "";
    #ifdef NEW
    posBuffer = 0;
    #endif
    while(_client.available() > 0 && (character = _client.read()) != '\n') {
        if (character != '\r' && character != -1) {
            #ifndef NEW
            buff += character;
            #endif
            #ifdef NEW
            readBuffer[posBuffer] = character;
            posBuffer++;
            //if(posBuffer>1000) posBuffer = 0;
            #endif
        }
    }
    #ifdef NEW
    readBuffer[posBuffer]= '\0';
      #ifdef DEBUGPRINT
      Serial.println(readBuffer);
      #endif
    #endif

    #ifdef DEBUGPRINT
        Serial.println(buff);
    #endif
}

void SmartClient::send (bool GoP, char hostname[], char app[], char schema[], char caminho[], String PostData) {
   //DADOS A SEREM ENVIADOS!
    //_client.flush();
    if(GoP==true){
   if (_client.connect(hostname, 80)) {

    #ifdef DEBUGPRINT
        Serial.println(F("Sending Data POST"));
    #endif

    _client.print(F("POST /api/fp/exec/json/"));
    _client.print(app);
    _client.print(F("/"));
    _client.print(schema);
    _client.print(F("/"));
    _client.print(caminho);
    _client.println(F("/ HTTP/1.1"));
    _client.println(F("Host: www.smartapps.com.br"));
    _client.print(F("Authorization: Basic "));
    _client.println(AuthID);
    _client.print(F("Cookie: PHPSESSID="));
    _client.println(PHPSESSID);
    _client.println(F("Content-Type: application/x-www-form-urlencoded"));
    _client.print(F("Content-Length: "));
    _client.println(PostData.length());
    _client.println(F("Connection: close"));
    _client.println();
    _client.print(PostData);
    _client.println();
    #ifdef DEBUGPRINT
        Serial.println(F("Data Sent"));
        Serial.println(PHPSESSID);
    #endif
    //se tirar essa linha soh manda 1 vez
    _client.stop();
     }
    }
    if(GoP==false){
        if (_client.connect(hostname, 80)) {

#ifdef DEBUGPRINT
            Serial.println(F("Sending Data GET"));
#endif
            _client.print(F("GET /api/fp/"));//Normal é GET /api/fp/from/
            _client.print(app);
            _client.print(F("/"));
            _client.print(LOGIN);
            _client.print(F("/"));
            _client.print(caminho);
            _client.println(F("/ HTTP/1.1"));
            _client.println(F("Host: www.smartapps.com.br"));
            _client.print(F("Authorization: Basic "));
            _client.println(AuthID);
            _client.print(F("Cookie: PHPSESSID="));
            _client.println(PHPSESSID);
            //_client.println(F("Content-Type: application/x-www-form-urlencoded"));
            _client.println(F("Connection: close"));
            _client.println();
#ifdef DEBUGPRINT
            Serial.println(F("Data Sent"));
#endif
            //se tirar essa linha soh manda 1 vez
            //_client.stop();
        }
}

}
