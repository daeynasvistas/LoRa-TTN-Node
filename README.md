<a href="http://mcm.ipg.pt"><img src="http://www.ipg.pt/website/imgs/logotipo_ipg.jpg" title="IPG(MCM)" alt="IPG MCM 2018/19"></a>

![ezgif-1-477448af6497](https://user-images.githubusercontent.com/2634610/60972531-61670a80-a31e-11e9-81d3-3562b2e46e6f.gif)

# The Things Network Node LoRa Arduino UNO

Os dados ficam muito mais bonitos quando os pode visualizar. Este projeto torna fácil enviar dados de sensores de nodes LoRaWAN conectados à rede Things. 

Neste repositório encontra um step by step para registar um Node (Arduino UNO) na plataform TTN e a forma de enviar para esta os valores dos sensores: GPS, Humidade, Temperatura e luminosidade.


## Alterar em cada Node (Main.cpp)
 ```` C++
static const PROGMEM u1_t NWKSKEY[16] = { 0x82, 0x60 };  //COLOCAR AQUI "Network Session Key" (ver screenshot)
static const u1_t PROGMEM APPSKEY[16] = { 0x59, 0x76 };  //COLOCAR AQUI "App Session Key"
static const u4_t DEVADDR = 0x26011874;                  //COLOCAR AQUI "0xDevice Address"
 ````

## Alterar em cada Node (Main.cpp)
indicar de forma implícita a freq. que configurou no dragino, neste caso "868300000"
 ````C++
    #if defined(CFG_eu868)
    //LMIC_setupChannel(0, 868100000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
    LMIC_setupChannel(1, 868300000, DR_RANGE_MAP(DR_SF12, DR_SF7B), BAND_CENTI);      // g-band
    //LMIC_setupChannel(2, 868500000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
    //LMIC_setupChannel(3, 867100000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
    //LMIC_setupChannel(4, 867300000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
    //LMIC_setupChannel(5, 867500000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
    //LMIC_setupChannel(6, 867700000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
    //LMIC_setupChannel(7, 867900000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
    //LMIC_setupChannel(8, 868800000, DR_RANGE_MAP(DR_FSK,  DR_FSK),  BAND_MILLI);      // g2-band
 ````

# Como instalar

## 1. Clonar repositório 
(Foi utilizado VisualStudio Code + PlatformIO, mas pode editar Main.cpp com Arduino IDE)

 ````
 $ git clone https://github.com/daeynasvistas/LoRa-TTN-Node
 ````
 
 Librarias utilizadas
  ````
 lib_deps =
     # Depend on specific version
     DHTlib
     LMIC-Arduino@1.5.0+arduino-2
     TinyGPS
  ````
 
## 2. Configurar Gateway Dragino:

![2](https://user-images.githubusercontent.com/2634610/60979177-28349780-a32a-11e9-8a4e-4e61da2ec065.PNG)
![3](https://user-images.githubusercontent.com/2634610/60979176-28349780-a32a-11e9-8e82-f1761ff73820.PNG)

Utilizar MAC do Gateway com ID:
'''
a840411bc834ffff  <- adicionar f até 19bit
'''

## 3. Na Consola TTN
### 3.1 Registar uma nova GATEWAY

![4](https://user-images.githubusercontent.com/2634610/60979935-7302df00-a32b-11e9-8283-677316e516a7.png)
![5](https://user-images.githubusercontent.com/2634610/60980019-9b8ad900-a32b-11e9-85bd-c030760e7c3f.PNG)
![6](https://user-images.githubusercontent.com/2634610/60980101-c2490f80-a32b-11e9-9398-1541cc9e1632.PNG)

### 3.2 Adicionar uma nova aplicação

![7](https://user-images.githubusercontent.com/2634610/60980247-05a37e00-a32c-11e9-9ac9-ce3cfaa22ae8.PNG)
![8](https://user-images.githubusercontent.com/2634610/60980437-6337ca80-a32c-11e9-9ac8-792edc661a5c.PNG)

#### Obtemos Assim a chave da applicação (App ID)
![9](https://user-images.githubusercontent.com/2634610/60980508-84002000-a32c-11e9-8a67-de1345c74486.PNG)

## 4. Registar um novo Node (na aplicação anteriormente criada)
![10](https://user-images.githubusercontent.com/2634610/60980723-e3f6c680-a32c-11e9-96d6-f858a3a0b249.PNG)

### 4.1 Obtemos as chaves necessárias aqui
![11](https://user-images.githubusercontent.com/2634610/60980966-61223b80-a32d-11e9-87c5-328d7a43f843.png)

## 5. Foi utilizado um Payload "custom"
![12](https://user-images.githubusercontent.com/2634610/60981751-d2162300-a32e-11e9-9ac4-ad43772cf6a8.PNG)

### 5.1 Payload Custom

  ````JS
function Bytes2Float32(bytes) {
    var sign = (bytes & 0x80000000) ? -1 : 1;
    var exponent = ((bytes >> 23) & 0xFF) - 127;
    var significand = (bytes & ~(-1 << 23));

    if (exponent == 128) 
        return sign * ((significand) ? Number.NaN : Number.POSITIVE_INFINITY);

    if (exponent == -127) {
        if (significand == 0) return sign * 0.0;
        exponent = -126;
        significand /= (1 << 22);
    } else significand = (significand | (1 << 23)) / (1 << 23);

    return sign * significand * Math.pow(2, exponent);
}

// Test using 0xFF1641 for -23.4 and 65%, or 0x00EA41 for +23.4 and 65%
function Decoder(bytes, port) {
	var lat = bytes[3] << 24 | bytes[2] << 16 | bytes[1] << 8 | bytes[0];
  var lon = bytes[7] << 24 | bytes[6] << 16 | bytes[5] << 8 | bytes[4];
	
  var lux =  bytes[8] << 8 | bytes[9];

  var temp = bytes[10] << 24 >>16 | bytes[11];
  var hum = bytes[12] << 8 | bytes[13];
  

  return {
    latitude:  Bytes2Float32(lat),
		longitude: Bytes2Float32(lon),
    Temperatura: (temp/100),
    Humidade: (hum/100),
    Luminosidade: (lux),		
  };
}
  ````
