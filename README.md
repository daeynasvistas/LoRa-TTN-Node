<a href="http://mcm.ipg.pt"><img src="http://www.ipg.pt/website/imgs/logotipo_ipg.jpg" title="IPG(MCM)" alt="IPG MCM 2018/19"></a>

![ezgif-1-477448af6497](https://user-images.githubusercontent.com/2634610/60972531-61670a80-a31e-11e9-81d3-3562b2e46e6f.gif)

# The Things Network Node LoRa Arduino UNO

Os dados ficam muito mais bonitos quando os pode visualizar. Este projeto torna fácil enviar dados de sensores de nodes LoRaWAN conectados à rede Things. 

## Alterar em cada Node (Main.cpp)
 ````
static const PROGMEM u1_t NWKSKEY[16] = { 0x82, 0x60 };  //COLOCAR AQUI os secrets
static const u1_t PROGMEM APPSKEY[16] = { 0x59, 0x76 }; //COLOCAR AQUI os secrets
static const u4_t DEVADDR = 0x26011874; // <-- Change this address for every node!
 ````

# Como instalar

## 1. Foi utilizado VisualStudio Code + PlatformIO (mas pode editar Main.cpp com Arduino IDE)
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
